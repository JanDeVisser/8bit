#!/usr/bin/env python

import argparse
import os.path
import re
import sys

opcodes = {
    "nop": 0,
    "mov a,#%02x": 1,
    "mov a,*%04x": 2,
    "mov a,b": 3,
    "mov a,c": 4,
    "mov a,d": 5,
    "mov b,#%02x": 6,
    "mov b,*%04x": 7,
    "mov b,a": 8,
    "mov b,c": 9,
    "mov b,d": 10,
    "mov c,#%02x": 11,
    "mov c,*%04x": 12,
    "mov c,a": 13,
    "mov c,b": 14,
    "mov c,d": 15,
    "mov d,#%02x": 16,
    "mov d,*%04x": 17,
    "mov d,a": 18,
    "mov d,b": 19,
    "mov d,c": 20,
    "mov sp,#%04x": 21,
    "mov sp,*%04x": 22,
    "mov sp,si": 23,
    "mov si,#%04x": 24,
    "mov si,*%04x": 25,
    "mov si,cd": 26,
    "mov di,#%04x": 27,
    "mov di,*%04x": 28,
    "mov di,cd": 29,
    "mov a,*si": 30,
    "mov b,*si": 31,
    "mov c,*si": 32,
    "mov d,*si": 33,
    "mov a,*di": 34,
    "mov b,*di": 35,
    "mov c,*di": 36,
    "mov d,*di": 37,
    "mov *di,*si": 38,
    "jmp #%04x": 39,
    "jnz #%04x": 40,
    "jc #%04x": 41,
    "jv #%04x": 42,
    "call #%04x": 43,
    "ret": 44,
    "push a": 45,
    "push b": 46,
    "push c": 47,
    "push d": 48,
    "push si": 49,
    "push di": 50,
    "pop a": 51,
    "pop b": 52,
    "pop c": 53,
    "pop d": 54,
    "pop si": 55,
    "pop di": 56,
    "mov *%04x,a": 57,
    "mov *di,a": 58,
    "mov *%04x,b": 59,
    "mov *di,b": 60,
    "mov *%04x,c": 61,
    "mov *di,c": 62,
    "mov *%04x,d": 63,
    "mov *di,d": 64,
    "mov *%04x,si": 65,
    "mov *%04x,di": 66,
    "mov *%04x,cd": 67,
    "mov *si,cd": 68,
    "mov *di,cd": 69,
    "add a,b": 70,
    "adc a,b": 71,
    "sub a,b": 72,
    "sbb a,b": 73,
    "and a,b": 74,
    "or a,b": 75,
    "xor a,b": 76,
    "not a": 77,
    "shl a": 78,
    "shr a": 79,
    "add a,c": 80,
    "adc a,c": 81,
    "sub a,c": 82,
    "sbb a,c": 83,
    "and a,c": 84,
    "or a,c": 85,
    "xor a,c": 86,
    "add a,d": 87,
    "adc a,d": 88,
    "sub a,d": 89,
    "sbb a,d": 90,
    "and a,d": 91,
    "or a,d": 92,
    "xor a,d": 93,
    "add b,c": 94,
    "adc b,c": 95,
    "sub b,c": 96,
    "sbb b,c": 97,
    "and b,c": 98,
    "or b,c": 99,
    "xor b,c": 100,
    "not b": 101,
    "shl b": 102,
    "shr b": 103,
    "add b,d": 104,
    "adc b,d": 105,
    "sub b,d": 106,
    "sbb b,d": 107,
    "and b,d": 108,
    "or b,d": 109,
    "xor b,d": 110,
    "add c,d": 111,
    "adc c,d": 112,
    "sub c,d": 113,
    "sbb c,d": 114,
    "and c,d": 115,
    "or c,d": 116,
    "xor c,d": 117,
    "not c": 118,
    "shl c": 119,
    "shr c": 120,
    "not d": 121,
    "shl d": 122,
    "shr d": 123,
    "clr a": 124,
    "clr b": 125,
    "clr c": 126,
    "clr d": 127,
    "swp a,b": 128,
    "swp a,c": 129,
    "swp a,d": 130,
    "swp b,c": 131,
    "swp b,d": 132,
    "swp c,d": 133,
    "add ab,cd": 134,
    "adc ab,cd": 135,
    "sub ab,cd": 136,
    "sbb ab,cd": 137,
    "jmp *%04x": 138,
    "jnz *%04x": 139,
    "jc *%04x": 140,
    "jv *%04x": 141,
    "call *%04x": 142,
    "cmp a,b": 143,
    "cmp a,c": 144,
    "cmp a,d": 145,
    "cmp b,c": 146,
    "cmp b,d": 147,
    "cmp c,d": 148,
    "inc a": 149,
    "inc b": 150,
    "inc c": 151,
    "inc d": 152,
    "dec a": 153,
    "dec b": 154,
    "dec c": 155,
    "dec d": 156,
    "inc si": 157,
    "inc di": 158,
    "dec si": 159,
    "dec di": 160,
    "out #%02x, a": 161,
    "out #%02x, b": 162,
    "out #%02x, c": 163,
    "out #%02x, d": 164,
    "in a, #%02x": 165,
    "in b, #%02x": 166,
    "in c, #%02x": 167,
    "in d, #%02x": 168,
    "pushfl": 169,
    "popfl": 170,
    "clrfl": 171,
    "rti": 253,
    "nmi #%04x": 254,
    "hlt": 255,
}


