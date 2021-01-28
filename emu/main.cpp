#include <iostream>
#include "realsystem.h"

int main() {
  System *s = new RealSystem();
  s -> run();
  return 0;
}
