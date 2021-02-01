#!/usr/bin/env python

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
            elif (len(s) - 2) % 8 == 0 and s[0:2].lower() == "0b":
                val = int(s[2:], 2)
            elif (len(s) - 2) % 2 == 0 and s[0:2].lower() == "0x":
                val = int(s[2:], 16)
            elif (len(s) - 1) % 8 == 0 and s[0].lower() == "b":
                val = int(s[1:], 2)
            elif (len(s) - 1) % 4 == 0 and s[0].lower() == "0":
                val = int(s[1:], 8)
            elif (len(s) - 1) % 2 == 0 and s[0].lower() == "x":
                val = int(s[1:], 16)
            elif s.issigit():
                val = int(s)
            else:
                raise ValueError(f"Invalid byte value {d}")
            ret.extend(to_bytes(0, val))
        elif isinstance(d, int):
            if -128 <= d < 256:
                ret.append(d)
            else:
                ret.append(d / 256)
                ret.extend(to_bytes(0, d % 256))
        else:
            raise ValueError(f"Cannot convert {d} to bytes")

    if 0 < count != len(ret):
        raise ValueError(f'Expected {count} bytes in {" ".join(data)}, got {len(ret)}')

    return ret


def split_test():
    print(split("jan de visser"))
    print(split("jan    de    visser"))
    print(split("jan  \t   de\t\tvisser"))
    print(split("jan  \t   'de    visser' 12 34"))
    print(split("jan  \t   'de    visser'"))
    print(split("jan  \t   'de    visser' "))
    print(split("'jan  '\t   de    visser "))
    print(split("  'jan  '\t   de    visser "))
    print(split("  'jan'\t   de    visser "))
    print(split("  'jan'\t   de    visser "))
    print(split("'jan'\t   de    visser "))


class Instruction:
    def __init__(self, mnemonic, *args):
        self.error = None
        self.opcode = 0
        self.mnemonic = mnemonic
        self.arguments = args


class ParseException(Exception):
    def __init__(self, msg):
        self.msg = msg

    def __str__(self):
        return self.msg


class Nop(Instruction):
    def __init__(self, mnemonic, *args):
        super(Nop, self).__init__(mnemonic, *args)
        if args:
            self.error = "NOP instruction takes no arguments"


# mov a,#xx      ; Copy xxxx into A
# mov a,b        ; Copy contents of B into A
# mov a,sh       ; Copy contents of high byte of Si into A
# mov a,sl       ; Copy contents of low byte of Si into A
# mov a,*si      ; Copy contents of memory location pointed to by Si into A

# mov sp,si      ; Copy contents of Si into SP
# mov sp,#xxxx   ; Copy xxxx into SP

target_registers_8bit = ('*####', 'a', 'b', 'c', 'd', '*si', 'sl', 'sh', '*di', 'dl', 'dh')
source_registers_8bit = ('##', '*####', 'a', 'b', 'c', 'd', '*si', 'sl', 'sh', '*di', 'dl', 'dh')

target_registers_16bit = ('si', 'di', 'sp')
source_registers_16bit = ('####', 'si', 'di', 'sp')

registers = ('#', '##', '*##', 'a', 'b', 'c', 'd', 'si', '*si', 'sl', 'sh', 'di', '*di', 'dl', 'dh', 'sp')

moves_8bit = ('#', '*##', 'a', 'b', 'c', 'd', 'sh', 'sl', '*si', 'dh', '*di', 'dl')
# (12-1) + (12-2) = 21   21*4 = 84    target: 11*4 = 44, 0x01..0x2C  source: 10*4 = 40, 0xD8..0xFF

moves_16bit = ('##', 'si', 'di', 'sp',)
# (4-1) + (4-2) = 5       5*3 = 15    target: 3*3 = 9,   0x2D..0x36  source: 2*3 = 6,   0xD2..0xD7


