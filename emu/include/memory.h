//
// Created by jan on 2021-01-25.
//

#ifndef EMU_MEMORY_H
#define EMU_MEMORY_H

#include <memory>
#include <vector>
#include <cstring>

#include "addressregister.h"
#include "systembus.h"

class MemoryBank {
private:
  word                    m_start;
  word                    m_size;
  bool                    m_writable;
  std::shared_ptr<byte>   m_image;

public:
               MemoryBank() = default;
               MemoryBank(const MemoryBank &);
               MemoryBank(MemoryBank &&) noexcept;
               MemoryBank(word, word, bool = true, const byte * = nullptr) noexcept;
               ~MemoryBank() = default;
  MemoryBank & operator=(const MemoryBank &);
  MemoryBank & operator=(MemoryBank &&) noexcept;
  byte &       operator[](std::size_t);
  const byte & operator[](std::size_t) const;

  bool         mapped(size_t) const;
  bool         fits(size_t, size_t) const;
  bool         disjointFrom(size_t, size_t) const;
  void         erase();
  void         copy(size_t, size_t, const byte *);
  void         copy(MemoryBank &);
  void         copy(MemoryBank &&);
  word         offset(size_t addr) const { return addr - start(); }
  word         start() const    { return m_start;          }
  word         size() const     { return m_size;           }
  word         end() const      { return m_start + m_size; }
  bool         writable() const { return m_writable;       }
};

class Memory : public AddressRegister {
private:
  std::vector<MemoryBank> m_banks;

  const MemoryBank & findBankForAddress(size_t, bool &) const;
  const MemoryBank & findBankForBlock(size_t, size_t, bool &) const;
  bool               disjointFromAll(size_t, size_t) const;

public:
                     Memory();
                     Memory(Memory &) = delete;
                     Memory(Memory &&) = delete;
                     Memory(word, word, word, word, MemoryBank && = MemoryBank());
                     Memory(word, word, word, word, MemoryBank &);
  explicit           Memory(MemoryBank &&);
  explicit           Memory(MemoryBank &);
                     ~Memory() override = default;
  void               erase();
  void               add(word, word, bool = true, const byte * = nullptr);
  void               add(MemoryBank &&);
  void               add(MemoryBank &);
  void               initialize(word, word, const byte * = nullptr, bool = true);
  void               initialize();
  void               initialize(MemoryBank &&);
  void               initialize(MemoryBank &);
  bool               inRAM(word) const;
  bool               inROM(word) const;
  bool               isMapped(word) const;
  byte &             operator[](std::size_t);
  const byte &       operator[](std::size_t) const;
  std::ostream &     status(std::ostream &) override;
  SystemError        onRisingClockEdge() override;
  SystemError        onHighClock() override;

  constexpr static byte  MEM_ID = 0x7;
  constexpr static byte  ADDR_ID = 0xF;
  constexpr static int   EV_CONTENTSCHANGED = 0x04;
  constexpr static int   EV_IMAGELOADED = 0x05;
};

#endif //EMU_MEMORY_H
