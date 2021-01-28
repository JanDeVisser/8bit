#include <iostream>
#include <cstring>
#include "memory.h"

Memory::Memory(System *s, word ramStart, word ramSize, word romStart, word romSize, MemImage *image)
  : OwnedComponent(s), ram_start(ramStart), ram_size(ramSize),
    rom_start(romStart),  rom_size(romSize) {
  ram = new byte[ram_size];
  rom = new byte[rom_size];

  if (image) {
    byte *mem = nullptr;
    if (inRAM(image->address) && inRAM(image->address + image->size)) {
      mem = ram;
    } else if (inROM(image->address) && inROM(image->address + image->size)) {
      mem = rom;
    }
    if (mem) {
      memcpy(mem, image->contents, image->size);
    }
  }
}

Memory::~Memory() {
  delete ram;
  delete rom;
}

SystemError Memory::status() {
  printf("M  %04x [%02x]\n", address_ptr, (isMapped(address_ptr) ? (*this)[address_ptr] : 0xFF));
  return NoError;
}

SystemError Memory::reset() {
  address_ptr = 0;
  memset(ram, 0, ram_size);
  return NoError;
}

SystemError Memory::onRisingClockEdge() {
  if (!system()->xdata() && (system()->getID() == 0x07)) {
    if (!isMapped(address_ptr)) {
      return ProtectedMemory;
    }
    system()->putOnBus((*this)[address_ptr]);
  }
  return NoError;
}

SystemError Memory::onHighClock() {
  if (!system()->xdata() && (system()->putID() == 0x07)) {
    if (!inRAM(address_ptr)) {
      return ProtectedMemory;
    }
    (*this)[address_ptr] = system()->readBus();
  } else if (system()->putID() == 0x0F) {
    if (!system() -> xaddr()) {
      address_ptr = (word) ((system()->addrBus() << 8) | system()->readBus());
    } else if (!system() -> xdata()) {
      if (!system()->xdata()) {
        if (!(system()->opflags() & OP_MSB)) {
          address_ptr &= 0xFF00;
          address_ptr |= system()->readBus();
        } else {
          address_ptr &= 0x00FF;
          address_ptr |= ((word) system()->readBus()) << 8;
        }
      }
    }
  }
  return NoError;
}

