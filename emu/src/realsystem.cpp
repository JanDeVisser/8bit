#include "addressregister.h"
#include "controller.h"
#include "memory.h"
#include "realsystem.h"
#include "register.h"

enum OpCode {
  NOP             = 0x00,
  MOV_A_CONST     = 0x01,
  MOV_A_ADDR      = 0x02,
  MOV_A_B         = 0x03,
  MOV_A_C         = 0x04,
  MOV_A_D         = 0x05,

  MOV_B_CONST     = 0x06,
  MOV_B_ADDR      = 0x07,
  MOV_B_A         = 0x08,
  MOV_B_C         = 0x09,
  MOV_B_D         = 0x0A,

  MOV_C_CONST     = 0x0B,
  MOV_C_ADDR      = 0x0C,
  MOV_C_A         = 0x0D,
  MOV_C_B         = 0x0E,
  MOV_C_D         = 0x0F,

  MOV_D_CONST     = 0x10,
  MOV_D_ADDR      = 0x11,
  MOV_D_A         = 0x12,
  MOV_D_B         = 0x13,
  MOV_D_C         = 0x14,

  MOV_SP_CONST    = 0x15,
  MOV_SP_ADDR     = 0x16,
  MOV_SP_CD       = 0x17,


  MOV_SI_CONST    = 0x18,
  MOV_SI_ADDR     = 0x19,
  MOV_SI_CD       = 0x1A,

  MOV_A_SIC       = 0x1B,
  MOV_B_SIC       = 0x1C,
  MOV_A_SID       = 0x1D,
  MOV_B_SID       = 0x1E,

  MOV_DI_CONST    = 0x1F,
  MOV_DI_ADDR     = 0x20,
  MOV_DI_CD       = 0x21,

  MOV_A_DIC       = 0x22,
  MOV_B_DIC       = 0x23,
  MOV_A_DID       = 0x24,
  MOV_B_DID       = 0x25,


};

