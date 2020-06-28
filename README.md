# A Simple Ben Eater Inspired 8-bit computer

Please describe me.

Register IDs
===

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

Control Lines
===

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

Operation Codes
===

XADDR: 
	If the source (Get) register is a 16 bit register:
	OP2: If Low register will be incremented/decremented on clock rising edge	
        OP0-1:
		01: Increment source (Get) register
		10: Decrement source (Get) register

XDATA:	
	Target register 8-bit or 16-bit data registers:
	====
	OP0-OP2: X
	OP3: If either target or destination is a 16 bit register, 0 indicates the 
	data should be transfered to/from the LSB of the register, and conversely 1
	indicates a data transfer to/from the MSB. If both source and targets are 8 
	bits, this operation code, like OP0-OP2, has no effect.

	Target register ALU Operation (0x5):
	====
	OP0-OP3:
	0x0=ADD
	0x1=ADC
	0x2=SUB
	0x3=SUBC

	0x8=AND
	0x9=OR
	0xA=XOR
	0xB=NOT (LHS)
	0xC=SHL
	0xD=SHR



