#include <algorithm>
#include <iostream>
#include <cstring>
#include "memory.h"

Memory::Memory(word ramStart, word ramSize, word romStart, word romSize, MemImage *image)
    : AddressRegister(ADDR_ID, "M"), ram_start(ramStart), ram_size(ramSize), rom_start(romStart),  rom_size(romSize),
      rom(), ram() {
  ram.resize(ram_size);
  rom.resize(rom_size);
  initialize(image);
}

Memory::~Memory() {
}

void Memory::erase() {
  std::fill(ram.begin(), ram.end(), 0);
  std::fill(rom.begin(), rom.end(), 0);
}

void Memory::add(MemImage *image) {
  if (image) {
    add(image->address, image->size, image->contents);
  }
}

void Memory::add(word address, word size, const byte *contents) {
  if (contents) {
    std::vector<byte> *mem = nullptr;
    if (inRAM(address) && inRAM(address + size)) {
      mem = &ram;
    } else if (inROM(address) && inROM(address + size)) {
      mem = &rom;
    }
    if (mem) {
      for (int ix = 0; ix < size; ix++) {
        (*mem)[ix] = contents[ix];
      }
    }
    sendEvent(EV_IMAGELOADED);
  }
}

void Memory::initialize(MemImage *image) {
  erase();
  add(image);
}

void Memory::initialize(word address, word size, const byte *contents) {
  erase();
  add(address, size, contents);
}


SystemError Memory::status() {
  printf("%1x. M  %04x   CONTENTS %1x. [%02x]\n", id(), getValue(),
         MEM_ID, (isMapped(getValue()) ? (*this)[getValue()] : 0xFF));
  return NoError;
}

SystemError Memory::onRisingClockEdge() {
  if (!bus()->xdata() && (bus()->getID() == MEM_ID)) {
    if (!isMapped(getValue())) {
      return ProtectedMemory;
    }
    bus()->putOnDataBus((*this)[getValue()]);
  }
  return NoError;
}

SystemError Memory::onHighClock() {
  if (!bus()->xdata() && (bus()->putID() == MEM_ID)) {
    if (!inRAM(getValue())) {
      return ProtectedMemory;
    }
    (*this)[getValue()] = bus()->readDataBus();
    sendEvent(EV_CONTENTSCHANGED);
  } else if (bus()->putID() == ADDR_ID) {
    if (!(bus() -> xaddr())) {
      setValue(((word) bus()->readAddrBus() << 8) | ((word) bus()->readDataBus()));
    } else if (!(bus() -> xdata())) {
      if (!(bus()->opflags() & SystemBus::MSB)) {
        setValue((getValue() & 0xFF00) | bus()->readDataBus());
      } else {
        setValue((getValue() & 0x00FF) | (((word) bus()->readDataBus()) << 8));
      }
    }
  }
  return NoError;
}

