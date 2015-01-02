# Key
n stands for an 8-bit immediate value.
nn stands for a 16-bit immediate value (remember that the Gameboy CPU is little-endian).

A, B, C, D, E, H, and L are 8-Bit registers
BC, DE, HL, and SP are 16-bit registers. BC, DE, and HL are comprised of two of the 8-bit registers as they are listed. Keep in mind that the most significant byte is stored in the first part and the less significant byte is stored in the second. So setting BC <= 0xEEFF is equivalent to setting B <= 0xEE and C <= 0xFF. SP is not split up like this. SP is where the stack pointer is held.

If a register is wrapped in parentheses that means to apply the operation to the address held in that register. If this is done with a 16-bit register the result is as you would expect. With an 8-bit register this applies to the address in the zero page, which is at the top of the address space.

So if C <= 0x10 and A <= 0x13, then LDH (C),A places 0x13 in memory at the address 0xFF10.

Not evey instruction can be used indirectly like this, and some instructions only allow this indirect use with a subset of the registers you woud otherwise think they would be able to use.

Durations are expressed in cycles on the main CPU clock. All instructions on a gameboy are done in multiples of four of these, but in order to eliminate any confusion they are listed as their complete clock values.

The program counter is referred to as PC, but there is no way to actually use it in an instruction, and the only way to discover it is to CALL an instruction and read the value written to the stack. In the descriptions for the instructions it is assumed the PC is pointing to the _next_ instruction while carrying it out. This means that (SP) <= PC effectively describes setting a return address in a CALL instruction.

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
- Length: 2

### LD X,Y
- Valid registers for X or Y: B, C, D, E, H, L, (HL), A with the exception that X and Y cannot both be (HL)
- Description: X <= Y
- Encoding: 01xx xyyy
- Duration: 4 unless one of the arguments is (HL), in that case 8.
- Length: 1

### LD (XX),A
- Valid registers for XX: BC, DE, HL+, and HL-
- Description: (XX) <= A
- Encoding: 00xx 0010
- Duration: 8
- Length: 1

### LD A,(XX)
- Valid registers for XX: BC, DE, HL+, and HL-
- Description: A <= (XX)
- Encoding: 00xx 1010
- Duration: 8
- Length: 1

### LD (nn),A
- Description: (nn) <= A
- Signature: 1110 1010 (0xEA)
- Duration: 16
- Length: 3

### LD A,(nn)
- Description: A <= (nn)
- Signature: 1111 1010 (0xFA)
- Duration: 16
- Length: 3

### LDH (n),A
- Description: (n + 0xFF00) <= A
- Encoding: 1110 0000 (0xE0)
- Duration: 12
- Length: 2

### LDH A,(n)
- Description: A <= (n + 0xFF00)
- Encoding: 1111 0000 (0xF0)
- Duration: 12
- Length: 2

### LDH (C),A
- Description: A <= (C + 0xFF00)
- Encoding: 1110 0010 (0xE2)
- Duration: 8
- Length: 1

### LDH A,(C)
- Description: (C + 0xFF00) <= A
- Encoding: 1111 0010 (0xF2)
- Duration: 8
- Length: 1

## 16-Bit Loads

### LD XX,nn
- Valid registers for XX: BC, DE, HL, SP
- Description: XX <= nn
- Encoding: 00xx 0001
- Duration: 12
- Length: 3

### PUSH XX
- Valid registers for XX: BC, DE, HL, AF
- Description: SP <= SP - 2; (SP) <= XX
- Encoding:  11xx 0101
- Duration: 16
- Length: 1

### POP XX
- Valid registers for XX: BC, DE, HL, AF
- Description: XX <= (SP); SP <= SP + 2
- Encoding: 11xx 0001
- Flags affected: Z, N, H, C (they are set from the value popped into the F register only for POP AF)
- Duration: 16
- Length: 1

### LD (nn),SP
- Description: (nn) <= SP
- Encoding: 0000 1000 (0x08)
- Duration: 20
- Length: 3

### LD HL,SP+n
- Description: HL <= SP + n
- Encoding: 1111 1000 (0xF8)
- Duration: 12
- Length: 2

### LD SP,HL
- Description: SP <= HL
- Encoding: 1111 1001 (0xF9)
- Duration: 8
- Length: 1

## 8-Bit Arithmetic and Logical Operations

