#ifndef EMU_MICROCODE_INC
#define EMU_MICROCODE_INC

#include "alu.h"
#include "registers.h"

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
  MOV_SP_SI       = 0x17,

  MOV_SI_CONST    = 0x18,
  MOV_SI_ADDR     = 0x19,
  MOV_SI_CD       = 0x1A,

  MOV_DI_CONST    = 0x1B,
  MOV_DI_ADDR     = 0x1C,
  MOV_DI_CD       = 0x1D,

  MOV_A_SI        = 0x1E,
  MOV_B_SI        = 0x1F,
  MOV_C_SI        = 0x20,
  MOV_D_SI        = 0x21,

  MOV_A_DI        = 0x22,
  MOV_B_DI        = 0x23,
  MOV_C_DI        = 0x24,
  MOV_D_DI        = 0x25,

  MOV_DI_SI       = 0x26,

  JMP             = 0x27,
  JNZ             = 0x28,
  JC              = 0x29,
  JV              = 0x2A,
  CALL            = 0x2B,
  RET             = 0x2C,

  PUSH_A          = 0x2D,
  PUSH_B          = 0x2E,
  PUSH_C          = 0x2F,
  PUSH_D          = 0x30,
  PUSH_SI         = 0x31,
  PUSH_DI         = 0x32,
  POP_A           = 0x33,
  POP_B           = 0x34,
  POP_C           = 0x35,
  POP_D           = 0x36,
  POP_SI          = 0x37,
  POP_DI          = 0x38,

  MOV_ADDR_A      = 0x39,
  MOV_DI_A        = 0x3A,
  MOV_ADDR_B      = 0x3B,
  MOV_DI_B        = 0x3C,
  MOV_ADDR_C      = 0x3D,
  MOV_DI_C        = 0x3E,
  MOV_ADDR_D      = 0x3F,
  MOV_DI_D        = 0x40,

  MOV_ADDR_SI     = 0x41,
  MOV_ADDR_DI     = 0x42,
  MOV_ADDR_CD     = 0x43,
  MOV_SI_CD_      = 0x44,
  MOV_DI_CD_      = 0x45,

  ADD_A_B         = 0x46,
  ADC_A_B         = 0x47,
  SUB_A_B         = 0x48,
  SBC_A_B         = 0x49,
  AND_A_B         = 0x4A,
  OR_A_B          = 0x4B,
  NOT_A           = 0x4C,
  XOR_A_B         = 0x4D,
  SHR_A           = 0x4E,
  SHL_A           = 0x4F,

  HLT             = 0xFF
};

#define PUSH_REG(reg, opflag) \
  { .action = MicroCode::XADDR, .src = SP, .target = MEMADDR, .opflags = SystemBus::Inc }, \
  { .action = MicroCode::XDATA, .src = reg, .target = MEM, .opflags = opflag },

#define POP_REG(reg, opflag) \
  { .action = MicroCode::XADDR, .src = SP, .target = MEMADDR, .opflags = SystemBus::Dec }, \
  { .action = MicroCode::XDATA, .src = MEM, .target = reg, .opflags = opflag },

#define PUSH_ADDR(reg, opflag) \
  { .action = MicroCode::XADDR, .src = SP, .target = MEMADDR, .opflags = SystemBus::Inc }, \
  { .action = MicroCode::XDATA, .src = reg, .target = MEM, .opflags = SystemBus::None }, \
  { .action = MicroCode::XADDR, .src = SP, .target = MEMADDR, .opflags = SystemBus::Inc }, \
  { .action = MicroCode::XDATA, .src = reg, .target = MEM, .opflags = SystemBus::MSB | opflag }, \

#define POP_ADDR(reg, opflag) \
  { .action = MicroCode::XADDR, .src = SP, .target = MEMADDR, .opflags = SystemBus::Dec }, \
  { .action = MicroCode::XDATA, .src = MEM, .target = reg, .opflags = SystemBus::MSB }, \
  { .action = MicroCode::XADDR, .src = SP, .target = MEMADDR, .opflags = SystemBus::Dec }, \
  { .action = MicroCode::XDATA, .src = MEM, .target = reg, .opflags = opflag },


