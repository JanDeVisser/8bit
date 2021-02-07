#include "addressregister.h"
#include "controller.h"
#include "memory.h"
#include "backplane.h"
#include "register.h"

#include "microcode.inc"

byte mem[] = { MOV_A_CONST, 0x42, MOV_B_A, 0xFF };
MemImage image = {
  .address = 0x00, .size = 0x04, .contents = mem
};

BackPlane::BackPlane() : components(), clock(this, 0.001) {
  systemBus = new SystemBus();
  components.resize(16);
}

void BackPlane::defaultSetup() {
  insert(new Register(GP_A));             // 0x00
  insert(new Register(GP_B));             // 0x01
  insert(new Register(GP_C));             // 0x02
  insert(new Register(GP_D));             // 0x03
  insert(new Controller(mc));             // 0x06
  insert(new AddressRegister(PC, "PC"));  // 0x08
  insert(new AddressRegister(SP, "SP"));  // 0x09
  insert(new AddressRegister(Si, "Si"));  // 0x0A
  insert(new AddressRegister(Di, "Di"));  // 0x0B
  insert(new AddressRegister(TX, "TX"));  // 0x0C
  insert(new Memory(0x0000, 0x8000, 0x8000, 0x8000, &image));    // 0x0F
}

void BackPlane::insert(ConnectedComponent *comp) {
  comp->bus(systemBus);
  components[comp->id()] = comp;
}

ConnectedComponent * BackPlane::componentByID(int id) const {
  return (id == 0x07) ? components.at(0x0F) : components.at(id);
}

Controller::RunMode BackPlane::runMode() const {
  auto c = dynamic_cast<Controller *>(componentByID(IR));
  return c -> runMode();
}

void BackPlane::setRunMode(Controller::RunMode runMode) const {
  auto c = dynamic_cast<Controller *>(componentByID(IR));
  return c -> setRunMode(runMode);
}

Controller * BackPlane::controller() const {
  return dynamic_cast<Controller *>(componentByID(IR));
}

void BackPlane::run() {
  if (controller() -> getValue() == HLT) {
    reset();
  }
  clock.start();
}

SystemError BackPlane::reportError() {
  if (error == NoError) {
    return NoError;
  }
  std::cout << "EXCEPTION " << error << std::endl;
  clock.stop();
  return error;
}

SystemError BackPlane::reset() {
  if (error != NoError) {
    return error;
  }
  systemBus -> reset();
  for (auto & component : components) {
    if (!component) continue;
    error = component -> reset();
    if (error != NoError) {
      return reportError();
    }
  }
  return NoError;
}

SystemError BackPlane::status() {
  error = systemBus -> status();
  if (error == NoError) {
    for (auto &component : components) {
      if (!component) continue;
      error = component->status();
      if (error != NoError) {
        return reportError();
      }
    }
  }
  return NoError;
}

SystemError BackPlane::onRisingClockEdge() {
  if (error != NoError) {
    return error;
  }
  status();
  for (auto & component : components) {
    if (!component) continue;
    error = component -> onRisingClockEdge();
    if (error != NoError) {
      return reportError();
    }
  }
  return NoError;
}

SystemError BackPlane::onHighClock() {
  if (error != NoError) {
    return error;
  }
  for (auto & component : components) {
    if (!component) continue;
    error = component -> onHighClock();
    if (error != NoError) {
      return reportError();
    }
  }
  if (!systemBus -> halt()) {
    stop();
  }
  return NoError;
}

SystemError BackPlane::onFallingClockEdge() {
  if (error != NoError) {
    return error;
  }
  for (auto & component : components) {
    if (!component) continue;
    error = component -> onFallingClockEdge();
    if (error != NoError) {
      return reportError();
    }
  }
  return NoError;
}

SystemError BackPlane::onLowClock() {
  if (error != NoError) {
    return error;
  }
  for (auto & component : components) {
    if (!component) continue;
    error = component -> onLowClock();
    if (error != NoError) {
      return reportError();
    }
  }
  if (!systemBus -> halt()) {
    stop();
  }
  return NoError;
}


