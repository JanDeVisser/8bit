#include <chrono>
#include <iostream>
#include <gtest/gtest.h>
#include "memory.h"
#include "controller.h"
#include "harness.h"

#include "src/cpu/microcode.inc"

constexpr word RAM_START = 0x2000;
constexpr word RAM_SIZE = 0x2000;
constexpr word ROM_START = 0x8000;
constexpr word ROM_SIZE = 0x2000;
constexpr word START_VECTOR = ROM_START;


class ControllerTest : public ::testing::Test {
protected:
  Harness *system = nullptr;
  Memory *mem = nullptr;
  Controller *c = nullptr;
  Register *gp_a = new Register(0x0);
  Register *gp_b = new Register(0x1);
  Register *gp_c = new Register(0x2);
  Register *gp_d = new Register(0x3);
  AddressRegister *pc = new AddressRegister(PC, "PC");
  AddressRegister *tx = new AddressRegister(TX, "TX");
  AddressRegister *sp = new AddressRegister(SP, "SP");
  AddressRegister *si = new AddressRegister(Si, "Si");
  AddressRegister *di = new AddressRegister(Di, "Di");

  void SetUp() override {
    mem = new Memory(RAM_START, RAM_SIZE, ROM_START, ROM_SIZE, nullptr);

    c = new Controller(mc);
    system = new Harness();
    system -> insert(mem);
    system -> insert(c);
    system -> insert(gp_a);
    system -> insert(gp_b);
    system -> insert(gp_c);
    system -> insert(gp_d);
    system -> insert(pc);
    system -> insert(tx);
    system -> insert(sp);
    system -> insert(si);
    system -> insert(di);
//    system -> printStatus = true;
  }

  void TearDown() override {
    delete system;
  }

};

byte mov_a_direct[] = {
  /* 8000 */ MOV_A_CONST, 0x42,
  /* 8002 */ HLT
};

TEST_F(ControllerTest, testMovADirect) {
  mem -> initialize(ROM_START, 3, mov_a_direct);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_A_CONST);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov a, #42 takes 4 cycles. hlt takes 3.
  ASSERT_EQ(system -> run(), 7);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
}

TEST_F(ControllerTest, testMovADirectUsingRun) {
  mem -> initialize(ROM_START, 3, mov_a_direct);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_A_CONST);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  ASSERT_EQ(system -> run(), 7);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
}

byte mov_a_absolute[] = {
  /* 8000 */ MOV_A_ADDR, 0x04, 0x80,
  /* 8003 */ HLT,
  /* 8004 */ 0x42
};

TEST_F(ControllerTest, testMovAAbsolute) {
  mem -> initialize(ROM_START, 5, mov_a_absolute);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_A_ADDR);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov a, (8004) takes 8 cycles. hlt takes 3.
  system -> cycles(11);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
}

byte mov_x_a[] = {
  MOV_A_CONST, 0x42,
  MOV_B_A,
  MOV_C_A,
  MOV_D_A,
  HLT,
};

TEST_F(ControllerTest, testMovAToOtherGPRs) {
  mem -> initialize(ROM_START, 6, mov_x_a);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_A_CONST);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov a, #42  4 cycles
  // mov x, a    3 cycles x3
  // hlt         3 cycles
  // Total       16 cycles
  ASSERT_EQ(system -> run(), 16);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
  ASSERT_EQ(gp_b -> getValue(), 0x42);
  ASSERT_EQ(gp_c -> getValue(), 0x42);
  ASSERT_EQ(gp_d -> getValue(), 0x42);
}

byte mov_x_b[] = {
  MOV_B_CONST, 0x42,
  MOV_A_B,
  MOV_C_B,
  MOV_D_B,
  HLT,
};

TEST_F(ControllerTest, testMovBToOtherGPRs) {
  mem -> initialize(ROM_START, 6, mov_x_b);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_B_CONST);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  ASSERT_EQ(system -> run(), 16);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(gp_b -> getValue(), 0x42);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
  ASSERT_EQ(gp_c -> getValue(), 0x42);
  ASSERT_EQ(gp_d -> getValue(), 0x42);
}

byte mov_x_c[] = {
  MOV_C_CONST, 0x42,
  MOV_A_C,
  MOV_B_C,
  MOV_D_C,
  HLT,
};

TEST_F(ControllerTest, testMovCToOtherGPRs) {
  mem -> initialize(ROM_START, 6, mov_x_c);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_C_CONST);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  ASSERT_EQ(system -> run(), 16);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(gp_c -> getValue(), 0x42);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
  ASSERT_EQ(gp_b -> getValue(), 0x42);
  ASSERT_EQ(gp_d -> getValue(), 0x42);
}