static MicroCode mc[256] = {
  {
    .opcode = NOP, .instruction = "nop", .addressingMode = static_cast<AddressingMode>(Immediate | Done)
  },

  {
    .opcode = MOV_A_CONST, .instruction = "mov a,#%02x", 
    .addressingMode = static_cast<AddressingMode>(DirectByte | Done), .target = GP_A
  },

  {
    .opcode = MOV_A_ADDR, .instruction = "mov a,(%04x)", 
    .addressingMode = static_cast<AddressingMode>(AbsoluteByte | Done), .target = GP_A
  },

  {
    .opcode = MOV_A_B, .instruction = "mov a,b", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_B, .target = GP_A, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_A_C, .instruction = "mov a,c", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_C, .target = GP_A, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_A_D, .instruction = "mov a,d", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_D, .target = GP_A, .opflags = SystemBus::Done },
    }
  },


  {
    .opcode = MOV_B_CONST, .instruction = "mov b, #%02x", 
    .addressingMode = static_cast<AddressingMode>(DirectByte | Done), .target = GP_B
  },

  {
    .opcode = MOV_B_ADDR, .instruction = "mov b,(%04x)",
    .addressingMode = static_cast<AddressingMode>(AbsoluteByte | Done), .target = GP_B
  },

  {
    .opcode = MOV_B_A, .instruction = "mov b,a", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_A, .target = GP_B, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_B_C, .instruction = "mov b,c", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_C, .target = GP_B, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_B_D, .instruction = "mov b,d", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_D, .target = GP_B, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_C_CONST, .instruction = "mov c,#%02x",
    .addressingMode = static_cast<AddressingMode>(DirectByte | Done), .target = GP_C
  },

  {
    .opcode = MOV_C_ADDR, .instruction = "mov c,(%04x)",
    .addressingMode = static_cast<AddressingMode>(AbsoluteByte | Done), .target = GP_C
  },

  {
    .opcode = MOV_C_A, .instruction = "mov c,a", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_B, .target = GP_C, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_C_B, .instruction = "mov c,b", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_B, .target = GP_C, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_C_D, .instruction = "mov c,d", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_D, .target = GP_C, .opflags = SystemBus::Done },
    }
  },


  {
    .opcode = MOV_D_CONST, .instruction = "mov d,#%02x",
    .addressingMode = static_cast<AddressingMode>(DirectByte | Done), .target = GP_D
  },

  {
    .opcode = MOV_D_ADDR, .instruction = "mov d,(%04x)",
    .addressingMode = static_cast<AddressingMode>(AbsoluteByte | Done), .target = GP_D
  },

  {
    .opcode = MOV_D_A, .instruction = "mov d,a", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_A, .target = GP_D, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_D_B, .instruction = "mov d,b", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_B, .target = GP_D, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_D_C, .instruction = "mov d,c", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_C, .target = GP_D, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_SP_CONST, .instruction = "mov sp,#%04x",
    .addressingMode = static_cast<AddressingMode>(DirectWord | Done), .target = SP
  },

  {
    .opcode = MOV_SP_ADDR, .instruction = "mov sp,(%04x)", .
    addressingMode = static_cast<AddressingMode>(AbsoluteWord | Done), .target = SP
  },

  {
    .opcode = MOV_SP_SI, .instruction = "mov sp,si", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XADDR, .src = Si, .target = SP, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_SI_CONST, .instruction = "mov si,#%04x",
    .addressingMode = static_cast<AddressingMode>(AddressingMode::DirectWord | AddressingMode::Done), .target = Si
  },

  {
    .opcode = MOV_SI_ADDR, .instruction = "mov si,(%04x)",
    .addressingMode = static_cast<AddressingMode>(AbsoluteWord | Done), .target = Si
  },

  {
    .opcode = MOV_SI_CD, .instruction = "mov si,cd", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_C, .target = Si, .opflags = SystemBus::None },
      { .action = MicroCode::XDATA, .src = GP_D, .target = Si, .opflags = SystemBus::MSB | SystemBus::Done },
    }
  },

  {
    .opcode = MOV_DI_CONST, .instruction = "mov di,#%04x",
    .addressingMode = static_cast<AddressingMode>(DirectWord | Done), .target = Di
  },

  {
    .opcode = MOV_DI_ADDR, .instruction = "mov di,(%04x)",
    .addressingMode = static_cast<AddressingMode>(AbsoluteWord | Done), .target = Di
  },

  {
    .opcode = MOV_DI_CD, .instruction = "mov di,cd", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_C, .target = Di, .opflags = SystemBus::None },
      { .action = MicroCode::XDATA, .src = GP_D, .target = Di, .opflags = SystemBus::MSB | SystemBus::Done },
    }
  },

  {
    .opcode = MOV_A_SI, .instruction = "mov a,(si)", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XADDR, .src = Si, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = MEM, .target = GP_A, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_B_SI, .instruction = "mov b,(si)", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XADDR, .src = Si, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = MEM, .target = GP_B, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_C_SI, .instruction = "mov c,(si)", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XADDR, .src = Si, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = MEM, .target = GP_C, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_D_SI, .instruction = "mov d,(si)", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XADDR, .src = Si, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = MEM, .target = GP_D, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_A_DI, .instruction = "mov a,(di)", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XADDR, .src = Di, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = MEM, .target = GP_A, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_B_DI, .instruction = "mov b,(di)", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XADDR, .src = Di, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = MEM, .target = GP_B, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_C_DI, .instruction = "mov c,(di)", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XADDR, .src = Di, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = MEM, .target = GP_C, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_D_DI, .instruction = "mov d,(di)", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XADDR, .src = Di, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = MEM, .target = GP_D, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_DI_SI, .instruction = "mov (di),(si)", .addressingMode = Immediate,
    .steps = {
      { .action = MicroCode::XADDR, .src = Si, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = MEM, .target = TX, .opflags = SystemBus::None },
      { .action = MicroCode::XADDR, .src = Di, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = TX, .target = MEM, .opflags = SystemBus::Done },
    }
  },

