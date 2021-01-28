#include <chrono>
#include <iostream>
#include <gtest/gtest.h>
#include "addressregister.h"
#include "mocksystem.h"

static int REGID = 0xC;

class AddressRegisterTest : public ::testing::Test {
protected:
  MockSystem *system = nullptr;
  AddressRegister *reg = nullptr;

  void SetUp() override {
    system = new MockSystem();
    reg = new AddressRegister(system, REGID, "TEST");
    system -> reg = reg;
  }

  void TearDown() override {
    delete system;
  }

};

TEST_F(AddressRegisterTest, canPutLSB) {
  reg -> setValue(0x5555);
  system -> cycle(false, true, 1, REGID, 0, 0x42);
  ASSERT_EQ(reg -> getValue(), 0x5542);
}

TEST_F(AddressRegisterTest, canPutMSB) {
  reg -> setValue(0x5555);
  system -> cycle(false, true, 1, REGID, OP_MSB, 0x42);
  ASSERT_EQ(reg -> getValue(), 0x4255);
}

TEST_F(AddressRegisterTest, canPutLSBThenMSB) {
  reg -> setValue(0x5555);
  system -> cycle(false, true, 1, REGID, 0, 0x37);
  system -> cycle(false, true, 1, REGID, OP_MSB, 0x42);
  ASSERT_EQ(reg -> getValue(), 0x4237);
}

TEST_F(AddressRegisterTest, canPutAddr) {
  reg -> setValue(0x5555);
  system -> cycle(true, false, 1, REGID, 0, 0x42, 0x37);
  ASSERT_EQ(reg -> getValue(), 0x3742);
}

TEST_F(AddressRegisterTest, canGetAddr) {
  reg -> setValue(0x4237);
  system -> cycle(true, false, REGID, 1, 0, 0x72);
  ASSERT_EQ(system->data_bus, 0x37);
  ASSERT_EQ(system->addr_bus, 0x42);
}

TEST_F(AddressRegisterTest, canGetLSB) {
  reg -> setValue(0x4237);
  system -> cycle(false, true, REGID, 1, 0, 0x72);
  ASSERT_EQ(system->data_bus, 0x37);
}

TEST_F(AddressRegisterTest, canGetMSB) {
  reg -> setValue(0x4237);
  system -> cycle(false, true, REGID, 1, OP_MSB, 0x72);
  ASSERT_EQ(system->data_bus, 0x42);
}

TEST_F(AddressRegisterTest, dontPutWhenOtherRegAddressed) {
  reg -> setValue(0x5555);
  system -> cycle(false, true, 1, 2, 0, 0x42);
  ASSERT_EQ(reg -> getValue(), 0x5555);
}

TEST_F(AddressRegisterTest, dontGetWhenOtherRegAddressed) {
  reg -> setValue(0x5555);
  system -> cycle(false, true, 2, 1, 0, 0x37);
  ASSERT_EQ(system->data_bus, 0x37);
}