Decoding note: most of the instructions that have the option of including a carry or not have a one bit difference between them, so they could be interpreted similarly.

### ADD A,X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: A <= A + X
- Encoding: 1000 0xxx
- Duration: 4 unless X is (HL), in that case 8
- Flags affected:
  - Z <= A == 0
  - N <= 0
  - H <= set if carry from bit 3
  - C <= set if carry from bit 7
- Length: 1

### ADD A,n
- Description: A <= A + n
- Encoding: 1100 0110 (0xC6)
- Duration: 8
- Flags affected:
  - Z <= A == 0
  - N <= 0
  - H <= set if carry from bit 3
  - C <= set if carry from bit 7
- Length: 2

### ADC A,X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: A <= X + fC
- Encoding: 1000 1xxx
- Duration: 4 unless X is (HL), in that case 8
- Flags affected:
  - Z <= A == 0
  - N <= 0
  - H <= set if carry from bit 3
  - C <= set if carry from bit 7
- Length: 1

### ADC A,n
- Description: A <= n + fC
- Encoding:  1100 1110 (0xCE)
- Duration: 8
- Flags affected:
  - Z <= A == 0
  - N <= 0
  - H <= set if carry from bit 3
  - C <= set if carry from bit 7
- Length: 2

### SUB X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: A <= A - X
- Encoding: 1001 0xxx
- Duration: 4 unless X is (HL), in that case 8
- Flags affected:
  - Z <= A == 0
  - N <= 1
  - H <= set if no borrow from bit 4
  - C <= set if no borrow
- Length: 1

### SUB n
- Description: A <= A - n
- Encoding:  1101 0110 (0xD6)
- Duration: 8
- Flags affected:
  - Z <= A == 0
  - N <= 1
  - H <= set if no borrow from bit 4
  - C <= set if no borrow
- Length: 2

### SBC X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: A <= A - X - fC
- Encoding: 1001 1xxx
- Duration: 4 unless X is (HL), in that case 8
- Flags affected:
  - Z <= A == 0
  - N <= 1
  - H <= set if no borrow from bit 4
  - C <= set if no borrow
- Length: 1

### SBC n
- Description: A <= A - n - fC
- Encoding 1101 1110 (0xDE)
- Duration: 8
- Flags affected:
  - Z <= A == 0
  - N <= 1
  - H <= set if no borrow from bit 4
  - C <= set if no borrow
- Length: 2

### AND X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: A <= A & X
- Encoding: 1010 1xxx
- Duration: 4 unless X is (HL), in that case 8
- Flags affected:
  - Z <= A == 0
  - N <= 0
  - H <= 1
  - C <= 0
- Length: 1

### AND n
- Description: A <= A & n
- Encoding: 1110 0110 (0xE6)
- Duration: 8
- Flags affected:
  - Z <= A == 0
  - N <= 0
  - H <= 1
  - C <= 0
- Length: 2

### OR X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: A <= A | X
- Encoding: 1011 0xxx
- Duration: 4 unless X is (HL), in that case 8
- Flags affected:
  - Z <= A == 0
  - N <= 0
  - H <= 0
  - C <= 0
- Length: 1

### OR n
- Description: A <= A | n
- Encoding: 1111 0110 (0xF6)
- Duration: 8
- Flags affected:
  - Z <= A == 0
  - N <= 0
  - H <= 0
  - C <= 0
- Length: 2

### XOR X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: A <= A ^ X
- Encoding: 1010 1xxx
- Duration: 4 unless X is (HL), in that case 8
- Flags affected:
  - Z <= A == 0
  - N <= 0
  - H <= 0
  - C <= 0
- Length: 1

### XOR n
- Description: A <= A ^ n
- Encoding: 1110 1110 (0xEE)
- Duration: 8
- Flags affected:
  - Z <= A == 0
  - N <= 0
  - H <= 0
  - C <= 0
- Length: 2

### CP X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: A - X (set the flags for this result, but throw the result away)
- Encoding: 1011 1xxx
- Duration: 4 unless X is (HL), in that case 8
- Flags affected:
  - Z <= result == 0 (A == N)
  - N <= 1
  - H <= set if no borrow from bit 4
  - C <= set if no borrow (A < N)
- Length: 1

