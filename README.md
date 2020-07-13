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
1100 12  |                |       |
1101 13  |                |       |
1110 14	 | Monitor        |  x x  |
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

OP | |
---|-|---
0-1|01|Increment source (Get) register
   |10|Decrement source (Get) register
2-3| X|X

#### XDATA
	
_When moving data between 8- and 16-bit registers:_
	
OP | |
---|-|---
0-2|X|X
 3 |0|Transfer to/from the LSB of the 16-bit register
 3 |1|Transfer to/from the MSB of the 16-bit register
 
_When the target register is ALU Operation (0x5):

OP0-3|Operation
-----|-----------
0x0|ADD
0x1|ADC
0x2|SUB
0x3|SUBC
...|...
0x8|AND
0x9|OR
0xA|XOR
0xB|NOT LHS
0xC|SHL
0xD|SHR
...|...


## N O T E S


### 74xxx253 Function Table

As determined by Julian Ilett in https://www.youtube.com/watch?v=15M63Zqkthk

C3-C0 :arrow_down: BA :arrow_right: | 11 | 10 | 01 | 00 | Operation
------|----|----|----|----|-----------
  00  | 0  | 0  | 0  | 0  | All Zero
  01  | 0  | 0  | 0  | 1  | NOR
  02  | 0  | 0  | 1  | 0  | A & !B
  03  | 0  | 0  | 1  | 1  | !B
  04  | 0  | 1  | 0  | 0  | !A & B
  05  | 0  | 1  | 0  | 1  | !A
  06  | 0  | 1  | 1  | 0  | XOR
  07  | 0  | 1  | 1  | 1  | NAND
  08  | 1  | 0  | 0  | 0  | AND
  09  | 1  | 0  | 0  | 1  | XNOR
  0A  | 1  | 0  | 1  | 0  | A
  0B  | 1  | 0  | 1  | 1  | A | !B
  0C  | 1  | 1  | 0  | 0  | B
  0D  | 1  | 1  | 0  | 1  | !A | B
  0E  | 1  | 1  | 1  | 0  | OR
  0F  | 1  | 1  | 1  | 1  | All One
