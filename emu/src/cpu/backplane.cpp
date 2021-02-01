#include "addressregister.h"
#include "controller.h"
#include "memory.h"
#include "backplane.h"
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

  HLT             = 0xFF
};

static MicroCode mc[256] = {
  {
    .opcode = NOP, .instruction = "nop",
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_A, .target = GP_A, .opflags = SystemBus::OP_DONE },
    }
  },

  {
    .opcode = MOV_A_CONST, .instruction = "mov a,#xx",
    .steps = {
      { .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = SystemBus::OP_INC },
      { .action = MicroCode::XDATA, .src = MEM, .target = GP_A, .opflags = SystemBus::OP_DONE },
    }
  },

  {
    .opcode = MOV_A_ADDR, .instruction = "mov a,(xxxx)",
    .steps = {
      { .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = SystemBus::OP_INC },
      { .action = MicroCode::XDATA, .src = MEM, .target = TX, .opflags = SystemBus::OP_NONE },
      { .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = SystemBus::OP_INC },
      { .action = MicroCode::XDATA, .src = MEM, .target = TX, .opflags = SystemBus::OP_MSB },
      { .action = MicroCode::XADDR, .src = TX, .target = MEMADDR, .opflags = SystemBus::OP_DONE },
      { .action = MicroCode::XDATA, .src = MEM, .target = GP_A, .opflags = SystemBus::OP_DONE },
    }
  },

  {
    .opcode = MOV_A_B, .instruction = "mov a,b",
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_B, .target = GP_A, .opflags = SystemBus::OP_DONE },
    }
  },

  {
    .opcode = MOV_A_C, .instruction = "mov a,c",
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_C, .target = GP_A, .opflags = SystemBus::OP_DONE },
    }
  },

  {
    .opcode = MOV_A_D, .instruction = "mov a,d",
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_D, .target = GP_A, .opflags = SystemBus::OP_DONE },
    }
  },


  {
    .opcode = MOV_B_CONST, .instruction = "mov b, #xx",
    .steps = {
      { .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = SystemBus::OP_INC },
      { .action = MicroCode::XDATA, .src = MEM, .target = GP_B, .opflags = SystemBus::OP_DONE },
    }
  },

  {
    .opcode = MOV_B_ADDR, .instruction = "mov b,(xxxx)",
    .steps = {
      { .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = SystemBus::OP_INC },
      { .action = MicroCode::XDATA, .src = MEM, .target = TX, .opflags = SystemBus::OP_NONE },
      { .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = SystemBus::OP_INC },
      { .action = MicroCode::XDATA, .src = MEM, .target = TX, .opflags = SystemBus::OP_MSB },
      { .action = MicroCode::XADDR, .src = TX, .target = MEMADDR, .opflags = SystemBus::OP_DONE },
      { .action = MicroCode::XDATA, .src = MEM, .target = GP_B, .opflags = SystemBus::OP_DONE },
    }
  },

  {
    .opcode = MOV_B_A, .instruction = "mov b,a",
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_A, .target = GP_B, .opflags = SystemBus::OP_DONE },
    }
  },

  {
    .opcode = MOV_B_C, .instruction = "mov b,c",
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_C, .target = GP_B, .opflags = SystemBus::OP_DONE },
    }
  },

  {
    .opcode = MOV_B_D, .instruction = "mov b,d",
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_D, .target = GP_B, .opflags = SystemBus::OP_DONE },
    }
  },

  {
    .opcode = MOV_C_CONST, .instruction = "mov c,#xx",
    .steps = {
      { .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = SystemBus::OP_INC },
      { .action = MicroCode::XDATA, .src = MEM, .target = GP_C, .opflags = SystemBus::OP_DONE },
    }
  },

  {
    .opcode = MOV_C_ADDR, .instruction = "mov c,(xxxx)",
    .steps = {
      { .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = SystemBus::OP_INC },
      { .action = MicroCode::XDATA, .src = MEM, .target = TX, .opflags = SystemBus::OP_NONE },
      { .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = SystemBus::OP_INC },
      { .action = MicroCode::XDATA, .src = MEM, .target = TX, .opflags = SystemBus::OP_MSB },
      { .action = MicroCode::XADDR, .src = TX, .target = MEMADDR, .opflags = SystemBus::OP_DONE },
      { .action = MicroCode::XDATA, .src = MEM, .target = GP_C, .opflags = SystemBus::OP_DONE },
    }
  },

  {
    .opcode = MOV_C_A, .instruction = "mov c,a",
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_B, .target = GP_C, .opflags = SystemBus::OP_DONE },
    }
  },

  {
    .opcode = MOV_C_B, .instruction = "mov c,b",
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_C, .target = GP_C, .opflags = SystemBus::OP_DONE },
    }
  },

  {
    .opcode = MOV_C_D, .instruction = "mov c,d",
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_D, .target = GP_C, .opflags = SystemBus::OP_DONE },
    }
  },


  {
    .opcode = MOV_D_CONST, .instruction = "mov d,#xx",
    .steps = {
      { .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = SystemBus::OP_INC },
      { .action = MicroCode::XDATA, .src = MEM, .target = GP_D, .opflags = SystemBus::OP_DONE },
    }
  },

  {
    .opcode = MOV_D_ADDR, .instruction = "mov d,(xxxx)",
    .steps = {
      { .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = SystemBus::OP_INC },
      { .action = MicroCode::XDATA, .src = MEM, .target = TX, .opflags = SystemBus::OP_NONE },
      { .action = MicroCode::XADDR, .src = PC, .target = MEMADDR, .opflags = SystemBus::OP_INC },
      { .action = MicroCode::XDATA, .src = MEM, .target = TX, .opflags = SystemBus::OP_MSB },
      { .action = MicroCode::XADDR, .src = TX, .target = MEMADDR, .opflags = SystemBus::OP_DONE },
      { .action = MicroCode::XDATA, .src = MEM, .target = GP_D, .opflags = SystemBus::OP_DONE },
    }
  },

  {
    .opcode = MOV_D_A, .instruction = "mov d,a",
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_A, .target = GP_D, .opflags = SystemBus::OP_DONE },
    }
  },

  {
    .opcode = MOV_D_B, .instruction = "mov d,b",
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_B, .target = GP_D, .opflags = SystemBus::OP_DONE },
    }
  },

  {
    .opcode = MOV_D_C, .instruction = "mov d,c",
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_C, .target = GP_D, .opflags = SystemBus::OP_DONE },
    }
  },

  { /* 21 */ }, { /* 22 */ }, { /* 23 */ }, { /* 24 */ }, { /* 25 */ }, { /* 26 */ }, { /* 27 */ }, { /* 28 */ },
  { /* 29 */ }, { /* 30 */ }, { /* 31 */ }, { /* 32 */ }, { /* 33 */ }, { /* 34 */ }, { /* 35 */ }, { /* 36 */ },
  { /* 37 */ }, { /* 38 */ }, { /* 39 */ }, { /* 40 */ }, { /* 41 */ }, { /* 42 */ }, { /* 43 */ }, { /* 44 */ },
  { /* 45 */ }, { /* 46 */ }, { /* 47 */ }, { /* 48 */ }, { /* 49 */ }, { /* 50 */ }, { /* 51 */ }, { /* 52 */ },
  { /* 53 */ }, { /* 54 */ }, { /* 55 */ }, { /* 56 */ }, { /* 57 */ }, { /* 58 */ }, { /* 59 */ }, { /* 60 */ },
  { /* 61 */ }, { /* 62 */ }, { /* 63 */ }, { /* 64 */ }, { /* 65 */ }, { /* 66 */ }, { /* 67 */ }, { /* 68 */ },
  { /* 69 */ }, { /* 70 */ }, { /* 71 */ }, { /* 72 */ }, { /* 73 */ }, { /* 74 */ }, { /* 75 */ }, { /* 76 */ },
  { /* 77 */ }, { /* 78 */ }, { /* 79 */ }, { /* 80 */ }, { /* 81 */ }, { /* 82 */ }, { /* 83 */ }, { /* 84 */ },
  { /* 85 */ }, { /* 86 */ }, { /* 87 */ }, { /* 88 */ }, { /* 89 */ }, { /* 90 */ }, { /* 91 */ }, { /* 92 */ },
  { /* 93 */ }, { /* 94 */ }, { /* 95 */ }, { /* 96 */ }, { /* 97 */ }, { /* 98 */ }, { /* 99 */ }, { /* 100 */ },
  { /* 101 */ }, { /* 102 */ }, { /* 103 */ }, { /* 104 */ }, { /* 105 */ }, { /* 106 */ }, { /* 107 */ }, { /* 108 */ },
  { /* 109 */ }, { /* 110 */ }, { /* 111 */ }, { /* 112 */ }, { /* 113 */ }, { /* 114 */ }, { /* 115 */ }, { /* 116 */ },
  { /* 117 */ }, { /* 118 */ }, { /* 119 */ }, { /* 120 */ }, { /* 121 */ }, { /* 122 */ }, { /* 123 */ }, { /* 124 */ },
  { /* 125 */ }, { /* 126 */ }, { /* 127 */ }, { /* 128 */ }, { /* 129 */ }, { /* 130 */ }, { /* 131 */ }, { /* 132 */ },
  { /* 133 */ }, { /* 134 */ }, { /* 135 */ }, { /* 136 */ }, { /* 137 */ }, { /* 138 */ }, { /* 139 */ }, { /* 140 */ },
  { /* 141 */ }, { /* 142 */ }, { /* 143 */ }, { /* 144 */ }, { /* 145 */ }, { /* 146 */ }, { /* 147 */ }, { /* 148 */ },
  { /* 149 */ }, { /* 150 */ }, { /* 151 */ }, { /* 152 */ }, { /* 153 */ }, { /* 154 */ }, { /* 155 */ }, { /* 156 */ },
  { /* 157 */ }, { /* 158 */ }, { /* 159 */ }, { /* 160 */ }, { /* 161 */ }, { /* 162 */ }, { /* 163 */ }, { /* 164 */ },
  { /* 165 */ }, { /* 166 */ }, { /* 167 */ }, { /* 168 */ }, { /* 169 */ }, { /* 170 */ }, { /* 171 */ }, { /* 172 */ },
  { /* 173 */ }, { /* 174 */ }, { /* 175 */ }, { /* 176 */ }, { /* 177 */ }, { /* 178 */ }, { /* 179 */ }, { /* 180 */ },
  { /* 181 */ }, { /* 182 */ }, { /* 183 */ }, { /* 184 */ }, { /* 185 */ }, { /* 186 */ }, { /* 187 */ }, { /* 188 */ },
  { /* 189 */ }, { /* 190 */ }, { /* 191 */ }, { /* 192 */ }, { /* 193 */ }, { /* 194 */ }, { /* 195 */ }, { /* 196 */ },
  { /* 197 */ }, { /* 198 */ }, { /* 199 */ }, { /* 200 */ }, { /* 201 */ }, { /* 202 */ }, { /* 203 */ }, { /* 204 */ },
  { /* 205 */ }, { /* 206 */ }, { /* 207 */ }, { /* 208 */ }, { /* 209 */ }, { /* 210 */ }, { /* 211 */ }, { /* 212 */ },
  { /* 213 */ }, { /* 214 */ }, { /* 215 */ }, { /* 216 */ }, { /* 217 */ }, { /* 218 */ }, { /* 219 */ }, { /* 220 */ },
  { /* 221 */ }, { /* 222 */ }, { /* 223 */ }, { /* 224 */ }, { /* 225 */ }, { /* 226 */ }, { /* 227 */ }, { /* 228 */ },
  { /* 229 */ }, { /* 230 */ }, { /* 231 */ }, { /* 232 */ }, { /* 233 */ }, { /* 234 */ }, { /* 235 */ }, { /* 236 */ },
  { /* 237 */ }, { /* 238 */ }, { /* 239 */ }, { /* 240 */ }, { /* 241 */ }, { /* 242 */ }, { /* 243 */ }, { /* 244 */ },
  { /* 245 */ }, { /* 246 */ }, { /* 247 */ }, { /* 248 */ }, { /* 249 */ }, { /* 250 */ }, { /* 251 */ }, { /* 252 */ },
  { /* 253 */ }, { /* 254 */ },

  {
    .opcode = HLT, .instruction = "hlt",
    .steps = {
      { .action = MicroCode::OTHER, .src = GP_A, .target = GP_A, .opflags = SystemBus::OP_HALT | SystemBus::OP_DONE },
    }
  }

};

