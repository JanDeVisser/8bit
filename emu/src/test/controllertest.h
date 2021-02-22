#ifndef EMU_CONTROLLERTEST_H
#define EMU_CONTROLLERTEST_H

#include <iostream>
#include <gtest/gtest.h>
#include "alu.h"
#include "memory.h"
#include "controller.h"
#include "harness.h"

#include "src/cpu/microcode.inc"

constexpr word RAM_START = 0x2000;
constexpr word RAM_SIZE = 0x2000;
constexpr word ROM_START = 0x8000;
constexpr word ROM_SIZE = 0x2000;
constexpr word START_VECTOR = ROM_START;
constexpr word RAM_VECTOR = RAM_START;


class TESTNAME : public ::testing::Test, public ComponentListener {
protected:
  Harness *system = nullptr;
  Memory *mem = new Memory(RAM_START, RAM_SIZE, ROM_START, ROM_SIZE, nullptr);
  Controller *c = new Controller(mc);
  Register *gp_a = new Register(0x0);
  Register *gp_b = new Register(0x1);
  Register *gp_c = new Register(0x2);
  Register *gp_d = new Register(0x3);
  AddressRegister *pc = new AddressRegister(PC, "PC");
  AddressRegister *tx = new AddressRegister(TX, "TX");
  AddressRegister *sp = new AddressRegister(SP, "SP");
  AddressRegister *si = new AddressRegister(Si, "Si");
  AddressRegister *di = new AddressRegister(Di, "Di");
  ALU *alu = new ALU(RHS, new Register(LHS));

  void SetUp() override {
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
    system -> insert(alu);
    system -> insert(alu -> lhs());
    c -> setListener(this);
//    system -> printStatus = true;
  }

  void TearDown() override {
    delete system;
  }

  void componentEvent(Component *sender, int ev) {
    if (system -> printStatus && (ev == Controller::EV_AFTERINSTRUCTION)) {
      system -> status("After Instruction", 0);
    }
  }

};

extern const byte unary_op[];
extern const byte binary_op[];

#endif //EMU_CONTROLLERTEST_H