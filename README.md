# A Simple Ben Eater Inspired 8-bit computer

Please describe me.

### Register IDs

ID       | Register       |  R W  | Remarks
---------|----------------|-------|--------------
0000 00  | A              |  x x  |
0001 01  | B              |  x x  |
0010 02  | C              |  x x  |
0011 03  | D              |  x x  |
0100 04  | ALU LHS/Result |  x x  | Put: LHS, Get: Result
0101 05  | ALU Operation  |  - x  | Put: RHS, OP0-3: Operation
0110 06  | Instruction    |  - x  |
0111 07  | Mem            |  x x  |
1000 08  | Prog Counter   |  x x  |
1001 09  | SP             |  x x  |
1010 10  | Si             |  x x  |
1011 11  | Di             |  x x  |
1100 12  | TX             |       |
1101 13  |                |       |
1110 14	 | Monitor        |  x -  |
1111 15  | Mem Addr       |  - x  |

### Control Lines


Pin   | Name                   | Description
------|------------------------|-----------------------
1     | `GND`                  | Common Ground
2     | `VCC`                  | +5V Power
3     | `CLK`                  | Square Wave Clock
4     | ~~`CLK`~~              | Reverse Clock
5     | `^CLK`                 | "Burst" Clock at rising edge of CLK
6     | `HLT`                  | Halts clock
7     | ~~`SUS`~~              | Requests Control Unit to cease dispatching instructions.
8     | ~~`XDATA`~~            | 8 bit Data transfer instruction
9     | ~~`XADDR`~~            | 16 bit address transfer instruction
10    | ~~`SACK`~~             | Set by Control Unit as acknowlegement of SUS.
11-14 | `OP0`-`OP3`            | Custom Operation codes used by various instructions. See below.
15    | `RST`                  | Reset
16    | ~~`IO`~~               | In/Out instruction
17-20 | `PUT0`-`PUT3`          | Register ID of the target of a transfer instruction.
21-24 | `GET0`-`GET3`          | Register ID of the source of a transfer instruction.
25-32 | `D0`-`D7`              | Databus
33-40 | `A0`-`A7` / `A8`-`A15` | Address bus

### Operation Codes

#### XADDR

OP0-3 |Function
---|---
`XX01`|Increment source (Get) register
`XX10`|Decrement source (Get) register


#### XDATA

_When moving data between 8- and 16-bit registers:_

OP0-3 |Function
------|--------
`XXX0`|Transfer to/from the LSB of the 16-bit register
`XXX1`|Transfer to/from the MSB of the 16-bit register

_When the target register is ALU Operation (`0x5`):_

OP0-3|Operation
-----|-----------
`0x0`|`ADD`
`0x1`|`ADC`
`0x2`|`SUB`
`0x3`|`SBB`
...|...
`0x8`|`AND`
`0x9`|`OR`
`0xA`|`XOR`
`0xB`|`NOT LHS`
`0xC`|`SHL`
`0xD`|`SHR`
`0xE`|`CLR`
...|...


## N O T E S

### SBB (Subtract with Borrow)

See here: https://en.wikipedia.org/wiki/Carry_flag#Vs._borrow_flag

We use the convention that when subtracting the processor Carry flag is a borrow flag. This is in line with the Z80 and x68.

### 74xxx253 Function Table

As determined by Julian Ilett in https://www.youtube.com/watch?v=15M63Zqkthk

C3-C0 :arrow_down: BA :arrow_right: | 11 | 10 | 01 | 00 | Operation
------|----|----|----|----|-----------
  0x0  | 0  | 0  | 0  | 0  | All Zero
  0x1  | 0  | 0  | 0  | 1  | A NOR B
  0x2  | 0  | 0  | 1  | 0  | A AND (NOT B)
  0x3  | 0  | 0  | 1  | 1  | NOT B
  0x4  | 0  | 1  | 0  | 0  | (NOT A) AND B
  0x5  | 0  | 1  | 0  | 1  | NOT A
  0x6  | 0  | 1  | 1  | 0  | A XOR B
  0x7  | 0  | 1  | 1  | 1  | A NAND B
  0x8  | 1  | 0  | 0  | 0  | A AND B
  0x9  | 1  | 0  | 0  | 1  | A XNOR B
  0xA  | 1  | 0  | 1  | 0  | A
  0xB  | 1  | 0  | 1  | 1  | A OR (NOT B)
  0xC  | 1  | 1  | 0  | 0  | B
  0xD  | 1  | 1  | 0  | 1  | (NOT A) OR B
  0xE  | 1  | 1  | 1  | 0  | A OR B
  0xF  | 1  | 1  | 1  | 1  | All One