### CP n
- Description: A - n (set the flags for this result, but throw the result away)
- Encoding: 1111 1110 (0xFE)
- Duration: 8
- Flags affected: 
  - Z <= result == 0 (A == N)
  - N <= 1
  - H <= set if no borrow from bit 4
  - C <= set if no borrow (A < N)
- Length: 2

### INC X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: X <= X + 1
- Encoding: 00xx x100
- Duration: 4 unless X is (HL), in that case 12
- Flags affected:
  - Z <= X == 0
  - N <= 0
  - H <= set if carry from bit 3
  - C <= C (not affected)
- Length: 1

### DEC X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: X <= X - 1
- Encoding: 00xx x101
- Duration: 4 unless X is (HL), in that case 12
- Flags affected:
  - Z <= X == 0
  - N <= 1
  - H <= set if no borrow from bit 4
  - C <= C (not affected)
- Length: 1

### DAA
- Description: A <= convert_to_bcd(A)
  - For example, if A is 14 (decimal) it would be set to 0x14
- Encoding: 0010 0111 (0x27)
- Duration: 4
- Flags affected: 
  - Z <= A == 0
  - N <= N (not affected)
  - H <= 0
  - C <= set if the number converted from is greater than 99
- Length: 1

### CPL
- Description: A <= ~A
- Encoding: 0010 1111 (0x2F)
- Duration: 4
- Flags affected: 
  - Z <= Z (not affected)
  - N <= 1
  - H <= 1
  - C <= C (not affected)
- Length: 1

## 16-Bit Arithmetic and Logical Operations

### INC XX
- Valid registers for XX: BC, DE, HL, SP
- Description: XX <= XX + 1
- Encoding: 00xx 0011
- Duration: 8
- Flags affected: no flags affected
- Length: 1

### DEC XX
- Valid registers for XX: BC, DE, HL, SP
- Description: XX <= XX - 1
- Encoding: 00xx 1011
- Duration: 8
- Flags affected: no flags affected
- Length: 1

### ADD HL,XX
- Valid registers for XX: BC, DE, HL, SP
- Description: HL <= HL + XX
- Encoding: 00xx 1001
- Duration: 8
- Flags affected:
  - Z <= Z (not affected)
  - N <= 0
  - H <= set if carry from bit 11
  - C <= set if carry from bit 15
- Length: 1

### ADD SP,n
- Description: SP <= SP + n
- Encoding: 1110 1000 (0xE8)
- Duration: 16
- Flags affected:
  - Z <= 0
  - N <= 0
  - H <= set if carry from bit 11
  - C <= set if carry from bit 15
- Note: the n is a one byte immediate value, _not_ a two byte value.
- Length: 2

## Misc Arithmetic Operations

### SCF
- Description: set carry flag
- Encoding: 0011 0111 (0x37)
- Duration: 4
- Flags affected: 
  - Z <= Z (not affected)
  - N <= 0
  - H <= 0
  - C <= 1
- Length: 1

### CCF
- Description: complement carry flag
- Encoding: 0011 1111 (0x3F)
- Duration: 4
- Flags affected: 
  - Z <= Z (not affected)
  - N <= 0
  - H <= 0
  - C <= ~C
- Length: 1

## Program Flow

### JP nn
- Description: PC <= nn
- Encoding: 1100 0011 (0xC3)
- Duration: 16
- Length: 3

### JP CC,nn
- Valid conditions for CC: NZ, Z, NC, C
- Description: if CC then PC <= nn
- Encoding: 110c c011
- Duration: if CC then 16 else 12
- Length: 3

### JP (HL)
- Description: PC <= (HL)
- Encoding: 1110 1001 (0xE9)
- Duration: 4
- Length: 1

### JR n
- Description: PC <= PC + sign_extend_8bit_to_16bit(n)
- Encoding: 0001 1000 (0x18)
- Duration: 12
- Length: 2

### JR CC,n
- Valid conditions for CC: NZ, Z, NC, C
- Description: if CC then PC <= PC + sign_extend_8bit_to_16_bit(n)
- Encoding: 001c c000
- Duration: if CC then 12 else 8
- Length: 2

### CALL nn
- Description: SP <= SP - 2; (SP) <= PC; PC <= nn
- Encoding: 1100 1101 (0xCD)
- Duration: 24
- Length: 3

