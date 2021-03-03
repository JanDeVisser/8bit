#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>
#include "memory.h"

/* ----------------------------------------------------------------------- */

MemoryBank::MemoryBank(word start, word size, bool writable, const byte *image) noexcept
    : m_start(start), m_size(size), m_writable(writable) {
  if ((int(start) + int(size)) > 0xFFFF) {
    m_start = m_size = 0x00;
    return;
  }

  m_image = std::shared_ptr<byte>(new byte[m_size], std::default_delete<byte[]>());
  if (image) {
    memcpy(m_image.get(), image, size);
  } else {
    erase();
  }
}

MemoryBank::MemoryBank(const MemoryBank &other) :
    m_start(other.start()),
    m_size(other.size()),
    m_writable(other.writable()) {
  m_image = other.m_image;
}

MemoryBank::MemoryBank(MemoryBank &&other) noexcept {
  m_start = other.start();
  m_size = other.size();
  m_writable = other.writable();
  m_image = other.m_image;
  other.m_image = nullptr;
  other.m_size = 0;
}

MemoryBank & MemoryBank::operator=(const MemoryBank &other) {
  if (this != &other) {
    m_start = other.start();
    m_size = other.size();
    m_writable = other.writable();
    m_image = other.m_image;
  }
  return *this;
}

MemoryBank & MemoryBank::operator=(MemoryBank &&other) noexcept {
  m_start = other.start();
  m_size = other.size();
  m_writable = other.writable();
  m_image = other.m_image;
  other.m_image = nullptr;
  other.m_size = 0;
  return *this;
}

byte & MemoryBank::operator[](std::size_t addr) {
  if (mapped(addr)) {
    return m_image.get()[offset(addr)];
  } else {
    throw std::exception(); // FIXME
  }
}

const byte & MemoryBank::operator[](std::size_t addr) const {
  if (mapped(addr)) {
    return m_image.get()[offset(addr)];
  } else {
    throw std::exception(); // FIXME
  }
}

void MemoryBank::erase() {
  memset(m_image.get(), 0, m_size);
}

void MemoryBank::copy(MemoryBank &other) {
  if (fits(other.start(), other.size())) {
    for (auto ix = 0; ix < other.size(); ix++) {
      m_image.get()[offset(other.start()) + ix] = other.m_image.get()[ix];
    }
    other.m_image = nullptr;
    other.m_size = 0;
  }
}

void MemoryBank::copy(MemoryBank &&other) {
  copy(other);
}

void MemoryBank::copy(size_t addr, size_t size, const byte *contents) {
  if (fits(addr, size)) {
    for (auto ix = 0; ix < size; ix++) {
      m_image.get()[offset(addr) + ix] = contents[ix];
    }
  }
}

bool MemoryBank::mapped(size_t addr) const {
  return (start() <= addr) && (addr < end());
}

bool MemoryBank::fits(size_t addr, size_t size) const {
  return mapped(addr) && mapped(addr + size);
}

bool MemoryBank::disjointFrom(size_t addr, size_t size) const {
  return ((addr < start()) && ((addr + size) < start())) || (addr >= end());
}

/* ----------------------------------------------------------------------- */

Memory::Memory() : AddressRegister(ADDR_ID, "M"), m_banks() {
}

Memory::Memory(MemoryBank &&bank) : Memory() {
  initialize(bank);
}

Memory::Memory(MemoryBank &bank) : Memory() {
  initialize(bank);
}

Memory::Memory(word ramStart, word ramSize, word romStart, word romSize, MemoryBank &bank)
    : Memory() {
  add(MemoryBank(ramStart, ramSize, true));
  add(MemoryBank(romStart, romSize, false));
  initialize(bank);
}

Memory::Memory(word ramStart, word ramSize, word romStart, word romSize, MemoryBank &&bank)
    : Memory(ramStart, ramSize, romStart, romSize, bank) {
}

const MemoryBank & Memory::findBankForAddress(size_t addr, bool &found) const {
  static MemoryBank dummy;
  found = false;
  for (auto const &bank : m_banks) {
    if (bank.mapped(addr)) {
      found = true;
      return bank;
    }
  }
  return dummy;
}