def _split(line: str):

    # Clean the input string by:
    # - Stripping leading and trailing whitespace
    # - Collapsing sequences of spaces and/or tabs into a single space.
    def clean(s):
        def _merge_spaces(s):
            merged = s.replace("  ", " ")
            return _merge_spaces(merged) if merged != s else s

        return _merge_spaces(s.strip().replace("\t", " "))

    # "Clean" the line. clean() will merge consecutive spaces/tabs into a single space.
    c = clean(line)

    # If the line is empty after cleaning, return the empty list. Otherwise split on spaces.
    return c.split() if c != "" else []


def split(line):
    state = 0
    ix = 0
    start_quote = 0
    while ix < len(line):
        if state == 0:
            if line[ix] in '\'"`':
                start_quote = ix
                state = line[ix]
            elif line[ix] == '\\':
                ix += 1
            elif line[ix] == ';':
                return split(line[:ix])
        elif state in '"\'' and line[ix] == state:
            ret = split(line[:start_quote])
            ret.append(line[start_quote:ix+1])
            ret.extend(split(line[ix+1:]))
            return ret
        ix += 1
    return _split(line)


def to_bytes(count, *data):
    ret = []
    for d in data:
        if d is None or d == '':
            pass
        if isinstance(d, bytes):
            for b in d:
                ret.append(b)      
        if isinstance(d, str):
            s: str = d
            if s[0] in '"\'' and s[-1] == s[0] and len(s) > 2:
                val = bytes(s[1:-1], "utf-8")[0]
            elif s[0:2].lower() in ("0b", "0", "0x"):
                val = int(s, 0)
            elif (len(s) - 1) % 8 == 0 and s[0].lower() == "b":
                val = int(s[1:], 2)
            elif (len(s) - 1) % 2 == 0 and s[0] in "$xX":
                val = int(s[1:], 16)
            elif s.isdigit():
                val = int(s)
            elif count == 0:
                ret.extend(to_bytes(1, *[b.strip() for b in s.split()]))
                continue
            else:
                raise ValueError(f"Invalid byte value {d}")
            ret.append(val % 256)
            if count == 2:
                ret.append(val // 256)
        elif isinstance(d, int):
            ret.append(d % 256)
            if count == 2:
                ret.append(d // 256)
        else:
            raise ValueError(f"Cannot convert {d} to bytes")

    if 0 < count != len(ret):
        raise ValueError(f'Expected {count} bytes in {" ".join(data)}, got {len(ret)}')

    return ret


def bytes_to_str(bytes, prefix="", address=None, radix=True):
    lines = []
    b = bytes
    r = "0x" if radix else ""
    while len(b):
        eight = b[0:8]
        if address is None:
            fmt = "{prefix}" + " ".join([f'{r}{{{ix}:02x}}' for ix in range(0, len(eight))])
            s = fmt.format(*eight, prefix=prefix)
        else:
            fmt = "{prefix}{address:04x}  " + " ".join([f'{r}{{{ix}:02x}}' for ix in range(0, len(eight))])
            s = fmt.format(*eight, address=address, prefix=prefix)
            address += 8
        lines.append(s)
        b = b[8:]
    return "\n".join(lines)


class Entry:
    def __init__(self, *tokens):
        self.bytes = 0
        self.prefix = None
        self._errors = []

    def errors(self):
        return self._errors

    def append_to(self, image):
        pass


class Bytes(Entry):
    def __init__(self, mnemonic, *args):
        super(Bytes, self).__init__(mnemonic, *args)
        self.mnemonic = mnemonic
        self.bytes_ = None
        if mnemonic == "db":
            self.bytes_ = to_bytes(1, *args)
        elif mnemonic == "dw":
            self.bytes_ = to_bytes(2, *args)
        elif mnemonic == "data":
            self.bytes_ = to_bytes(0, *args)
        self.bytes = len(self.bytes_) if self.bytes_ else 0

    def __str__(self):
        return f'{self.mnemonic} {bytes_to_str(self.bytes_) if self.bytes_ else ""}'

    def append_to(self, image):
        if self.bytes_:
            image.append(self.bytes_)


class String(Entry):
    def __init__(self, mnemonic, *args):
        super(String, self).__init__(mnemonic, *args)
        self.mnemonic = mnemonic
        self.string = " ".join(t if t[0] not in '"\'' else t[1:-1] for t in args)
        self.bytes = len(self.string)
        if self.mnemonic == "asciz":
            self.bytes += 1

    def __str__(self):
        return f'{self.mnemonic} "{self.string}"'

    def append_to(self, image):
        image.append(self.string)
        if self.mnemonic == "asciz":
            image.append(0)


class Instruction(Entry):
    def __init__(self, mnemonic, *args):
        super(Instruction, self).__init__(mnemonic, *args)
        self.error = None
        self.opcode = 0
        self.mnemonic = mnemonic
        self.arguments = [s.strip() for s in "".join(args).split(",")] if args else []
        self.constants = None
        self.label = None
        self.bytes = 1
        self.canonical = f'{mnemonic} {",".join(self.arguments)}' if self.arguments else mnemonic

    def __str__(self):
        m = re.search(r'%0([24])x', self.canonical)
        if m:
            if self.label:
                fmt = self.canonical.replace(m.group(0), '{0}')
                val = self.label
            elif m.group(1) == '2':
                fmt = self.canonical.replace(m.group(0), '{0:02x}')
                val = self.constants[0] % 256
            else:
                fmt = self.canonical.replace(m.group(0), '{0:04}')
                val = 256*self.constants[1] + self.constants[0]
            return fmt.format(val)
        else:
            return self.canonical

    def append_to(self, image):
        if self.canonical in opcodes:
            self.opcode = opcodes[self.canonical]
            if self.label:
                if image.has_label(self.label):
                    label = image.label(self.label)
                    self.label = None
                    self.constants = [label.value % 256, label.value // 256] \
                        if self.bytes == 3 \
                        else [label.value % 256]
                else:
                    self._errors.append(f'Label {self.label} not defined')
            if not self.label:
                const_len = len(self.constants) if self.constants else 0
                if self.bytes != (const_len + 1):
                    raise ParseException(f"Opcode '{self.canonical}' needs {self.bytes - 1} constant bytes")

                image.append(self.opcode)
                if self.constants:
                    image.append(*self.constants)
        else:
            self._errors.append(f'Canonical opcode {self.canonical} not defined')


class ParseException(Exception):
    def __init__(self, msg):
        self.msg = msg

    def __str__(self):
        return self.msg


class Nop(Instruction):
    def __init__(self, mnemonic, *args):
        super(Nop, self).__init__(mnemonic, *args)
        if self.arguments:
            self.error = f"{self.mnemonic} takes no arguments"


class OneArg(Instruction):
    def __init__(self, mnemonic, *args):
        super(OneArg, self).__init__(mnemonic, *args)
        if len(self.arguments) != 1:
            self.error = f"{self.mnemonic} takes exactly one argument"


class TwoArg(Instruction):
    def __init__(self, mnemonic, *args):
        super(TwoArg, self).__init__(mnemonic, *args)
        if len(self.arguments) != 2:
            self.error = f"{self.mnemonic} takes exactly one argument"


def _parse_operand(op):
    ret = {
        "bytes": 1,
        "constants": None,
        "canonical": op,
        "label": None,
    }
    if re.match(r'([*#])(?:\$|(?:0x))([\dAaBbCcDdEeFf]{1,4})', op):
        m = re.match(r'([*#])(?:\$|(?:0x))([\dAaBbCcDdEeFf]{1,4})', op)
        val = int(m.group(2), 16)
        ret["canonical"] = f'{m.group(1)}%04x'
        ret["bytes"] = -1
        ret["constants"] = [val]
    elif re.match(r'[*#]\d{1,5}', op):
        m = re.match(r'([*#])(\d{1,5})', op)
        val = int(m.group(2))
        ret["canonical"] = f'{m.group(1)}%04x'
        ret["bytes"] = -1
        ret["constants"] = [val]
    elif re.match(r'[*#]?[a-zA-Z_][\w]*', op):
        m = re.match(r'([*#]?)([a-zA-Z_][\w]*)', op)
        reg = m.group(2)
        if reg in reserved:
            ret["canonical"] = op
        else:
            ret["label"] = reg
            prefix = m.group(1) if m.group(1) else "#"
            ret["canonical"] = f'{prefix}%04x'
            ret["bytes"] = 3
    return ret


class Jump(Instruction):

    def __init__(self, mnemonic, *args):
        super(Jump, self).__init__(mnemonic, *args)
        if len(self.arguments) != 1:
            raise ParseException(f"Invalid {self.mnemonic} arguments '{' '.join(self.arguments)}'")
        if not self.arguments[0]:
            raise ParseException(f'No address specified in {self.canonical}')

        addr = _parse_operand(self.arguments[0])
        self.constants = addr["constants"]
        self.bytes = addr["bytes"]
        self.label = addr["label"]

        self.canonical = f'{self.mnemonic} {addr["canonical"]}'


class Move(Instruction):

    def __init__(self, mnemonic, *args):
        super(Move, self).__init__(mnemonic, *args)
        if len(self.arguments) != 2:
            raise ParseException(f"Invalid {self.mnemonic} arguments '{' '.join(self.arguments)}'")
        if not self.arguments[0]:
            raise ParseException(f'No destination specified in {self.canonical}')
        if not self.arguments[1]:
            raise ParseException(f'No target specified in {self.canonical}')

        dest = _parse_operand(self.arguments[0])
        src = _parse_operand(self.arguments[1])


        # HACK
        if src["bytes"] < 0:
            val = src["constants"][0] if src["constants"] else None
            if dest["canonical"] in reserved and len(dest["canonical"]) == 1 and src["canonical"][0] == "#":
                src["bytes"] = 2
                if val is not None:
                    src["constants"] = [val % 256]
                src["canonical"] = '#%02x'
            else:
                src["bytes"] = 3
                if val is not None:
                    src["constants"] = [val % 256, val // 256]

        self.constants = src["constants"] if src["constants"] else dest["constants"]
        self.bytes = max(src["bytes"], dest["bytes"])
        self.label = src["label"] if src["label"] else dest["label"]

        self.canonical = f'{self.mnemonic} {dest["canonical"]},{src["canonical"]}'


mnemonics = {
    "clrfl":  Nop,
    "hlt":    Nop,
    "nop":    Nop,
    "popfl":  Nop,
    "pushfl": Nop,
    "ret":    Nop,
    "mov":    Move,
    "clr":    OneArg,
    "dec":    OneArg,
    "inc":    OneArg,
    "not":    OneArg,
    "push":   OneArg,
    "pop":    OneArg,
    "shl":    OneArg,
    "shr":    OneArg,
    "adc":    TwoArg,
    "add":    TwoArg,
    "and":    TwoArg,
    "cmp":    TwoArg,
    "or":     TwoArg,
    "sub":    TwoArg,
    "sbb":    TwoArg,
    "swp":    TwoArg,
    "xor":    TwoArg,
    "call":   Jump,
    "jc":     Jump,
    "jnz":    Jump,
    "jmp":    Jump,
    "db":     Bytes,
    "dw":     Bytes,
    "data":   Bytes,
    "asciz":  String,
    "str":    String,
}


reserved = {
    "a", "b", "c", "d", "ab", "cd", "si", "di", "sp", "pc", "flags"
}


class Directive(Entry):
    def __init__(self, *tokens):
        super(Directive, self).__init__(*tokens)
        self.prefix = "\n"


class Label(Directive):
    def __init__(self, label, value):
        super(Label, self).__init__("label:", label, value)
        if label in reserved:
            raise ParseException(f"Cannot use reserved value '{label}' as label")
        self.label = label
        self.value = value

    def __str__(self):
        return f'{self.label}:'


class Define(Label):
    def __init__(self, label, value):
        super(Define, self).__init__(label, value)
        self.prefix = None

    def __str__(self):
        return f'{self.label} = 0x{self.value:04x}'


class Include(Directive):
    def __init__(self, fname):
        super(Include, self).__init__("include:", fname)
        self.fname = fname

    def __str__(self):
        return f'.include {self.fname}'


class Segment(Directive):
    def __init__(self, start_address):
        super(Segment, self).__init__(".segment", start_address)
        self._start_address = start_address
        self._size = 0
        self._entries = []

    def size(self):
        return self._size

    def __str__(self):
        return f".segment 0x{self._start_address:04x}"

    def add(self, entry):
        self._entries.append(entry)
        self._size += entry.bytes

    def current_address(self):
        return self._start_address + self._size

    def append_to(self, image):
        addr = self._start_address
        image.list(addr, self)
        image.set_address(addr)
        for e in self._entries:
            image.list(addr, e)
            addr += e.bytes
            e.append_to(image)
            self._errors.extend(e.errors())


class Image:
    def __init__(self, size):
        self._size = size
        self._segments = [Segment(0)]
        self._current = self._segments[0]
        self._labels = {}
        self._start_address = -1
        self._address = 0
        self._image = bytearray()
        self._errors = []
        self._list = False
        self._list_address = False

    def add(self, entry):
        self._current.add(entry)
        if isinstance(entry, Label):
            self._labels[entry.label] = entry
        # print(entry)

    def has_label(self, name):
        return name in self._labels

    def label(self, name):
        return self._labels.get(name, None)

    def new_segment(self, addr):
        if self._current.size() == 0:
            self._segments.pop()
        segment = Segment(addr)
        self._segments.append(segment)
        self._current = segment
        print(segment)
        return self

    def current_address(self):
        return self._current.current_address()

    def errors(self):
        return self._errors

    def assemble(self, list=False, address=False):
        self._list = list
        self._list_address = address
        for s in self._segments:
            s.append_to(self)
            self._errors.extend(s.errors())

    def list(self, address, entry):
        if self._list:
            addr = f'{address:04x}' if self._list_address else ''
            prefix = f'{addr}\t' if entry.prefix is None else entry.prefix
            print(f'{prefix}{entry}')

    def set_address(self, address):
        if address < self._address:
            raise ParseException(f"Overlapping segments at address {address}")
        if self._start_address == -1:
            self._start_address = address
        while self._address < address:
            self.append(0)

    def append(self, *data):
        for d in data:
            if d is None:
                continue
            elif isinstance(d, str):
                self.append(bytes(d, "utf-8"))
            elif isinstance(d, (bytes, bytearray)):
                self._image.extend(d)
                self._address += len(d)
            elif isinstance(d, (list, tuple)):
                self.append(*d)
            elif isinstance(d, int):
                if -128 <= d < 256:
                    self._image.append(d)
                    self._address += 1
                else:
                    self._errors.append(f"Byte value {d} out of range")
            else:
                self._errors.append(f"Cannot append {d} (type {type(d)} to segment")

    def write(self, fname):
        if self._errors:
            raise ParseException("Cannot write assembled output due to assembly errors")
        with open(fname, mode="wb") as fh:
            fh.write(self._image)

    def dump(self):
        print("\nBinary dump\n")
        print(bytes_to_str(self._image, address=self._start_address, radix=False))


class AssemblyParser:
    def __init__(self, fname, outfile=None, image=None):
        self._fname = fname
        if outfile is None:
            n, _ = os.path.splitext(fname[0] if isinstance(fname, (list, tuple)) else fname)
            self._out = n + ".bin"
        else:
            self._out = outfile
        self._errors = []
        self._image = Image(32*1024) if image is None else image

    def get_opcode(self, mnemonic, *args):
        lwr = mnemonic.lower()
        opcode = mnemonics[lwr](lwr, *args) if lwr in mnemonics else None
        if not opcode:
            self._errors.append(f'Mnemonic {mnemonic} not defined')
        else:
            self._errors.extend(opcode.errors())
        return opcode

    def directive(self, directiv, *args):
        if directiv == "segment":
            if args:
                addr_str = args[0]
                try:
                    if addr_str[0] == '$':
                        addr_str = "0x" + addr_str[1:]
                    addr = int(addr_str, 0)
                    self._image.new_segment(addr)
                except ValueError:
                    self._errors.append(f"Bad address {addr_str}")
            else:
                self._errors.append(".segment directive must specify start address")
        elif directiv == "define":
            if args and len(args) == 2:
                value_str = args[1]
                try:
                    if value_str[0] == '$':
                        value_str = "0x" + value_str[1:]
                    value = int(value_str, 0)
                    define = Define(args[0], value)
                    self._image.add(define)
                except ValueError:
                    self._errors.append(f"Bad define value {value_str}")
            else:
                self._errors.append(".define directive must specify name and value")
        elif directiv == "include":
            if args:
                include = Include(args[0])
                self._image.add(include)
                self.parse(args[0])
            else:
                self._errors.append(".include directive must specify file name")
        else:
            self._errors.append(f"Invalid directive {directiv}")

    def opcode(self, mnemonic, *args):
        try:
            opcode = self.get_opcode(mnemonic, *args)
            if opcode is None:
                self._errors.append(f"Unknown opcode mnemonic {mnemonic}")
            else:
                self._image.add(opcode)
        except ParseException as pe:
            self._errors.append(str(pe))
        except ValueError as ve:
            self._errors.append(str(ve))

    def handle_line(self, tokens):
        if not tokens:
            return
        if tokens[0].endswith(":"):
            label = Label(tokens[0][:-1], self._image.current_address())
            self._image.add(label)
            self.handle_line(tokens[1:])
        elif tokens[0].startswith("."):
            self.directive(tokens[0][1:], *tokens[1:])
        else:
            self.opcode(tokens[0], *tokens[1:])

    def parse(self, fname=None):
        if fname is None:
            fname = self._fname
        if isinstance(fname, (list, tuple)):
            for n in fname:
                self._parse(n)
        else:
            self._parse(fname)

    def _parse(self, fname):
        with open(fname) as fh:
            try:
                for line in fh:
                    tokens = split(line)
                    if tokens:
                        self.handle_line(tokens)
            except ParseException as pe:
                self._errors.append(str(pe))
            except IOError as ioe:
                self._errors.append(str(ioe))

    def assemble(self, write=True, list=False, address=False):
        if self._errors:
            raise ParseException("Cannot assemble binary due to parsing errors")
        self._image.assemble(list=list, address=address)
        self._errors.extend(self._image.errors())
        self.print_errors()
        if write:
            self._image.write(self._out)
            self._errors.extend(self._image.errors())

    def dump(self):
        self._image.dump()

    def errors(self):
        return self._errors

    def print_errors(self):
        if self._errors:
            print("\n".join(self._errors))
            sys.exit(1)


if __name__ == "__main__":
    argparser = argparse.ArgumentParser(description='JV Assembler')
    argparser.add_argument('files', nargs='+', type=str, help="The file to assemble")
    argparser.add_argument('--objdump', dest='dump', action='store_true', help="Dump the assembled binary data")
    argparser.add_argument('--list', dest='list', action='store_true', help="List the parsed input")
    argparser.add_argument('--address', dest='address', action='store_true', help="Include addresses in --list output")
    argparser.add_argument('--out', '-o', dest='out', type=str, default=None,
               help="Name of the output file. By default the name of the first input file with the extension '.bin")
    argparser.add_argument('--noout', dest='noout', action='store_true',
                           help='Do not write the binary assembled output')
    args = argparser.parse_args()

    parser = AssemblyParser(args.files, args.out)
    parser.parse()
    parser.print_errors()
    parser.assemble(write=not args.noout, list=args.list, address=args.address)
    parser.print_errors()
    if args.dump:
        parser.dump()