### CALL CC,nn
- Valid conditions for CC: NZ, Z, NC, C
- Description: if CC then SP <= SP - 2; (SP) <= PC; PC <= nn
- Encoding: 110c c100
- Duration: if CC then 24 else 12
- Length: 3

### RET
- Description: PC <= (SP); SP <= SP + 2
- Encoding: 1100 1001 (0xC9)
- Duration: 16
- Length: 1

### RET CC
- Valid conditions for CC: NZ, Z, NC, C
- Description: if CC then PC <= (SP); SP <= SP + 2
- Encoding: 110c c000
- Duration: if CC then 24 else 8
- Length: 1

### RETI
- Description: PC <= (SP); SP <= SP + 2; enable_interrupts()
- Encoding: 1100 1001 (0xC9)
- Duration: 16
- Length: 1

### RST d
- Valid numbers for d: $00, $08, $10, $18, $20, $28, $30, $38. These are encoded in the instruction as follows:
  - 000: $00
  - 001: $08
  - 010: $10
  - 011: $18
  - 100: $20
  - 101: $28
  - 110: $30
  - 111: $38
- Description: SP <= SP - 2; (SP) <= PC; PC <= 0x0000 + d
- Encoding: 11dd d111
- Duration: 16
- Note: The encoded value is effectively multiplied by 8. The following table shows the result of each encoded value:
  - 000: PC <= 0x0000
  - 001: PC <= 0x0008
  - 010: PC <= 0x0010
  - 011: PC <= 0x0018
  - 100: PC <= 0x0020
  - 101: PC <= 0x0028
  - 110: PC <= 0x0030
  - 111: PC <= 0x0038
- Length: 1

## Rotates and Shifts

### RLCA
- Description: rotate_left(A)
- Encoding: 0000 0111 (0x07)
- Duration: 4
- Flags affected:
  - Z <= 0
  - N <= 0
  - H <= 0
  - C <= most_significant_bit(A) (before the rotate)
- Note: this effectively rotates A left and places whatever value gets rotated from bit 7 to 0 into the carry flag as well.
- Length: 1
```
      +-----------+
fC <--+--A[7:0]<--+
```

### RRCA
- Description: rotate_right(A)
- Encoding: 0000 1111 (0x0F)
- Duration: 4
- Flags affected:
  - Z <= 0
  - N <= 0
  - H <= 0
  - C <= least_significant_bit(A) (before the rotate)
- Note: this effectively rotates A right and places whatever value gets rotated from bit 0 to 7 into the carry flag as well.
- Length: 1
```
+-----------+
+-->A[7:0]--+--> fC
```

### RLA
- Description: rotate_left_through_carry(A)
- Encoding: 0001 0111 (0x17)
- Duration: 4
- Flags affected:
  - Z <= 0
  - N <= 0
  - H <= 0
  - C <= most_significant_bit(A) (before the rotate)
- Note: this effectively rotates A left through the carry flag. This ends up placing bit 7 into the carry flag and the carry flag into bit 0.
- Length: 1
```
+----------------+
+--fC<--A[7:0]<--+
```

### RRA
- Description: rotate_right_through_carry(A)
- Encoding: 0001 1111 (0x1F)
- Duration: 4
- Flags affected:
  - Z <= 0
  - N <= 0
  - H <= 0
  - C <= least_significant_bit(A) (before the rotate)
- Note: this effectively rotates A right through the carry flag. This ends up placing bit 0 into the carry flag and the carry flag into bit 7.
- Length: 1
```
+----------------+
+-->A[7:0]-->fC--+
```

### RLC X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: rotate_left(X)
- Encoding: 0xCB 0000 0xxx
- Duration: 8 unless X is (HL), in that case 16
- Flags affected:
  - Z <= X == 0
  - N <= 0
  - H <= 0
  - C <= most_significant_bit(X) (before the rotate)
- Note: this effectively rotates X left and places whatever value gets rotated from bit 7 to 0 into the carry flag as well.
- Length: 2
```
      +-----------+
fC <--+--X[7:0]<--+
```

### RRC X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: rotate_right(X)
- Encoding: 0xCB 0000 1xxx
- Duration: 8 unless X is (HL), in that case 16
- Flags affected:
  - Z <= X == 0
  - N <= 0
  - H <= 0
  - C <= least_significant_bit(X) (before the rotate)
- Note: this effectively rotates X right and places whatever value gets rotated from bit 0 to 7 into the carry flag as well.
- Length: 2
```
+-----------+
+-->X[7:0]--+--> fC
```