//  JMP             = 0x27,
  {
    .opcode = JMP, .instruction = "jmp #%04x",
    .addressingMode = static_cast<AddressingMode>(DirectWord | Done), .target = PC
  },

//  JNZ             = 0x28,
  {
    .opcode = JNZ, .instruction = "jnz #%04x",
    .addressingMode = static_cast<AddressingMode>(DirectWord | Done), .target = PC,
    .condition = SystemBus::Z, .condition_op = MicroCode::Nand
  },

//  JC              = 0x29,
  {
    .opcode = JC, .instruction = "jc #%04x",
    .addressingMode = static_cast<AddressingMode>(DirectWord | Done), .target = PC,
    .condition = SystemBus::C, .condition_op = MicroCode::And
  },

//  JV              = 0x2A,
  {
    .opcode = JV, .instruction = "jv #%04x",
    .addressingMode = static_cast<AddressingMode>(DirectWord | Done), .target = PC,
    .condition = SystemBus::V, .condition_op = MicroCode::And
  },

  {
    .opcode = CALL, .instruction = "call #%04x", .addressingMode = DirectWord, .target = TX, // 0x2b
    .steps = {
      // TX Contains the address to jump to. PC has the address to return to (one past the address)

      // Push the return address:
      PUSH_ADDR(PC, SystemBus::None)

      // Load PC with the subroutine address:
      { .action = MicroCode::XADDR, .src = TX, .target = PC, .opflags = SystemBus::Done },

    }
  },

  {
    .opcode = RET, .instruction = "ret", .addressingMode = Immediate, //0x02c
    .steps = {
      POP_ADDR(PC, SystemBus::Done)
    }
  },

  {
    .opcode = PUSH_A, .instruction = "push a", .addressingMode = Immediate, //0x2d
    .steps = {
      PUSH_REG(GP_A, SystemBus::Done)
    }
  },

  {
    .opcode = PUSH_B, .instruction = "push b", .addressingMode = Immediate, //0x2e
    .steps = {
      PUSH_REG(GP_B, SystemBus::Done)
    }
  },

  {
    .opcode = PUSH_C, .instruction = "push c", .addressingMode = Immediate, //0x2f
    .steps = {
      PUSH_REG(GP_C, SystemBus::Done)
    }
  },

  {
    .opcode = PUSH_D, .instruction = "push d", .addressingMode = Immediate, //0x30
    .steps = {
      PUSH_REG(GP_D, SystemBus::Done)
    }
  },

  {
    .opcode = PUSH_SI, .instruction = "push si", .addressingMode = Immediate, //0x31
    .steps = {
      PUSH_ADDR(Si, SystemBus::Done)
    }
  },

  {
    .opcode = PUSH_DI, .instruction = "push di", .addressingMode = Immediate, //0x32
    .steps = {
      PUSH_ADDR(Di, SystemBus::Done)
    }
  },

  {
    .opcode = POP_A, .instruction = "pop a", .addressingMode = Immediate, //0x33
    .steps = {
      POP_REG(GP_A, SystemBus::Done)
    }
  },

  {
    .opcode = POP_B, .instruction = "pop b", .addressingMode = Immediate, //0x34
    .steps = {
      POP_REG(GP_B, SystemBus::Done)
    }
  },

  {
    .opcode = POP_C, .instruction = "pop c", .addressingMode = Immediate, //0x35
    .steps = {
      POP_REG(GP_C, SystemBus::Done)
    }
  },

  {
    .opcode = POP_D, .instruction = "pop d", .addressingMode = Immediate, //0x36
    .steps = {
      POP_REG(GP_D, SystemBus::Done)
    }
  },

  {
    .opcode = POP_SI, .instruction = "pop si", .addressingMode = Immediate, //0x37
    .steps = {
      POP_ADDR(Si, SystemBus::Done)
    }
  },

  {
    .opcode = POP_DI, .instruction = "pop di", .addressingMode = Immediate, //0x38
    .steps = {
      POP_ADDR(Di, SystemBus::Done)
    }
  },

  {
    .opcode = MOV_ADDR_A, .instruction = "mov (%04x), a", .addressingMode = DirectWord, .target = MEMADDR, // 0x39
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_A, .target = MEM, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_DI_A, .instruction = "mov (di), a", .addressingMode = Immediate, // 0x3A
    .steps = {
      { .action = MicroCode::XADDR, .src = Di, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = GP_A, .target = MEM, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_ADDR_B, .instruction = "mov (%04x), b", .addressingMode = DirectWord, .target = MEMADDR, // 0x3B
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_B, .target = MEM, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_DI_B, .instruction = "mov (di), b", .addressingMode = Immediate, // 0x3C
    .steps = {
      { .action = MicroCode::XADDR, .src = Di, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = GP_B, .target = MEM, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_ADDR_C, .instruction = "mov (%04x), c",  .addressingMode = DirectWord, .target = MEMADDR, // 0x3D
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_C, .target = MEM, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_DI_C, .instruction = "mov (di), c", .addressingMode = Immediate, // 0x3E
    .steps = {
      { .action = MicroCode::XADDR, .src = Di, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = GP_C, .target = MEM, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_ADDR_D, .instruction = "mov (%04x), d",  .addressingMode = DirectWord, .target = MEMADDR, // 0x3F
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_D, .target = MEM, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_DI_D, .instruction = "mov (di), d", .addressingMode = Immediate, // 0x40
    .steps = {
      { .action = MicroCode::XADDR, .src = Di, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = GP_D, .target = MEM, .opflags = SystemBus::Done },
    }
  },

  {
    .opcode = MOV_ADDR_SI, .instruction = "mov (%04x), si",  .addressingMode = DirectWord, .target = TX, // 0x41
    .steps = {
      { .action = MicroCode::XADDR, .src = TX, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = Si, .target = MEM, .opflags = SystemBus::None },
      { .action = MicroCode::XADDR, .src = TX, .target = MEMADDR, .opflags = SystemBus::None },
      { .action = MicroCode::XDATA, .src = Si, .target = MEM, .opflags = SystemBus::MSB | SystemBus::Done },
    }
  },

  {
    .opcode = MOV_ADDR_DI, .instruction = "mov (%04x), di",  .addressingMode = DirectWord, .target = TX, // 0x42
    .steps = {
      { .action = MicroCode::XADDR, .src = TX, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = Di, .target = MEM, .opflags = SystemBus::None },
      { .action = MicroCode::XADDR, .src = TX, .target = MEMADDR, .opflags = SystemBus::None },
      { .action = MicroCode::XDATA, .src = Di, .target = MEM, .opflags = SystemBus::MSB | SystemBus::Done },
    }
  },

  {
    .opcode = MOV_ADDR_CD, .instruction = "mov (%04x), cd",  .addressingMode = DirectWord, .target = TX, // 0x43
    .steps = {
      { .action = MicroCode::XADDR, .src = TX, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = GP_C, .target = MEM, .opflags = SystemBus::None },
      { .action = MicroCode::XADDR, .src = TX, .target = MEMADDR, .opflags = SystemBus::None },
      { .action = MicroCode::XDATA, .src = GP_D, .target = MEM, .opflags = SystemBus::MSB | SystemBus::Done },
    }
  },

  {
    .opcode = MOV_SI_CD_, .instruction = "mov (si), cd",  .addressingMode = Immediate, // 0x44
    .steps = {
      { .action = MicroCode::XADDR, .src = Si, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = GP_C, .target = MEM, .opflags = SystemBus::None },
      { .action = MicroCode::XADDR, .src = Si, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = GP_D, .target = MEM, .opflags = SystemBus::MSB | SystemBus::Done },
    }
  },

  {
    .opcode = MOV_DI_CD_, .instruction = "mov (di), cd",  .addressingMode = Immediate, // 0x45
    .steps = {
      { .action = MicroCode::XADDR, .src = Di, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = GP_C, .target = MEM, .opflags = SystemBus::None },
      { .action = MicroCode::XADDR, .src = Di, .target = MEMADDR, .opflags = SystemBus::Inc },
      { .action = MicroCode::XDATA, .src = GP_D, .target = MEM, .opflags = SystemBus::MSB | SystemBus::Done },
    }
  },

//  ADD_A_B         = 0x46,
  {
    .opcode = ADD_A_B, .instruction = "add a, b",  .addressingMode = Immediate, // 0x46
    .steps = {
      { .action = MicroCode::XDATA, .src = GP_A, .target = LHS, .opflags = SystemBus::None },
      { .action = MicroCode::XDATA, .src = GP_B, .target = RHS, .opflags = ALU::ADD },
      { .action = MicroCode::XDATA, .src = LHS, .target = GP_A, .opflags = SystemBus::Done },
    }
  },

//  ADC_A_B         = 0x47,
//  SUB_A_B         = 0x48,
//  SBC_A_B         = 0x49,
//  AND_A_B         = 0x4A,
//  OR_A_B          = 0x4B,
//  NOT_A           = 0x4C,
//  XOR_A_B         = 0x4D,
//  SHR_A           = 0x4E,
//  SHL_A           = 0x4F,


  { /* 71 */ }, { /* 72 */ }, { /* 73 */ }, { /* 74 */ }, { /* 75 */ }, { /* 76 */ },
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
      { .action = MicroCode::OTHER, .src = GP_A, .target = GP_A, .opflags = SystemBus::Halt | SystemBus::Done },
    }
  }

};

#endif //EMU_MICROCODE_INC
