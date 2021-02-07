#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <vector>

#include "addressregister.h"
#include "controller.h"
#include "register.h"
#include "registers.h"

MicroCodeRunner::MicroCodeRunner(SystemBus *bus, const MicroCode *microCode)  :
    m_bus(bus), mc(microCode), steps() {

  evaluateCondition();
  fetchSteps();
  if (!(mc -> addressingMode & AddressingMode::Done)) {
    int ix = -1;
    do {
      ix++;
      steps.emplace_back(mc->steps[ix]);
    } while (mc -> steps[ix].opflags & SystemBus::Done);
  }
}

void MicroCodeRunner::evaluateCondition() {
  byte valid_byte;
  switch (mc -> condition_op) {
    case MicroCode::And:
      valid_byte = m_bus -> flags() & mc -> condition;
      break;
    case MicroCode::Nand:
      valid_byte = (!(m_bus -> flags())) & mc -> condition;
      break;
    default:
      valid_byte = true;
      break;
  }
  valid = valid_byte != 0;
}

void MicroCodeRunner::fetchSteps() {
  switch (mc -> addressingMode & AddressingMode::Mask) {
    case DirectByte:
      fetchDirectByte();
      break;
    case DirectWord:
      fetchDirectWord();
      break;
    case AbsoluteByte:
      fetchAbsoluteByte();
      break;
    case AbsoluteWord:
      fetchAbsoluteWord();
      break;
    default:
      break;
  }
}

void MicroCodeRunner::fetchDirectByte() {
  byte target = (valid) ? mc -> target : TX;
  steps.push_back({ MicroCode::Action::XADDR, PC, MEMADDR, SystemBus::Inc });
  steps.push_back({ MicroCode::Action::XDATA, MEM, target, SystemBus::None });
}

void MicroCodeRunner::fetchDirectWord() {
  byte target = (valid) ? mc -> target : TX;
  steps.push_back({ MicroCode::Action::XADDR, PC, MEMADDR, SystemBus::Inc });
  steps.push_back({ MicroCode::Action::XDATA, MEM, target, SystemBus::None });
  steps.push_back({ MicroCode::Action::XADDR, PC, MEMADDR, SystemBus::Inc });
  steps.push_back({ MicroCode::Action::XDATA, MEM, target, SystemBus::MSB });
}

void MicroCodeRunner::fetchAbsoluteByte() {
  steps.push_back({ MicroCode::Action::XADDR, PC, MEMADDR, SystemBus::Inc });
  steps.push_back({ MicroCode::Action::XDATA, MEM, TX, SystemBus::None });
  steps.push_back({ MicroCode::Action::XADDR, PC, MEMADDR, SystemBus::Inc });
  steps.push_back({ MicroCode::Action::XDATA, MEM, TX, SystemBus::MSB });
  if (valid) {
    steps.push_back({ MicroCode::Action::XADDR, TX, MEMADDR, SystemBus::None });
    steps.push_back({ MicroCode::Action::XDATA, MEM, mc -> target, SystemBus::None });
  }
}

void MicroCodeRunner::fetchAbsoluteWord() {
  steps.push_back({ MicroCode::Action::XADDR, PC, MEMADDR, SystemBus::Inc });
  steps.push_back({ MicroCode::Action::XDATA, MEM, TX, SystemBus::None });
  steps.push_back({ MicroCode::Action::XADDR, PC, MEMADDR, SystemBus::Inc });
  steps.push_back({ MicroCode::Action::XDATA, MEM, TX, SystemBus::MSB });
  if (valid) {
    steps.push_back({ MicroCode::Action::XADDR, TX, MEMADDR, SystemBus::Inc });
    steps.push_back({ MicroCode::Action::XDATA, MEM, mc->target, SystemBus::None });
    steps.push_back({ MicroCode::Action::XADDR, TX, MEMADDR, SystemBus::None });
    steps.push_back({ MicroCode::Action::XDATA, MEM, mc->target, SystemBus::MSB });
  }
}

