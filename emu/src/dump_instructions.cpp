#include <iostream>

#include "alu.h"
#include "controller.h"
#include "src/cpu/microcode.inc"

int main(int argc, char **argv) {
  std::cout << "const char * MNEMONIC[256] = {" << std::endl;
  for (int ix = 0; ix < 256; ix++) {
    MicroCode &m = mc[ix];
    if (m.opcode == ix) {
      std::cout << "  \"" << m.instruction << "\"," << std::endl;
    } else {
      std::cout << "  nullptr," << std::endl;
    }
  }
  std::cout << "}" << std::endl;
}