### RL X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: rotate_left_through_carry(X)
- Encoding: 0xCB 0001 0xxx (0x17)
- Duration: 8 unless X is (HL), in that case 16
- Flags affected:
  - Z <= X == 0
  - N <= 0
  - H <= 0
  - C <= most_significant_bit(X) (before the rotate)
- Note: this effectively rotates X left through the carry flag. This ends up placing bit 7 into the carry flag and the carry flag into bit 0.
- Length: 2
```
+----------------+
+--fC<--X[7:0]<--+
```

### RR X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: rotate_right_through_carry(X)
- Encoding: 0xCB 0001 1xxx
- Duration: 8 unless X is (HL), in that case 16
- Flags affected:
  - Z <= X == 0
  - N <= 0
  - H <= 0
  - C <= least_significant_bit(X) (before the rotate)
- Note: this effectively rotates X right through the carry flag. This ends up placing bit 0 into the carry flag and the carry flag into bit 7.
- Length: 2
```
+----------------+
+-->X[7:0]-->fC--+
```

### SLA X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: X <= X << 1
- Encoding:  0xCB 0010 0xxx
- Duration: 8 unless X is (HL), in that case 16
- Flags affected:
  - Z <= X == 0
  - N <= 0
  - H <= 0
  - C <= most_significant_bit(X) (before rotate)
- Length: 2
```
fC<--X[7:0]<--0
```

### SRA X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: X <= X >> 1 (the shift is arithmetic, though, so the sign bit sticks)
- Encoding:  0xCB 0010 1xxx
- Duration: 8 unless X is (HL), in that case 16
- Flags affected:
  - Z <= X == 0
  - N <= 0
  - H <= 0
  - C <= least_significant_bit(X) (before rotate)
- Length: 2
```
+---------+
+-->X[7]--+-->X[6:0]-->fC
```

### SRL X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: X <= X >> 1 (the shift is logical, so zeroes come in on the left)
- Encoding:  0xCB 0011 1xxx
- Duration: 8 unless X is (HL), in that case 16
- Flags affected:
  - Z <= X == 0
  - N <= 0
  - H <= 0
  - C <= least_significant_bit(X) (before rotate)
- Length: 2
```
0-->X[7:0]-->fC
```

## Bit Operations

### SWAP X
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: X <= swap_nibbles(X)
  - This means something like 0xEF would turn into 0xFE
- Encoding: 0xCB 0011 0xxx
- Duration: 8 unless X is (HL), in that case 16
- Flags affected:
  - Z <= X == 0
  - N <= 0
  - H <= 0
  - C <= 0
- Length: 2

### BIT b,X
- Valid values for b: 0, 1, 2, 3, 4, 5, 6, 7
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: only affects flags
- Duration: 8 unless X is (HL), in that case 16
- Flags affected:
  - Z <= X & \[1 << b\] == 0 (set if the bit checked is zero)
  - N <= 0
  - H <= 1
  - C <= C (not affected)
- Length: 2

### SET b,X
- Valid values for b: 0, 1, 2, 3, 4, 5, 6, 7
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: X <= X | \[1 << b\] (set bit b in X)
- Duration: 8 unless X is (HL), in that case 16
- Flags affected: none
- Length: 2

### RES b,X
- Valid values for b: 0, 1, 2, 3, 4, 5, 6, 7
- Valid registers for X: B, C, D, E, H, L, (HL), A
- Description: X <= X & ~\[1 << b\] (reset bit b in X)
- Duration: 8 unless X is (HL), in that case 16
- Flags affected: none
- Length: 2

## Misc

### NOP
- Description: no operation
- Encoding: 0000 0000 (0x00)
- Duration: 4
- Length: 1

### STOP
- Description: halt the CPU and LCD display until a button is pressed
- Encoding: 0001 0000 (0x10)
- Duration: 4
- Length: 1

### HALT
- Description: power down the CPU until an interrupt occurs
- Encoding: 0111 0110 (0x76)
- Duration: 4
- Length: 1

### DI
- Description: disable_interrupts()
- Encoding: 1111 0011 (0xF3)
- Duration: 4
- Length: 1

### EI
- Description: enable_interrupts()
- Encoding: 1111 1011 (0xFB)
- Duration: 4
- Length: 1
