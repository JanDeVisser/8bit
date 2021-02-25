//
// Created by jan on 2021-01-25.
//

#ifndef EMU_MEMORY_H
#define EMU_MEMORY_H

#include <vector>
#include <cstring>

#include "addressregister.h"
#include "systembus.h"

struct MemImage {
  word        address;
  word        size;
  const byte *contents;

  MemImage(word addr, word sz, const byte *d)
      : address(addr), size(sz) {
    contents = new byte[sz];
      memcpy((void *) contents, (void *) d, sz);
  }
};

class Memory : public AddressRegister {
private:
  word              ram_start;
  word              ram_size;
  word              rom_start;
  word              rom_size;
  std::vector<byte> ram;
  std::vector<byte> rom;

public:
  Memory(word, word, word, word, MemImage * = nullptr);
  ~Memory() override;

  constexpr static byte MEM_ID = 0x7;
  constexpr static byte ADDR_ID = 0xF;
  constexpr static int EV_CONTENTSCHANGED = 0x04;
  constexpr static int EV_IMAGELOADED = 0x05;

  void erase();
  void add(word, word, const byte *);
  void add(MemImage *);
  void initialize(MemImage *);
  void initialize(word, word, const byte *);

  bool inRAM(word addr) const {
    return (addr >= ram_start) && (addr < (ram_start + ram_size));
  }

  bool inROM(word addr) const {
    return (addr >= rom_start) && (addr < (rom_start + rom_size));
  }

  bool isMapped(word addr) const {
    return inRAM(addr) || inROM(addr);
  }

  byte & operator[](std::size_t addr) {
    if (inRAM(addr)) {
      return ram[addr - ram_start];
    } else if (inROM(addr)) {
      return rom[addr - rom_start];
    } else {
      throw std::exception(); // FIXME
    }
  }

  const byte & operator[](std::size_t addr) const {
    if (inRAM(addr)) {
      return ram[addr - ram_start];
    } else if (inROM(addr)) {
      return rom[addr - rom_start];
    } else {
      throw std::exception(); // FIXME
    }
  }

  word ramStart() const {
    return ram_start;
  }

  word ramEnd() const {
    return ram_start + ram_size;
  }

  word ramSize() const {
    return ram_size;
  }

  SystemError status() override;
  SystemError onRisingClockEdge() override;
  SystemError onHighClock() override;
};

#endif //EMU_MEMORY_H