word MicroCodeRunner::grabConstant(int step) {
  switch (mc -> addressingMode) {
    case DirectByte:
      if (step == 2) {
        m_constant = m_bus -> readDataBus();
      }
      break;
    case DirectWord:
    case AbsoluteByte:
    case AbsoluteWord:
      if (step == 2) {
        m_constant = m_bus -> readDataBus();
      } else if (step == 4) {
        m_constant |= (((word) m_bus -> readDataBus()) << 8);
      }
      break;
    default:
      break;
  }
  return m_constant;
}

SystemError MicroCodeRunner::executeNextStep(int step) {
  MicroCode::MicroCodeStep s = steps[step];
  switch (s.action) {
    case MicroCode::XDATA:
      m_bus -> xdata(s.src, s.target, s.opflags & SystemBus::Mask);
      break;
    case MicroCode::XADDR:
      m_bus->xaddr(s.src, s.target, s.opflags & SystemBus::Mask);
      break;
    case MicroCode::OTHER:
      switch (s.opflags & SystemBus::Mask) {
        case SystemBus::Halt:
          std::cerr << "Halting system" << std::endl;
          m_bus->stop();
          break;
        default:
          std::cerr << "Unhandled operation flag '" << std::hex << s.opflags
                    << "' for instruction " << std::hex << mc -> opcode << " step "
                    << std::dec << step << std::endl;
          return InvalidMicroCode;
      }
      break;
    default:
      std::cerr << "Unhandled microcode action '" << std::hex << s.action
                << "' for instruction " << std::hex << mc -> opcode << " step "
                << std::dec << step << std::endl;
      return InvalidMicroCode;
  }
  return NoError;
}

bool MicroCodeRunner::hasStep(int step) {
  return steps.size() > step;
}

std::string MicroCodeRunner::instruction() const {
  if (strchr(mc -> instruction, '%')) {
    char buf[32];
    snprintf(buf, 32, mc -> instruction, m_constant);
    return buf;
  } else {
    return mc -> instruction;
  }
}

word MicroCodeRunner::constant() const {
  return m_constant;
}


// -----------------------------------------------------------------------

Controller::Controller(const MicroCode *mc) : Register(IR), microCode(mc) {
}

std::string Controller::instruction() const {
  if (m_runner) {
    return m_runner -> instruction();
  } else {
    return "----";
  }
}

word Controller::constant() const {
  if (m_runner) {
    return m_runner -> constant();
  } else {
    return 0;
  }
}

void Controller::setRunMode(RunMode runMode) {
  m_runMode = runMode;
}

SystemError Controller::status() {
  printf("%1x. IR %02x %04x %-15.15s Step %d\n", id(), getValue(), constant(), instruction().c_str(), step);
  return NoError;
}

SystemError Controller::reset() {
  step = 0;
  this -> Register::reset();
  return NoError;
}

SystemError Controller::onHighClock() {
  this -> Register::onHighClock();
  if (m_runner) {
    m_runner -> grabConstant(step - 2);
  }
  return NoError;
}

SystemError Controller::onLowClock() {
  const MicroCode *mc;
  switch (step) {
    case 0:
      bus()->xaddr(PC, MEMADDR, SystemBus::Inc);
      break;
    case 1:
      bus()->xdata(MEM, IR, SystemBus::None);
      break;
    case 2:
      mc = microCode + getValue();
      if (mc->opcode != getValue()) {
        std::cerr << "Microcode mismatch for opcode " << std::hex << getValue()
                  << ": 'opcode' field contains " << std::hex << mc->opcode
                  << std::endl;
        return InvalidMicroCode;
      }
      m_runner = new MicroCodeRunner(bus(), mc);
      // fall through:
    default:
      if (m_runner -> hasStep(step - 2)) {
        auto err = m_runner->executeNextStep(step - 2);
        if (err != NoError) {
          return err;
        }
      } else {
        step = 0;
        delete m_runner;
        m_runner = nullptr;
        setValue(0);
        if (runMode() == BreakAtInstruction) {
          bus() -> stop();
        } else {
          bus()->xaddr(PC, MEMADDR, SystemBus::Inc);
        }
      }
      break;
  }
  step++;
  sendEvent(EV_STEPCHANGED);
  if (runMode() == BreakAtClock) {
    bus() -> stop();
  }
  return NoError;
}