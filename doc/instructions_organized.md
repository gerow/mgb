# Key
n stands for an 8-bit immediate value.
nn stands for a 16-bit immediate value (remember that the Gameboy CPU is little-endian).

A, B, C, D, E, H, and L are 8-Bit registers
BC, DE, HL, and SP are 16-bit registers. BC, DE, and HL are comprised of two of the 8-bit registers as they are listed. Keep in mind that the most significant byte is stored in the first part and the less significant byte is stored in the second. So setting BC <= 0xEEFF is equivalent to setting B <= 0xEE and C <= 0xFF. SP is not split up like this. SP is where the stack pointer is held.

If a register is wrapped in parentheses that means to apply the operation to the address held in that register. If this is done with a 16-bit register the result is as you would expect. With an 8-bit register this applies to the address in the zero page, which is at the top of the address space.

So if C <= 0x10 and A <= 0x13, then LDH (C),A places 0x13 in memory at the address 0xFF10.

Not evey instruction can be used indirectly like this, and some instructions only allow this indirect use with a subset of the registers you woud otherwise think they would be able to use.

Durations are expressed in cycles on the main CPU clock. All instructions on a gameboy are done in multiples of four of these, but in order to eliminate any confusion they are listed as their complete clock values.

The program counter is referred to as PC, but there is no way to actually use it in an instruction, and the only way to discover it is to CALL an instruction and read the value written to the stack.

The flags register is an 8 bit register encoded as follows:

```
76543210
ZNHC0000
```
- Z: Zero flag
- N: Subtract flag
- H: Half carry flag
- C: Carry flag
- 0: Not used. Should always be zero. I think that trying to pop nonzero bits into here might result in undefined behaviour...

In cases where the letters on their own might be confusing the flags may be referred to as f${LETTER} like fZ, fN, fH, or fC.

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

Push and pop instructions also allow the A register and the flags register (F) to used. These are encoded as follows:

- 00: BC
- 01: DE
- 10: HL
- 11: AF

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
- Duration: 8 unless X is (HL), in that case 12.

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

## 16-Bit Loads

### LD XX,nn
- Valid registers for XX: BC, DE, HL, SP
- Description: XX <= nn
- Encoding: 00xx 0001
- Duration: 12

### PUSH XX
- Valid registers for XX: BC, DE, HL, AF
- Description: SP <= SP - 2; (SP) <= XX
- Encoding:  11xx 0101
- Duration: 16

### POP XX
- Valid registers for XX: BC, DE, HL, AF
- Description: XX <= (SP); SP <= SP + 2
- Encoding: 11xx 0001
- Flags affected: Z, N, H, C (they are set from the value popped into the F register only for POP AF)
- Duration: 16

### LD (nn),SP
- Description: (nn) <= SP
- Encoding: 0000 1000 (0x08)
- Duration: 20

### LD HL,SP+n
- Description: HL <= SP + n
- Encoding: 1111 1000 (0xF8)
- Duration: 12

### LD SP,HL
- Description: SP <= HL
- Encoding: 1111 1001 (0xF9)
- Duration: 8

## 8-Bit Arithmetic and Logical Operations

Decoding note: most of the instructions that have the option of including a carry or not have a one bit difference between them, so they could be interpreted similarly.

### INC X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: X <= X + 1
- Encoding: 00xx x100
- Duration:
- Flags affected:

### DEC X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: X <= X - 1
- Encoding: 00xx x101
- Duration:
- Flags affected:

### ADD A,X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: A <= A + X
- Encoding: 1000 0xxx
- Duration:
- Flags affected:

### ADD A,n
- Description: A <= A + n
- Encoding: 1100 0110 (0xC6)
- Duration:
- Flags affected:

### ADC A,X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: A <= X + fC
- Encoding: 1000 1xxx
- Duration:
- Flags affected:

### ADC A,n
- Description: A <= n + fC
- Encoding:  1100 1110 (0xCE)
- Duration:
- Flags affected:

### SUB X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: A <= A - X
- Encoding: 1001 0xxx
- Duration:
- Flags affected:

### SUB n
- Description: A <= A - n
- Encoding:  1101 0110 (0xD6)
- Duration:
- Flags affected:

### SBC X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: A <= A - X - fC
- Encoding: 1001 1xxx
- Duration:
- Flags affected:

### AND X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: A <= A & X
- Encoding: 1010 1xxx
- Duration:
- Flags affected:

### AND n
- Description: A <= A & n
- Encoding: 1110 0110 (0xE6)
- Duration:
- Flags affected:

### OR X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: A <= A | X
- Encoding: 1011 0xxx
- Duration:
- Flags affected:

### OR n
- Description: A <= A | n
- Encoding: 1111 0110 (0xF6)
- Duration:
- Flags affected:

### XOR X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: A <= A ^ X
- Encoding: 1010 1xxx
- Duration:
- Flags affected:

### XOR n
- Description: A <= A ^ n
- Encoding: 1110 1110 (0xEE)
- Duration:
- Flags affected:

### CP X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: A - X (set the flags for this result, but throw the result away)
- Encoding: 1011 1xxx
- Duration:
- Flags affected:

### CP n
- Description: A - n (set the flags for this result, but throw the result away)
- Encoding: 1111 1110 (0xFE)
- Duration:
- Flags affected: 

### DAA
- Description: A <= convert_to_bcd(A)
  - For example, if A is 14 (decimal) it would be set to 0x14
- Encoding: 0010 0111 (0x27)
- Duration:
- Flags affected: 

### CPL
- Description: A <= ~A
- Encoding: 0010 1111 (0x2F)
- Duration:
- Flags affected: 

## 16-Bit Arithmetic and Logical Operations

## Misc Arithmetic Operations

### SCF
- Description: set carry flag
- Encoding: 0011 0111 (0x37)
- Duration:
- Flags affected: 

### CCF
- Description: clear carry flag
- Encoding: 0011 1111 (0x3F)
- Duration:
- Flags affected: 

## Program Flow

## Misc
