#include <iostream>
#include <cstring>
#include "memory.h"

Memory::Memory(word ramStart, word ramSize, word romStart, word romSize, MemImage *image)
    : AddressRegister(ADDR_ID, "M"), ram_start(ramStart), ram_size(ramSize), rom_start(romStart),  rom_size(romSize) {
  ram = new byte[ram_size];
  rom = new byte[rom_size];

  add(image);
}

Memory::~Memory() {
  delete ram;
  delete rom;
}

void Memory::add(MemImage *image) {
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

SystemError Memory::status() {
  printf("M  %04x [%02x]\n", getValue(), (isMapped(getValue()) ? (*this)[getValue()] : 0xFF));
  return NoError;
}

SystemError Memory::onRisingClockEdge() {
  if (!bus()->xdata() && (bus()->getID() == 0x07)) {
    if (!isMapped(getValue())) {
      return ProtectedMemory;
    }
    bus()->putOnDataBus((*this)[getValue()]);
  }
  return NoError;
}

SystemError Memory::onHighClock() {
  if (!bus()->xdata() && (bus()->putID() == 0x07)) {
    if (!inRAM(getValue())) {
      return ProtectedMemory;
    }
    (*this)[getValue()] = bus()->readDataBus();
    sendEvent(EV_CONTENTSCHANGED);
  } else if (bus()->putID() == 0x0F) {
    if (!(bus() -> xaddr())) {
      setValue(((word) bus()->readAddrBus() << 8) | ((word) bus()->readDataBus()));
    } else if (!(bus() -> xdata())) {
      if (!(bus()->opflags() & SystemBus::OP_MSB)) {
        setValue((getValue() & 0xFF00) | bus()->readDataBus());
      } else {
        setValue((getValue() & 0x00FF) | (((word) bus()->readDataBus()) << 8));
      }
    }
  }
  return NoError;
}