byte mov_x_d[] = {
  MOV_D_CONST, 0x42,
  MOV_A_D,
  MOV_B_D,
  MOV_C_D,
  HLT,
};

TEST_F(ControllerTest, testMovDToOtherGPRs) {
  mem -> initialize(ROM_START, 6, mov_x_d);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_D_CONST);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  ASSERT_EQ(system -> run(), 16);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(gp_d -> getValue(), 0x42);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
  ASSERT_EQ(gp_b -> getValue(), 0x42);
  ASSERT_EQ(gp_c -> getValue(), 0x42);
}

byte mov_x_absolute[] = {
  /* 8000 */ MOV_A_ADDR, 0x0D, 0x80,
  /* 8003 */ MOV_B_ADDR, 0x0D, 0x80,
  /* 8006 */ MOV_C_ADDR, 0x0D, 0x80,
  /* 8009 */ MOV_D_ADDR, 0x0D, 0x80,
  /* 800C */ HLT,
  /* 800D */ 0x42
};

TEST_F(ControllerTest, testMovXAbsolute) {
  mem -> initialize(ROM_START, 14, mov_x_absolute);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_A_ADDR);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov x, (800D) takes 8 cycles x4
  // hlt takes 3.
  ASSERT_EQ(system -> run(),35);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
  ASSERT_EQ(gp_b -> getValue(), 0x42);
  ASSERT_EQ(gp_c -> getValue(), 0x42);
  ASSERT_EQ(gp_d -> getValue(), 0x42);
}

byte mov_addr_regs_direct[] = {
  /* 8000 */ MOV_SI_CONST, 0x42, 0x37,
  /* 8003 */ MOV_DI_CONST, 0x42, 0x37,
  /* 8006 */ MOV_SP_CONST, 0x42, 0x37,
  /* 8009 */ HLT,
};

TEST_F(ControllerTest, testMovAddrRegsDirect) {
  mem -> initialize(ROM_START, 10, mov_addr_regs_direct);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_SI_CONST);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov si, #3742 takes 6 cycles x3.
  // hlt takes 3.
  ASSERT_EQ(system -> run(), 21);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(si -> getValue(), 0x3742);
  ASSERT_EQ(di -> getValue(), 0x3742);
  ASSERT_EQ(sp -> getValue(), 0x3742);
}

byte mov_addr_regs_absolute[] = {
  /* 8000 */ MOV_SI_ADDR, 0x0A, 0x80,
  /* 8003 */ MOV_DI_ADDR, 0x0A, 0x80,
  /* 8006 */ MOV_SP_ADDR, 0x0A, 0x80,
  /* 8009 */ HLT,
  /* 800A */ 0x42, 0x37
};

TEST_F(ControllerTest, testMovAddrRegsAbsolute) {
  mem -> initialize(ROM_START, 12, mov_addr_regs_absolute);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_SI_ADDR);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov si, (8004) takes 10 cycles x3
  // hlt takes 3.
  ASSERT_EQ(system -> run(), 33);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(si -> getValue(), 0x3742);
  ASSERT_EQ(di -> getValue(), 0x3742);
  ASSERT_EQ(sp -> getValue(), 0x3742);
}

byte mov_addr_regs_from_other_regs[] = {
  /* 8000 */ MOV_C_CONST, 0x42,
  /* 8002 */ MOV_D_CONST, 0x37,
  /* 8004 */ MOV_SI_CD,
  /* 8005 */ MOV_DI_CD,
  /* 8006 */ MOV_SP_SI,
  /* 8007 */ HLT,
};

TEST_F(ControllerTest, testMovAddrRegsFromOtherRegs) {
  mem -> initialize(ROM_START, 8, mov_addr_regs_from_other_regs);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_C_CONST);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov c, #xx     4 cycles x2
  // mov xx, cd     4 cycles x2
  // mov sp, si     3 cycles
  // hlt            3 cycles
  // total          22
  ASSERT_EQ(system -> run(), 22);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(si -> getValue(), 0x3742);
  ASSERT_EQ(di -> getValue(), 0x3742);
  ASSERT_EQ(sp -> getValue(), 0x3742);
}

byte mov_gp_regs_from_si[] = {
  /* 8000 */ MOV_SI_CONST, 0x08, 0x80,
  /* 8003 */ MOV_A_SI,
  /* 8004 */ MOV_B_SI,
  /* 8005 */ MOV_C_SI,
  /* 8006 */ MOV_D_SI,
  /* 8007 */ HLT,
  /* 8008 */ 0x42,
  /* 8009 */ 0x43,
  /* 800A */ 0x44,
  /* 800B */ 0x45
};