byte mem[] = { 0x01, 0x42, 0xFF };
MemImage image = {
  .address = 0x00, .size = 0x03, .contents = mem
};

class DummyComponent : public ConnectedComponent {
public:
  DummyComponent() = default;
};

BackPlane::BackPlane() : components(), clock(this, 0.001) {
  systemBus = new SystemBus();
  add(new Register(GP_A));
  add(new Register(GP_B));
  add(new Register(GP_C));
  add(new Register(GP_D));
  add(new DummyComponent());
  add(new DummyComponent());
  add(new Controller(mc));
  add(new DummyComponent());
  add(new AddressRegister(PC, "PC"));
  add(new AddressRegister(SP, "SP"));
  add(new AddressRegister(Si, "Si"));
  add(new AddressRegister(Di, "Di"));
  add(new AddressRegister(TX, "TX"));
  add(new DummyComponent());
  add(new DummyComponent());
  add(new Memory(0x0000, 0x8000, 0x8000, 0x8000, &image));
}

void BackPlane::add(ConnectedComponent *comp) {
  comp->bus(this -> systemBus);
  components.push_back(comp);
}

ConnectedComponent * BackPlane::componentByID(int id) const {
  return (id == 0x07) ? components.at(0x0F) : components.at(id);
}

void BackPlane::run() {
  reset();
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
  for (auto & component : components) {
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


