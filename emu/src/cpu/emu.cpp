#include <iostream>
#include "backplane.h"

int main() {
  auto *system = new BackPlane();
  system -> defaultSetup();
  system -> run();
  return 0;
}