static MicroCode mc[] = {
  { .instruction = NOP, .step = 2, .action = MicroCode::XDATA, .src = GP_A, .target = GP_A, .opflags = OP_DONE },

  { .instruction = 0x01, .step = 2, .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = OP_INC },
  { .instruction = 0x01, .step = 3, .action = MicroCode::XDATA, .src = MEM, .target = GP_A, .opflags = OP_DONE },

  { .instruction = 0x02, .step = 2, .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = OP_INC },
  { .instruction = 0x02, .step = 3, .action = MicroCode::XDATA, .src = MEM, .target = TX, .opflags = OP_NONE },
  { .instruction = 0x02, .step = 4, .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = OP_INC },
  { .instruction = 0x02, .step = 5, .action = MicroCode::XDATA, .src = MEM, .target = TX, .opflags = OP_MSB },
  { .instruction = 0x02, .step = 6, .action = MicroCode::XADDR, .src = TX, .target = MEMADDR, .opflags = OP_DONE },
  { .instruction = 0x02, .step = 7, .action = MicroCode::XDATA, .src = MEM, .target = GP_A, .opflags = OP_DONE },

  { .instruction = 0x03, .step = 2, .action = MicroCode::XDATA, .src = GP_B, .target = GP_A, .opflags = OP_DONE },
  { .instruction = 0x04, .step = 2, .action = MicroCode::XDATA, .src = GP_C, .target = GP_A, .opflags = OP_DONE },
  { .instruction = 0x05, .step = 2, .action = MicroCode::XDATA, .src = GP_D, .target = GP_A, .opflags = OP_DONE },

  { .instruction = 0x06, .step = 2, .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = OP_INC },
  { .instruction = 0x06, .step = 3, .action = MicroCode::XDATA, .src = MEM, .target = GP_B, .opflags = OP_DONE },

  { .instruction = 0x07, .step = 2, .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = OP_INC },
  { .instruction = 0x07, .step = 3, .action = MicroCode::XDATA, .src = MEM, .target = TX, .opflags = OP_NONE },
  { .instruction = 0x07, .step = 4, .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = OP_INC },
  { .instruction = 0x07, .step = 5, .action = MicroCode::XDATA, .src = MEM, .target = TX, .opflags = OP_MSB },
  { .instruction = 0x07, .step = 6, .action = MicroCode::XADDR, .src = TX, .target = MEMADDR, .opflags = OP_DONE },
  { .instruction = 0x07, .step = 7, .action = MicroCode::XDATA, .src = MEM, .target = GP_B, .opflags = OP_DONE },

  { .instruction = 0x08, .step = 2, .action = MicroCode::XDATA, .src = GP_A, .target = GP_B, .opflags = OP_DONE },
  { .instruction = 0x09, .step = 2, .action = MicroCode::XDATA, .src = GP_C, .target = GP_B, .opflags = OP_DONE },
  { .instruction = 0x0A, .step = 2, .action = MicroCode::XDATA, .src = GP_D, .target = GP_B, .opflags = OP_DONE },

  { .instruction = 0x0B, .step = 2, .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = OP_INC },
  { .instruction = 0x0B, .step = 3, .action = MicroCode::XDATA, .src = MEM, .target = GP_B, .opflags = OP_DONE },

  { .instruction = 0x0C, .step = 2, .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = OP_INC },
  { .instruction = 0x0C, .step = 3, .action = MicroCode::XDATA, .src = MEM, .target = TX, .opflags = OP_NONE },
  { .instruction = 0x0C, .step = 4, .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = OP_INC },
  { .instruction = 0x0C, .step = 5, .action = MicroCode::XDATA, .src = MEM, .target = TX, .opflags = OP_MSB },
  { .instruction = 0x0C, .step = 6, .action = MicroCode::XADDR, .src = TX, .target = MEMADDR, .opflags = OP_DONE },
  { .instruction = 0x0C, .step = 7, .action = MicroCode::XDATA, .src = MEM, .target = GP_C, .opflags = OP_DONE },

  { .instruction = 0x0D, .step = 2, .action = MicroCode::XDATA, .src = GP_A, .target = GP_C, .opflags = OP_DONE },
  { .instruction = 0x0E, .step = 2, .action = MicroCode::XDATA, .src = GP_B, .target = GP_C, .opflags = OP_DONE },
  { .instruction = 0x0F, .step = 2, .action = MicroCode::XDATA, .src = GP_D, .target = GP_C, .opflags = OP_DONE },

  { .instruction = 0x10, .step = 2, .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = OP_INC },
  { .instruction = 0x10, .step = 3, .action = MicroCode::XDATA, .src = MEM, .target = GP_D, .opflags = OP_DONE },

  { .instruction = 0x11, .step = 2, .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = OP_INC },
  { .instruction = 0x11, .step = 3, .action = MicroCode::XDATA, .src = MEM, .target = TX, .opflags = OP_NONE },
  { .instruction = 0x11, .step = 4, .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = OP_INC },
  { .instruction = 0x11, .step = 5, .action = MicroCode::XDATA, .src = MEM, .target = TX, .opflags = OP_MSB },
  { .instruction = 0x11, .step = 6, .action = MicroCode::XADDR, .src = TX, .target = MEMADDR, .opflags = OP_DONE },
  { .instruction = 0x11, .step = 7, .action = MicroCode::XDATA, .src = MEM, .target = GP_D, .opflags = OP_DONE },

  { .instruction = 0x12, .step = 2, .action = MicroCode::XDATA, .src = GP_A, .target = GP_D, .opflags = OP_DONE },
  { .instruction = 0x13, .step = 2, .action = MicroCode::XDATA, .src = GP_B, .target = GP_D, .opflags = OP_DONE },
  { .instruction = 0x14, .step = 2, .action = MicroCode::XDATA, .src = GP_C, .target = GP_D, .opflags = OP_DONE },

  { .instruction = 0xFF, .step = 2, .action = MicroCode::OTHER, .src = GP_A, .target = GP_A, .opflags = OP_HALT | OP_DONE },

  { .instruction = 0x00, .step = 0 },
};

byte mem[] = { 0x01, 0x42, 0xFF };
MemImage image = {
  .address = 0x00, .size = 0x03, .contents = mem
};