const MemoryBank & Memory::findBankForBlock(size_t addr, size_t size, bool &found) const {
  static MemoryBank dummy;
  found = false;
  for (auto const &bank : m_banks) {
    if (bank.fits(addr, size)) {
      found = true;
      return bank;
    }
  }
  return dummy;
}

bool Memory::disjointFromAll(size_t addr, size_t size) const {
  for (auto const &bank : m_banks) {
    if (bank.disjointFrom(addr, size)) {
      return true;
    }
  }
  return true;
}

void Memory::erase() {
  for (auto &bank : m_banks) {
    bank.erase();
  }
}

void Memory::add(word address, word size, bool writable, const byte *contents) {
  bool found;
  MemoryBank b = findBankForBlock(address, size, found);
  if (found) {
    b.copy(address, size, contents);
  } else if (disjointFromAll(address, size)) {
    m_banks.emplace_back(address, size, writable, contents);
  } else {
    throw std::exception(); // FIXME
  }
  if (contents) {
    sendEvent(EV_IMAGELOADED);
  }
}

void Memory::add(MemoryBank &&bank) {
  bool found;
  MemoryBank b = findBankForBlock(bank.start(), bank.size(), found);
  if (found) {
    b.copy(bank);
  } else if (disjointFromAll(bank.start(), bank.size())) {
    m_banks.emplace_back(bank);
  } else {
    throw std::exception(); // FIXME
  }
}

void Memory::add(MemoryBank &bank) {
  if (!bank.size()) {
    return;
  }
  bool found;
  MemoryBank b = findBankForBlock(bank.start(), bank.size(), found);
  if (found) {
    b.copy(bank);
  } else if (disjointFromAll(bank.start(), bank.size())) {
    m_banks.push_back(std::move(bank));
  } else {
    throw std::exception(); // FIXME
  }
}

void Memory::initialize() {
  m_banks.clear();
}

void Memory::initialize(word address, word size, const byte *contents, bool writable) {
  initialize(MemoryBank(address, size, writable, contents));
}

void Memory::initialize(MemoryBank &bank) {
  erase();
  add(bank);
}

void Memory::initialize(MemoryBank &&bank) {
  initialize(bank);
}

bool Memory::inRAM(word addr) const {
  bool found;
  MemoryBank bank = findBankForAddress(addr, found);
  return found && bank.writable();
}

bool Memory::inROM(word addr) const {
  bool found;
  MemoryBank bank = findBankForAddress(addr, found);
  return found && !bank.writable();
}

bool Memory::isMapped(word addr) const {
  bool found;
  findBankForAddress(addr, found);
  return found;
}

byte & Memory::operator[](std::size_t addr) {
  static byte dummy = 0xFF;
  bool found;
  MemoryBank bank = findBankForAddress(addr, found);
  if (found) {
    return bank[addr];
  } else {
    return dummy;
  }
}

const byte & Memory::operator[](std::size_t addr) const {
  static byte dummy = 0xFF;
  bool found;
  MemoryBank bank = findBankForAddress(addr, found);
  if (found) {
    return bank[addr];
  } else {
    return dummy;
  }
}

std::ostream & Memory::status(std::ostream &os) {
  char buf[80];
  snprintf(buf, 80, "%1x. M  %04x   CONTENTS %1x. [%02x]", id(), getValue(),
         MEM_ID, (isMapped(getValue()) ? (*this)[getValue()] : 0xFF));
  os << buf << std::endl;
  return os;
}

SystemError Memory::onRisingClockEdge() {
  if ((!bus()->xdata() || !bus()->xaddr() ||
      (!bus()->io() && (bus()->opflags() & SystemBus::IOOut))) &&
      (bus()->getID() == MEM_ID)) {
    if (!isMapped(getValue())) {
      return error(ProtectedMemory);
    }
    bus()->putOnAddrBus(0x00);
    bus()->putOnDataBus((*this)[getValue()]);
  }
  return NoError;
}

SystemError Memory::onHighClock() {
  if (((!bus()->xdata() || !bus()->xaddr()) && (bus()->putID() == MEM_ID)) ||
      (!bus()->io() && (bus()->opflags() & SystemBus::IOIn) && (bus()->getID() == MEM_ID))) {
    if (!inRAM(getValue())) {
      return error(ProtectedMemory);
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

