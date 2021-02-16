#include "addressregister.h"
#include "alu.h"
#include "controller.h"
#include "memory.h"
#include "backplane.h"
#include "register.h"

#include "microcode.inc"

byte mem[] = {
  /* 0x0000 */ CLR_A,
  /* 0x0001 */ CLR_B,
  /* 0x0002 */ MOV_C_CONST, 0x01,
  /* 0x0004 */ CLR_D,
  /* 0x0005 */ MOV_SI_CONST, 0x17, 0x00,
  /* 0x0008 */ ADD_AB_CD,
  /* 0x0009 */ SWP_A_C,
  /* 0x000A */ SWP_B_D,
  /* 0x000B */ DEC_SI,
  /* 0x000C */ JNZ, 0x08, 0x00,
  /* 0x000F */ MOV_DI_CD,
  /* 0x0010 */ HLT
};
MemImage image = {
  .address = 0x00, .size = 0x11, .contents = mem
};

BackPlane::BackPlane() : clock(this, 1.0) {
}

void BackPlane::defaultSetup() {
  insert(new Register(GP_A));             // 0x00
  insert(new Register(GP_B));             // 0x01
  insert(new Register(GP_C));             // 0x02
  insert(new Register(GP_D));             // 0x03
  auto lhs = new Register(LHS);
  insert(lhs);                         // 0x04
  insert(new ALU(RHS, lhs)); // 0x05
  insert(new Controller(mc));             // 0x06
  insert(new AddressRegister(PC, "PC"));  // 0x08
  insert(new AddressRegister(SP, "SP"));  // 0x09
  insert(new AddressRegister(Si, "Si"));  // 0x0A
  insert(new AddressRegister(Di, "Di"));  // 0x0B
  insert(new AddressRegister(TX, "TX"));  // 0x0C
  insert(new Memory(0x0000, 0x8000, 0x8000, 0x8000, &image));    // 0x0F
}

Controller::RunMode BackPlane::runMode() const {
  auto c = dynamic_cast<Controller *>(component(IR));
  return c -> runMode();
}

void BackPlane::setRunMode(Controller::RunMode runMode) const {
  auto c = dynamic_cast<Controller *>(component(IR));
  return c -> setRunMode(runMode);
}

Controller * BackPlane::controller() const {
  return dynamic_cast<Controller *>(component(IR));
}

void BackPlane::run() {
  if (controller() -> getValue() == HLT) {
    reset();
  }
  clock.start();
}

SystemError BackPlane::reportError() {
  if (error() == NoError) {
    return NoError;
  }
  std::cout << "EXCEPTION " << error() << std::endl;
  clock.stop();
  return error();
}

SystemError BackPlane::onClockEvent(const ComponentHandler& handler) {
  if (error() != NoError) {
    return error();
  }
  switch (m_phase) {
    case SystemClock:
      return forAllComponents(handler);
    case IOClock:
      // xx
    default:
      return NoError;
  }
}

SystemError BackPlane::reset() {
  if (error() != NoError) {
    return error();
  }
  error(bus().reset());
  if (error() == NoError) {
    forAllComponents([](Component *c) -> SystemError {
      return (c) ? c -> reset() : NoError;
    });
  }
  return NoError;
}

SystemError BackPlane::status() {
  error(bus().status());
  if (error() == NoError) {
    forAllComponents([](Component *c) -> SystemError {
      return (c) ? c -> status() : NoError;
    });
  }
  return NoError;
}

SystemError BackPlane::onRisingClockEdge() {
  if (error() != NoError) {
    return error();
  }
  if ((m_phase == SystemClock) && ((error(status())) != NoError)) {
    return reportError();
  }
  return onClockEvent([](Component *c) -> SystemError {
      return (c) ? c -> onRisingClockEdge() : NoError;
    });
}


SystemError BackPlane::onHighClock() {
  error(onClockEvent([](Component *c) -> SystemError {
      return (c) ? c -> onHighClock() : NoError;
    }));
  if ((error() == NoError) && !bus().halt()) {
    stop();
  }
  return error();
}

SystemError BackPlane::onFallingClockEdge() {
  return onClockEvent([](Component *c) -> SystemError {
    return (c) ? c -> onFallingClockEdge() : NoError;
  });
}

SystemError BackPlane::onLowClock() {
  error(onClockEvent([](Component *c) -> SystemError {
    return (c) ? c -> onLowClock() : NoError;
  }));
  if ((error() == NoError) && !bus().halt()) {
    stop();
  }
  m_phase = (m_phase == SystemClock) ? IOClock : SystemClock;
  return error();
}
