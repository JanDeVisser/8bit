#include <iostream>
#include <iomanip>
#include <cstdio>

#include "addressregister.h"
#include "controller.h"
#include "register.h"
#include "backplane.h"

Controller::Controller(MicroCode *mc) : Register(IR), microCode(mc) {
//  for (int ix = 0; mc[ix].instruction; ix++) {
//    //
//  }
}

std::string Controller::instruction() const {
  return (microCode[getValue()].opcode != getValue()) ? "hlt" : microCode[getValue()].instruction;
}

SystemError Controller::status() {
  printf("IR %02x Step %d\n", getValue(), step);
  return NoError;
}

SystemError Controller::reset() {
  this -> Register::reset();
  step = 0;
  return NoError;
}

const MicroCode::MicroCodeStep & Controller::findMicroCodeStep() {
  return microCode[getValue()].steps[step - 2];
}

SystemError Controller::onLowClock() {
  switch (step) {
    case 0:
      bus()->xaddr(PC, MEMADDR, SystemBus::OP_INC);
      break;
    case 1:
      bus()->xdata(MEM, IR, SystemBus::OP_NONE);
      break;
    default:
      MicroCode::MicroCodeStep mc = findMicroCodeStep();
      if (mc.action) {
        if (mc.opflags & SystemBus::OP_DONE) {
          step = -1;
        }
        switch (mc.action) {
          case MicroCode::XDATA:
            bus()->xdata(mc.src, mc.target, mc.opflags & SystemBus::OP_MASK);
            break;
          case MicroCode::XADDR:
            bus()->xaddr(mc.src, mc.target, mc.opflags & SystemBus::OP_MASK);
            break;
          case MicroCode::OTHER:
            switch (mc.opflags & SystemBus::OP_MASK) {
              case SystemBus::OP_HALT:
                fprintf(stderr, "Halting system\n");
                bus()->stop();
                break;
              default:
                fprintf(stderr, "Unhandled operation flag '%02x' for instruction %02x, step %d\n",
                        mc.opflags, getValue(), step);
                return InvalidMicroCode;
            }
            break;
          default:
            fprintf(stderr, "Unhandled microcode action %d for instruction %02x, step %d\n",
                    mc.action, getValue(), step);
            return InvalidMicroCode;
        }
      } else {
        fprintf(stderr, "No microcode for instruction %02x, step %d\n", getValue(), step);
        return InvalidInstruction;
      }
      break;
  }
  step++;
  sendEvent(EV_STEPCHANGED);
  return NoError;
}
