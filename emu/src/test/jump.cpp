#define TESTNAME Jump
#include "controllertest.h"

const byte jmp_basic[] = {
  /* 2000 */ JMP, 0x06, 0x20,
  /* 2003 */ MOV_A_CONST, 0x37,
  /* 2005 */ HLT,
  /* 2006 */ MOV_A_CONST, 0x42,
  /* 2008 */ HLT,
};

void test_jump(Harness *system, byte opcode, bool ok) {
  auto *mem = dynamic_cast<Memory *>(system -> component(MEMADDR));
  mem -> initialize(RAM_START, 9, jmp_basic);
  ASSERT_EQ((*mem)[RAM_START], JMP);
  (*mem)[RAM_START] = opcode;

  auto *pc = dynamic_cast<AddressRegister *>(system -> component(PC));
  pc -> setValue(RAM_START);
  ASSERT_EQ(pc -> getValue(), RAM_START);

  // jmp            7 cycles
  // mov a, #xx     4 cycles
  // hlt            3 cycles
  // total         14
  auto cycles = system -> run();
  ASSERT_EQ(system -> error, NoError);
  ASSERT_EQ(cycles, (ok) ? 14 : 13);
  ASSERT_EQ(system -> bus.halt(), false);
}

TEST_F(TESTNAME, jmp) {
  test_jump(system, JMP, true);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
}

TEST_F(TESTNAME, jcCarrySet) {
  system -> bus.setFlag(SystemBus::ProcessorFlags::C);
  test_jump(system, JC, true);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
}

TEST_F(TESTNAME, jcCarryNotSet) {
  system -> bus.clearFlag(SystemBus::ProcessorFlags::C);
  test_jump(system, JC, false);
  ASSERT_EQ(gp_a -> getValue(), 0x37);
}

TEST_F(TESTNAME, jnzZeroNotSet) {
  system -> bus.clearFlag(SystemBus::ProcessorFlags::Z);
  test_jump(system, JNZ, true);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
}

TEST_F(TESTNAME, jnzZeroSet) {
  system -> bus.setFlag(SystemBus::ProcessorFlags::Z);
  test_jump(system, JNZ, false);
  ASSERT_EQ(gp_a -> getValue(), 0x37);
}

TEST_F(TESTNAME, jvCarrySet) {
  system -> bus.setFlag(SystemBus::ProcessorFlags::V);
  test_jump(system, JV, true);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
}

TEST_F(TESTNAME, jvOverflowNotSet) {
  system -> bus.clearFlag(SystemBus::ProcessorFlags::V);
  test_jump(system, JV, false);
  ASSERT_EQ(gp_a -> getValue(), 0x37);
}

const byte asm_call[] = {
  /* 8000 */ MOV_A_CONST, 0x37,      //  4 cycles
  /* 8002 */ CALL, 0x06, 0x80,       // 11
  /* 8005 */ HLT,                    //  3
  /* 8006 */ MOV_A_CONST, 0x42,      //  4
  /* 8008 */ RET,                    //  6
};                            // Total: 28

TEST_F(TESTNAME, call) {
  mem -> initialize(ROM_START, 9, asm_call);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_A_CONST);

  sp -> setValue(RAM_START);
  ASSERT_EQ(sp -> getValue(), RAM_START);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  auto cycles = system -> run();
  ASSERT_EQ(system -> error, NoError);
  ASSERT_EQ(cycles, 28);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
}

const byte asm_call_abs[] = {
  /* 8000 */ MOV_A_CONST, 0x37,      //  4 cycles
  /* 8002 */ CALL_ABS, 0x09, 0x80,   // 14
  /* 8005 */ HLT,                    //  3
  /* 8006 */ MOV_A_CONST, 0x42,      //  4
  /* 8008 */ RET,                    //  6
  /* 8009 */ 0x06, 0x80
};                            // Total: 28

TEST_F(TESTNAME, call_abs) {
  mem -> initialize(ROM_START, 11, asm_call_abs);
  ASSERT_EQ((*mem)[START_VECTOR], MOV_A_CONST);

  sp -> setValue(RAM_START);
  ASSERT_EQ(sp -> getValue(), RAM_START);

  pc -> setValue(START_VECTOR);
  ASSERT_EQ(pc -> getValue(), START_VECTOR);

  auto cycles = system -> run();
  ASSERT_EQ(system -> error, NoError);
  ASSERT_EQ(cycles, 31);
  ASSERT_EQ(system -> bus.halt(), false);
  ASSERT_EQ(gp_a -> getValue(), 0x42);
}

