# Key
n stands for an 8-bit immediate value.
nn stands for a 16-bit immediate value (remember that the Gameboy CPU is little-endian).

A, B, C, D, E, H, and L are 8-Bit registers
BC, DE, HL, and SP are 16-bit registers. BC, DE, and HL are comprised of two of the 8-bit registers as they are listed. Keep in mind that the most significant byte is stored in the first part and the less significant byte is stored in the second. So setting BC <= 0xEEFF is equivalent to setting B <= 0xEE and C <= 0xFF.

If a register is wrapped in parentheses that means to apply the operation to the address held in that register. If this is done with a 16-bit register the result is as you would expect. With an 8-bit register this applies to the address in the zero page, which is at the top of the address space.

So if C <= 0x10 and A <= 0x13, then LDH (C),A places 0x13 in memory at the address 0xFF10.

Not evey instruction can be used indirectly like this, and some instructions only allow this indirect use with a subset of the registers you woud otherwise think they would be able to use.

Durations are expressed in cycles on the main CPU clock. All instructions on a gameboy are done in multiples of four of these, but in order to eliminate any confusion they are listed as their complete clock values.

# Target/Condition Encodings
The register targets and conditions are encoded into instructions as one, two, or three bit values. The various ways they are encoded are listed here.

## 8-Bit target encodings

These are represented as xxx or yyy in instruction entries.

- 000: B
- 001: C
- 010: D
- 011: E
- 100: H
- 101: L
- 110: (HL) (this is the address that the 16-bit register HL points to)
- 111: A

## 16-Bit target encodings

These are represented as xx in instruction entries.

- 00: BC
- 01: DE
- 10: HL
- 11: SP

Some instructions also replace HL and SP with HL+ and HL- which increment HL and decrement HL after the operation respectively. These are encoded as follows:

- 00: BC
- 01: DE
- 10: HL+
- 11: HL-

## Conditions

These are represented as cc in instruction listings.

- 00: NZ
- 01: Z
- 10: NC
- 11: C

# Instructions
## 8-Bit Loads

No 8-bit load instructions affect any flags.

### LD X,n
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: X <= n
- Encoding: 00xx x110
- Duration: 8

### LD X,Y
- Valid registers for X or Y: B, C, D, E, H, L, (HL), A with the exception that X and Y cannot both be (HL)
- Description: X <= Y
- Encoding: 01xx xyyy
- Duration: 4 unless one of the arguments is (HL), in that case 8.

### LD (XX),A
- Valid registers for XX: BC, DE, HL+, and HL-
- Description: (XX) <= A
- Encoding: 00xx 0010
- Duration: 8

### LD A,(XX)
- Valid registers for XX: BC, DE, HL+, and HL-
- Description: A <= (XX)
- Encoding: 00xx 1010
- Duration: 8

### LD (nn),A
- Description: (nn) <= A
- Signature: 1110 1010 (0xEA)
- Duration: 16

### LD A,(nn)
- Description: A <= (nn)
- Signature: 1111 1010 (0xFA)
- Duration: 16

### LDH (n),A
- Description: (n) <= A
- Encoding: 1110 0000 (0xE0)
- Duration: 12

### LDH A,(n)
- Description: A <= (n)
- Encoding: 1111 0000 (0xF0)
- Duration: 12

### LDH (C),A
- Description: A <= (C + 0xFF00)
- Encoding: 1110 0010 (0xE2)
- Duration: 8

### LDH A,(C)
- Description: (C + 0xFF00) <= A
- Encoding: 1111 0010 (0xF2)
- Duration: 8