class DummyComponent : public OwnedComponent {
public:
  explicit DummyComponent(System *s) : OwnedComponent(s) { }
};

RealSystem::RealSystem() : components(), clock(this, 0.001) {
  components.push_back(new Register(this, GP_A));
  components.push_back(new Register(this, GP_B));
  components.push_back(new Register(this, GP_C));
  components.push_back(new Register(this, GP_D));
  components.push_back(new DummyComponent(this));
  components.push_back(new DummyComponent(this));
  components.push_back(new Controller(this, mc));
  components.push_back(new DummyComponent(this));
  components.push_back(new AddressRegister(this, PC, "PC"));
  components.push_back(new AddressRegister(this, SP, "SP"));
  components.push_back(new AddressRegister(this, Si, "Si"));
  components.push_back(new AddressRegister(this, Di, "Di"));
  components.push_back(new AddressRegister(this, TX, "TX"));
  components.push_back(new DummyComponent(this));
  components.push_back(new DummyComponent(this));
  components.push_back(new Memory(this, 0x0000, 0x8000, 0x8000, 0x8000, &image));
}

const Component * RealSystem::componentByID(int id) const {
  return (id == 0x07) ? components.at(0x0F) : components.at(id);
}

void RealSystem::run() {
  clock.start();
}

byte RealSystem::readBus() const {
  return bus;
}

void RealSystem::putOnBus(byte value) {
  bus = value;
}

byte RealSystem::addrBus() const {
  return addr_bus;
}

void RealSystem::putOnAddrBus(byte value) {
  addr_bus = value;
}

ControlLines & RealSystem::control() {
  return controlLines;
}

void RealSystem::setControl(ControlLines &c) {
  controlLines = c;
}

void RealSystem::xdata(int from, int to, int opflags) {
  controlLines._xdata = false;
  controlLines._xaddr = true;
  controlLines.get = from;
  controlLines.put = to;
  controlLines.op = opflags;
}

void RealSystem::xaddr(int from, int to, int opflags) {
  controlLines._xdata = true;
  controlLines._xaddr = false;
  controlLines.get = from;
  controlLines.put = to;
  controlLines.op = opflags;
}

SystemError RealSystem::reportError() {
  if (error == NoError) {
    return NoError;
  }
  std::cout << "EXCEPTION " << error << std::endl;
  clock.stop();
  return error;
}

SystemError RealSystem::reset() {
  if (error != NoError) {
    return error;
  }
  for (auto & component : components) {
    error = component -> reset();
    if (error != NoError) {
      return reportError();
    }
  }
  return NoError;
}

SystemError RealSystem::status() {
  printf("DATA BUS %02x ADDR BUS %02x   GET '%01X' PUT '%01x' OP '%01X' %c %c\n",
         bus, addr_bus, controlLines.get, controlLines.put, controlLines.op,
         (controlLines._xdata) ? '_' : 'D',
         (controlLines._xaddr) ? '_' : 'A');
  return NoError;
}

SystemError RealSystem::onRisingClockEdge() {
  if (error != NoError) {
    return error;
  }
  status();
  for (auto & component : components) {
    error = component -> status();
    if (error != NoError) {
      return reportError();
    }
  }
  for (auto & component : components) {
    error = component -> onRisingClockEdge();
    if (error != NoError) {
      return reportError();
    }
  }
  return NoError;
}

SystemError RealSystem::onHighClock() {
  if (error != NoError) {
    return error;
  }
  for (auto & component : components) {
    error = component -> onHighClock();
    if (error != NoError) {
      return reportError();
    }
  }
  return NoError;
}

SystemError RealSystem::onFallingClockEdge() {
  if (error != NoError) {
    return error;
  }
  for (auto & component : components) {
    error = component -> onFallingClockEdge();
    if (error != NoError) {
      return reportError();
    }
  }
  return NoError;
}

SystemError RealSystem::onLowClock() {
  if (error != NoError) {
    return error;
  }
  for (auto & component : components) {
    error = component -> onLowClock();
    if (error != NoError) {
      return reportError();
    }
  }
  return NoError;
}