TEST_F(ControllerTest, testMovGPRegsFromSI) {
  mem -> initialize(ROM_START, 12, mov_gp_regs_from_si);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_SI_CONST);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov si, #xxxx  6 cycles
  // mov xx, (si)   4 cycles x4
  // hlt            3 cycles
  // total          17
  ASSERT_EQ(system -> run(), 25);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(si -> getValue(), 0x800C);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
  ASSERT_EQ(gp_b -> getValue(), 0x43);
  ASSERT_EQ(gp_c -> getValue(), 0x44);
  ASSERT_EQ(gp_d -> getValue(), 0x45);
}

byte mov_gp_regs_from_di[] = {
  /* 8000 */ MOV_DI_CONST, 0x08, 0x80,
  /* 8003 */ MOV_A_DI,
  /* 8004 */ MOV_B_DI,
  /* 8005 */ MOV_C_DI,
  /* 8006 */ MOV_D_DI,
  /* 8007 */ HLT,
  /* 8008 */ 0x42,
  /* 8009 */ 0x43,
  /* 800A */ 0x44,
  /* 800B */ 0x45
};

TEST_F(ControllerTest, testMovGPRegsFromDI) {
  mem -> initialize(ROM_START, 12, mov_gp_regs_from_di);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_DI_CONST);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov di, #xxxx  6 cycles
  // mov xx, (si)   4 cycles x4
  // hlt            3 cycles
  // total          17
  ASSERT_EQ(system -> run(), 25);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(di -> getValue(), 0x800C);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
  ASSERT_EQ(gp_b -> getValue(), 0x43);
  ASSERT_EQ(gp_c -> getValue(), 0x44);
  ASSERT_EQ(gp_d -> getValue(), 0x45);
}

byte mov_di_from_si[] = {
  /* 8000 */ MOV_SI_CONST, 0x0B, 0x80,
  /* 8003 */ MOV_DI_CONST, 0x00, 0x20,
  /* 8006 */ MOV_DI_SI,
  /* 8007 */ MOV_DI_SI,
  /* 8008 */ MOV_DI_SI,
  /* 8009 */ MOV_DI_SI,
  /* 800A */ HLT,
  /* 800B */ 0x42,
  /* 800C */ 0x43,
  /* 800D */ 0x44,
  /* 800E */ 0x45
};

TEST_F(ControllerTest, testMovDIFromSI) {
  mem -> initialize(ROM_START, 16, mov_di_from_si);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_SI_CONST);
  ASSERT_EQ((*mem)[0x800B], 0x42);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov si, #xxxx  6 cycles x2
  // mov (di), (si) 6 cycles x4
  // hlt            3 cycles
  // total          39
  ASSERT_EQ(system -> run(), 39);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(si -> getValue(), 0x800F);
  ASSERT_EQ(di -> getValue(), 0x2004);
  ASSERT_EQ((*mem)[0x2000], 0x42);
  ASSERT_EQ((*mem)[0x2001], 0x43);
  ASSERT_EQ((*mem)[0x2002], 0x44);
  ASSERT_EQ((*mem)[0x2003], 0x45);
}

const byte push_a[] = {
  /* 8000 */ MOV_SP_CONST, 0x00, 0x20,
  /* 8003 */ MOV_A_CONST, 0x42,
  /* 8005 */ PUSH_A,
  /* 8006 */ HLT
};

TEST_F(ControllerTest, testPushA) {
  mem -> initialize(ROM_START, 7, push_a);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_SP_CONST);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov sp, #xxxx  6 cycles
  // mov a, #xx     4 cycles
  // push a         4 cycles
  // hlt            3 cycles
  // total          17
  ASSERT_EQ(system -> run(), 17);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
  ASSERT_EQ(sp -> getValue(), 0x2001);
  ASSERT_EQ((*mem)[0x2000], 0x42);
}

const byte push_pop_a[] = {
  /* 8000 */ MOV_SP_CONST, 0x00, 0x20,
  /* 8003 */ MOV_A_CONST, 0x42,
  /* 8005 */ PUSH_A,
  /* 8006 */ MOV_A_CONST, 0x37,
  /* 8008 */ POP_A,
  /* 8009 */ HLT
};

TEST_F(ControllerTest, testPushPopA) {
  mem -> initialize(ROM_START, 10, push_pop_a);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_SP_CONST);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov sp, #xxxx  6 cycles
  // mov a, #xx     4 cycles
  // push a         4 cycles
  // mov a, #xx     4 cycles
  // pop a          4 cycles
  // hlt            3 cycles
  // total          25
  ASSERT_EQ(system -> run(), 25);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
  ASSERT_EQ(sp -> getValue(), 0x2000);
  ASSERT_EQ((*mem)[0x2000], 0x42);
}

