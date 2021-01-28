//
// Created by jan on 2021-01-25.
//

#ifndef EMU_MEMORY_H
#define EMU_MEMORY_H

#include "system.h"

struct MemImage {
  word address;
  word size;
  byte *contents;
};

class Memory : public OwnedComponent {
private:
  word  ram_start;
  word  ram_size;
  word  rom_start;
  word  rom_size;
  byte *ram;
  byte *rom;
  word  address_ptr = 0;

public:
  Memory(System *s, word, word, word, word, MemImage * = nullptr);
  ~Memory() override;

  constexpr static byte MEM_ID = 0x7;
  constexpr static byte ADDR_ID = 0xF;

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

  void setAddress(word addr) {
    address_ptr = addr;
  }

  word getAddress() const {
    return address_ptr;
  }

  SystemError status() override;
  SystemError reset() override;
  SystemError onRisingClockEdge() override;
  SystemError onHighClock() override;
};

#endif //EMU_MEMORY_H
