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
  AddressRegister *si = new AddressRegister(Si, "Si");

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
    system -> insert(si);
//    system -> printStatus = true;
  }

  void TearDown() override {
    delete system;
  }

};

byte mov_a_direct[] = {
  MOV_A_CONST, 0x42,
  HLT
};

TEST_F(ControllerTest, testMovADirect) {
  mem -> initialize(ROM_START, 3, mov_a_direct);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_A_CONST);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov a, #42 takes 4 cycles. hlt takes 3.
  ASSERT_EQ(system -> run(true), 7);
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
  MOV_A_ADDR, 0x04, 0x80,
  HLT,
  0x42
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

byte mov_si_direct[] = {
  MOV_SI_CONST, 0x42, 0x37,
  HLT,
};

TEST_F(ControllerTest, testMovSiDirect) {
  mem -> initialize(ROM_START, 4, mov_si_direct);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_SI_CONST);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov si, #3742 takes 6 cycles. hlt takes 3.
  system -> cycles(9);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(si -> getValue(), 0x3742);
}

byte mov_si_absolute[] = {
  MOV_SI_ADDR, 0x04, 0x80,
  HLT,
  0x42, 0x37
};

TEST_F(ControllerTest, testMovSiAbsolute) {
  mem -> initialize(ROM_START, 6, mov_si_absolute);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_SI_ADDR);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov si, (8004) takes 10 cycles. hlt takes 3.
  system -> cycles(13);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(si -> getValue(), 0x3742);
}