const byte push_pop_gp_regs[] = {
  /* 8000 */ MOV_SP_CONST, 0x00, 0x20,
  /* 8003 */ MOV_A_CONST, 0x42,
  /* 8005 */ MOV_B_CONST, 0x43,
  /* 8007 */ MOV_C_CONST, 0x44,
  /* 8009 */ MOV_D_CONST, 0x45,
  /* 800B */ PUSH_A,
  /* 800C */ PUSH_B,
  /* 800D */ PUSH_C,
  /* 800E */ PUSH_D,
  /* 800F */ MOV_A_CONST, 0x37,
  /* 8011 */ MOV_B_CONST, 0x36,
  /* 8013 */ MOV_C_CONST, 0x35,
  /* 8015 */ MOV_D_CONST, 0x34,
  /* 8017 */ POP_D,
  /* 8018 */ POP_C,
  /* 8019 */ POP_B,
  /* 801A */ POP_A,
  /* 801B */ HLT
};

TEST_F(ControllerTest, testPushPopGPRegs) {
  mem -> initialize(ROM_START, 29, push_pop_gp_regs);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_SP_CONST);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov sp, #xxxx  6 cycles      6
  // mov x, #xx     4 cycles 4x  16
  // push a         4 cycles 4x  16
  // mov a, #xx     4 cycles 4x  16
  // pop a          4 cycles 4x  16
  // hlt            3 cycles      3
  // total                       73
  ASSERT_EQ(system -> run(), 73);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
  ASSERT_EQ(gp_b -> getValue(), 0x43);
  ASSERT_EQ(gp_c -> getValue(), 0x44);
  ASSERT_EQ(gp_d -> getValue(), 0x45);
  ASSERT_EQ(sp -> getValue(), 0x2000);
  ASSERT_EQ((*mem)[0x2000], 0x42);
  ASSERT_EQ((*mem)[0x2001], 0x43);
  ASSERT_EQ((*mem)[0x2002], 0x44);
  ASSERT_EQ((*mem)[0x2003], 0x45);
}

const byte push_pop_addr_regs[] = {
  /* 8000 */ MOV_SP_CONST, 0x00, 0x20,
  /* 8003 */ MOV_SI_CONST, 0x34, 0x12,
  /* 8006 */ MOV_DI_CONST, 0x78, 0x56,
  /* 8009 */ PUSH_SI,
  /* 800A */ PUSH_DI,
  /* 800B */ MOV_SI_CONST, 0x55, 0x44,
  /* 800E */ MOV_DI_CONST, 0x77, 0x66,
  /* 8012 */ POP_DI,
  /* 8013 */ POP_SI,
  /* 8015 */ HLT
};

TEST_F(ControllerTest, testPushPopAddrRegs) {
  mem -> initialize(ROM_START, 22, push_pop_addr_regs);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_SP_CONST);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov xx, #xxxx  6 cycles 3x  18
  // push xx        6 cycles 2x  12
  // mov xx, #xxxx  6 cycles 2x  12
  // pop xx         6 cycles 2x  12
  // hlt            3 cycles      3
  // total                       57
  auto cycles = system -> run();
  ASSERT_EQ(system -> error, NoError);
  ASSERT_EQ(cycles, 57);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(si -> getValue(), 0x1234);
  ASSERT_EQ(di -> getValue(), 0x5678);
  ASSERT_EQ(sp -> getValue(), 0x2000);
  ASSERT_EQ((*mem)[0x2000], 0x34);
  ASSERT_EQ((*mem)[0x2001], 0x12);
  ASSERT_EQ((*mem)[0x2002], 0x78);
  ASSERT_EQ((*mem)[0x2003], 0x56);
}

const byte jmp_basic[] = {
  /* 8000 */ JMP, 0x06, 0x80,
  /* 8003 */ MOV_A_CONST, 0x37,
  /* 8005 */ HLT,
  /* 8006 */ MOV_A_CONST, 0x42,
  /* 8008 */ HLT,
};

TEST_F(ControllerTest, testJmp) {
  mem -> initialize(ROM_START, 9, jmp_basic);
  ASSERT_EQ((*mem)[START_VECTOR], JMP);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // jmp            7 cycles
  // mov a, #xx     4 cycles
  // hlt            3 cycles
  // total         14
  auto cycles = system -> run();
  ASSERT_EQ(system -> error, NoError);
  ASSERT_EQ(cycles, 14);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
}

