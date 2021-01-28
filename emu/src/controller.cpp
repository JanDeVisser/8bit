#include <iostream>
#include <iomanip>
#include <cstdio>

#include "addressregister.h"
#include "controller.h"
#include "register.h"
#include "realsystem.h"

Controller::Controller(System *s, MicroCode *mc) : Register(s, IR), microCode(mc) {
//  for (int ix = 0; mc[ix].instruction; ix++) {
//    //
//  }
}

SystemError Controller::status() {
  printf("IR %02x Step %d\n", getValue(), step);
  return NoError;
}

SystemError Controller::reset() {
  step = 0;
  return NoError;
}

MicroCode * Controller::findMicroCode() {
  for (int ix = 0; microCode[ix].instruction || microCode[ix].step; ix++) {
    if (microCode[ix].instruction == getValue() && microCode[ix].step == step) {
      return microCode + ix;
    }
  }
  return nullptr;
}

SystemError Controller::onLowClock() {
  switch (step) {
    case 0:
      system()->xaddr(PC, MEMADDR, OP_INC);
      break;
    case 1:
      system()->xdata(MEM, IR, OP_NONE);
      break;
    default:
      MicroCode *mc = findMicroCode();
      if (mc) {
        if (mc -> opflags & OP_DONE) {
          step = -1;
        }
        switch (mc->action) {
          case MicroCode::XDATA:
            system()->xdata(mc->src, mc->target, mc->opflags & OP_MASK);
            break;
          case MicroCode::XADDR:
            system()->xaddr(mc->src, mc->target, mc->opflags & OP_MASK);
            break;
          case MicroCode::OTHER:
            switch (mc->opflags & OP_MASK) {
              case OP_HALT:
                fprintf(stderr, "Halting system\n");
                system()->stop();
                break;
              default:
                fprintf(stderr, "Unhandled operation flag '%02x' for instruction %02x, step %d\n",
                        mc -> opflags, getValue(), step);
                return InvalidMicroCode;
            }
            break;
          default:
            fprintf(stderr, "Unhandled microcode action %d for instruction %02x, step %d\n",
                    mc -> action, getValue(), step);
            return InvalidMicroCode;
        }
      } else {
        fprintf(stderr, "No microcode for instruction %02x, step %d\n", getValue(), step);
        return InvalidInstruction;
      }
      break;
  }
  step++;
  return NoError;
}
