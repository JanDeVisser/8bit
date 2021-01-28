//
// Created by jan on 2021-01-25.
//

#include "component.h"

std::ostream & printhex(std::ostream &out, byte val) {
  if (val) {
    auto f = out.fill();
    out << std::showbase << std::hex << std::setw(2) << std::setfill('0') << std::internal << val << std::setfill(f);
  } else {
    out << "0x00";
  }
  return out;
}

std::ostream & printhex(std::ostream &out, word val) {
  if (val) {
    auto f = out.fill();
    out << std::showbase << std::hex << std::setw(4) << std::setfill('0') << std::internal << val << std::setfill(f);
  } else {
    out << "0x0000";
  }
  return out;
}