class Move(Instruction):

    def __init__(self, mnemonic, *args):
        super(Move, self).__init__(mnemonic, *args)
        self.indirect = False
        self.constant = None
        self.source = None
        self.target = None

    def parse_args(self, *args):
        parts = [s.strip() for s in "".join(args).split(",")]
        if len(parts) != 2:
            raise ParseException(f"Invalid {self.mnemonic} arguments '{' '.join(args)}'")
        if not parts[0]:
            raise ParseException(f'No destination specified in {self.mnemonic} {" ".join(args)}')
        elif parts[0][0] == '#':
            raise ParseException(f'Destination cannot be a constant in {self.mnemonic} {" ".join(args)}')
        if not parts[1]:
            raise ParseException(f'No source specified in {self.mnemonic} {" ".join(args)}')

        self.target = parts[0]
        if self.target in target_registers_8bit:
            pass
        elif self.target in target_registers_16bit:
            pass
        elif self.target[0] == '*':
            address_pointer = self.target[1:]
            if address_pointer.isidentifier():
                pass

            else:
                addres = to_bytes(4, address_pointer)




        self.source = parts[1]
        if self.source[0] == "#":
            self.constant = True
            self.source = self.source[1:]
            if self.source == "":
                raise ParseException(f'No constant value specified in {self.mnemonic} {" ".join(args)}')
            if self.source[0].isdigit():
                pass



opcodes = {
    "nop":  Nop,
    "mov":  Move,
    "jmp":  Jump,
    "jc":   Jump,
    "jz":   Jump,
    "pop":  Pop,
    "push": Push,
    "call": Call,
    "ret":  Return,
    "inc":  Inc,
    "dec":  Inc,
}


class Segment:
    def __init__(self, start_address):
        self._start_address = start_address
        self._size = 0
        self._image = []

    def append(self, *data):
        for d in data:
            if d is None:
                continue
            elif isinstance(d, str):
                self.append(bytes(d, "utf-8"))
            elif isinstance(d, (bytes, bytearray)):
                self._image.extend(d)
            elif isinstance(d, (list, tuple)):
                self.append(*d)
            elif isinstance(d, int):
                if -128 <= d < 256:
                    self._image.append(d)
                else:
                    errors.append(f"Byte value {d} out of range")
            else:
                errors.append(f"Cannot append {d} (type {type(d)} to segment")

    def current_address(self):
        return self._start_address + len(self._image)


class Image:
    def __init__(self, size):
        self._size = size
        self._segments = [Segment(0)]
        self._current = self._segments[0]

    def append(self, *data):
        self._current.append(*data)

    def new_segment(self, start_address):
        seg = Segment(start_address)
        self._segments.append(seg)
        self._current = seg
        return self

    def current_address(self):
        return self._current.current_address()


labels = {}
image = Image(32*1024)
errors = []


def get_opcode(mnemonic, *args):
    lwr = mnemonic.lower()
    return opcodes[lwr](lwr, *args) if lwr in opcodes else None


def directive(directiv, *args):
    if directiv == "segment":
        if args:
            addr_str = args[0]
            try:
                addr = uint(addr_str)
                image.new_segment(addr)
            except ValueError:
                errors.append(f"Bad address {addr_str}")
    elif directiv == "asciz" or directiv == "str":
        string = " ".join(t if t[0] not in '"\'' else t[1:-1] for t in args)
        image.append(string)
        if directiv == "asciz":
            image.append(0)
    else:
        errors.append(f"Invalid directive {directiv}")


def assembly(mnemonic, *args):
    try:
        bytes_ = to_bytes(1, *args) if mnemonic == "db" else \
            to_bytes(2, *args) if mnemonic == "dw" else \
            to_bytes(0, *args) if mnemonic == "data" else None
        if bytes_:
            image.append(bytes_)
        else:
            opcode = get_opcode(mnemonic, *args)
            if opcode is None:
                errors.append(f"Unknown opcode mnemonic {mnemonic}")
            elif opcode.error:
                errors.append(opcode.error)
            else:
                image.append(opcode.code)
                image.append(opcode.arguments)
    except ValueError as ve:
        errors.append(ve.str)


def handle_line(tokens):
    if not tokens:
        return
    if tokens[0].endswith(":"):
        labels[tokens[0][:-1]] = image.current_address()
        handle_line(tokens[1:])
    if tokens[0].startswith("."):
        directive(tokens[0], *tokens[1:])
    else:
        assembly(tokens[0], tokens[1:])


def parse(file_name: str):
    with open(file_name) as fh:
        for line in fh:
            tokens = split(line)
            if tokens:
                handle_line(tokens)
