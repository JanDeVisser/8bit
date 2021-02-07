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

byte mov_a_direct[] = {
  MOV_A_CONST, 0x42,
  HLT
};

MemImage img_mov_a_direct = {
  .address = ROM_START, .size = 0x03, .contents = mov_a_direct
};

byte mov_a_absolute[] = {
  MOV_A_ADDR,
  0x04,
  0x80,
  HLT,
  0x42
};

MemImage img_mov_a_absolute = {
  .address = ROM_START, .size = 0x05, .contents = mov_a_absolute
};

class ControllerTest : public ::testing::Test {
protected:
  Harness *system = nullptr;
  Memory *mem = nullptr;
  Controller *c = nullptr;
  Register *gp_a = new Register(0x0);
  Register *gp_b = new Register(0x1);
  AddressRegister *pc = new AddressRegister(PC, "PC");
  AddressRegister *tx = new AddressRegister(TX, "TX");

  void SetUp() override {
    mem = new Memory(RAM_START, RAM_SIZE, ROM_START, ROM_SIZE, nullptr);

    c = new Controller(mc);
    system = new Harness();
    system -> insert(mem);
    system -> insert(c);
    system -> insert(gp_a);
    system -> insert(gp_b);
    system -> insert(pc);
    system -> insert(tx);
    system -> printStatus = true;
  }

  void TearDown() override {
    delete system;
  }

};

TEST_F(ControllerTest, testMovADirect) {
  mem -> initialize(&img_mov_a_direct);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_A_CONST);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov a, #42 takes 4 cycles. hlt takes 3.
  system -> cycles(7);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
}

TEST_F(ControllerTest, testMovAAbsolute) {
  mem -> initialize(&img_mov_a_absolute);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_A_ADDR);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  // mov a, (8004) takes 8 cycles. hlt takes 3.
  system -> cycles(11);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
}

