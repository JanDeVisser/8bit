#include <chrono>
#include <iostream>
#include <gtest/gtest.h>
#include "memory.h"
#include "mocksystem.h"


byte mem[] = { 0x01, 0x42, 0xFF };
MemImage image = {
  .address = 0x8000, .size = 0x03, .contents = mem
};


class MemoryTest : public ::testing::Test {
protected:
  MockSystem *system = nullptr;
  Memory *mem = nullptr;

  void SetUp() override {
    system = new MockSystem();
    mem = new Memory(system, 0x0000, 0x2000, 0x8000, 0x2000, &image);
    (*mem)[0x0000] = 0x42;
    (*mem)[0x0001] = 0x37;
    (*mem)[0x0002] = 0x55;
    system -> reg = mem;
  }

  void TearDown() override {
    delete system;
  }

};

TEST_F(MemoryTest, setMemAddress) {
  SystemError err = system -> cycle(true, false, 1, Memory::ADDR_ID, 0, 0x01, 0x00);
  ASSERT_EQ(err, NoError);
  ASSERT_EQ(mem -> getAddress(), 0x0001);
}

TEST_F(MemoryTest, setMemAddressLSB) {
  mem -> setAddress(0x5555);
  SystemError err = system -> cycle(false, true, 1, Memory::ADDR_ID, 0, 0x01, 0x00);
  ASSERT_EQ(err, NoError);
  ASSERT_EQ(mem -> getAddress(), 0x5501);
}

TEST_F(MemoryTest, setMemAddressMSB) {
  mem -> setAddress(0x5555);
  SystemError err = system -> cycle(false, true, 1, Memory::ADDR_ID, OP_MSB, 0x00, 0x00);
  ASSERT_EQ(err, NoError);
  ASSERT_EQ(mem -> getAddress(), 0x0055);
}

TEST_F(MemoryTest, setMemAddressLSBAndMSB) {
  mem -> setAddress(0x5555);
  SystemError err = system -> cycle(false, true, 1, Memory::ADDR_ID, 0, 0x01, 0x00);
  ASSERT_EQ(err, NoError);
  err = system -> cycle(false, true, 1, Memory::ADDR_ID, OP_MSB, 0x00, 0x00);
  ASSERT_EQ(mem -> getAddress(), 0x0001);
}

TEST_F(MemoryTest, readRAM) {
  SystemError err = system -> cycle(true, false, 1, Memory::ADDR_ID, 0, 0x01, 0x00);
  ASSERT_EQ(err, NoError);
  ASSERT_EQ(mem -> getAddress(), 0x0001);
  err = system -> cycle(false, true, Memory::MEM_ID, 1, 0);
  ASSERT_EQ(err, NoError);
  ASSERT_EQ(system->data_bus, 0x37);
}

TEST_F(MemoryTest, writeRAM) {
  SystemError err = system -> cycle(true, false, 1, Memory::ADDR_ID, 0, 0x01, 0x00);
  ASSERT_EQ(err, NoError);
  ASSERT_EQ(mem -> getAddress(), 0x0001);
  err = system -> cycle(false, true, 1, Memory::MEM_ID, 0, 0x55);
  ASSERT_EQ(err, NoError);
  ASSERT_EQ(system->data_bus, 0x55);
}

TEST_F(MemoryTest, readROM) {
  SystemError err = system -> cycle(true, false, 1, Memory::ADDR_ID, 0, 0x01, 0x80);
  ASSERT_EQ(err, NoError);
  ASSERT_EQ(mem -> getAddress(), 0x8001);
  err = system -> cycle(false, true, Memory::MEM_ID, 1, 0);
  ASSERT_EQ(err, NoError);
  ASSERT_EQ(system->data_bus, 0x42);
}

TEST_F(MemoryTest, writeROM) {
  system -> cycle(true, false, 1, Memory::ADDR_ID, 0, 0x01, 0x80);
  ASSERT_EQ(mem -> getAddress(), 0x8001);
  SystemError err = system -> cycle(false, true, 1, Memory::MEM_ID, 0, 0x55);
  ASSERT_EQ(err, ProtectedMemory);
  ASSERT_EQ((*mem)[0x8001], 0x42);
}


