#include <chrono>
#include <iostream>
#include <gtest/gtest.h>
#include "register.h"
#include "mocksystem.h"

static int REGID = 0xC;

class RegisterTest : public ::testing::Test {
protected:
  MockSystem *system = nullptr;
  Register *reg = nullptr;

  void SetUp() override {
    system = new MockSystem();
    reg = new Register(system, REGID);
    system -> reg = reg;
  }

  void TearDown() override {
    delete system;
  }

};

TEST_F(RegisterTest, canPut) {
  system -> cycle(false, true, 1, REGID, 0, 0x42);
  ASSERT_EQ(reg -> getValue(), 0x42);
}

TEST_F(RegisterTest, canGet) {
  reg -> setValue(0x42);
  system -> cycle(false, true, REGID, 1, 0, 0x37);
  ASSERT_EQ(system->data_bus, 0x42);
}

TEST_F(RegisterTest, dontPutWhenOtherRegAddressed) {
  reg -> setValue(0x37);
  system -> cycle(false, true, 1, 2, 0, 0x42);
  ASSERT_EQ(reg -> getValue(), 0x37);
}

TEST_F(RegisterTest, dontGetWhenOtherRegAddressed) {
  reg -> setValue(0x42);
  system -> cycle(false, true, 2, 1, 0, 0x37);
  ASSERT_EQ(system->data_bus, 0x37);
}
