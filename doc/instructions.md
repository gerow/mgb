I put this together from the chart here <http://imrannazar.com/Gameboy-Z80-Opcode-Map> since I was more interested in decoding instructions. svbtle probably isn't the best place for this to live, but I'll figure that out better later.

Most of the naming is inconsistent and some things might be completely wrong. The only thing that is (mostly) consistent is that any instruction with a mnemonic followed by nn uses the two immediate bytes for its execution and any instruction with a mnemonic followed by n uses one immediate byte.

The ordering is based on when the first instruction of its kind shows up on the table, which doesn't end up being super useful. I should probably split them into categories at some point.

# No op
- Mnemonic: NOP
- Signature: 0000 0000

# Two byte immediate load
- Mnemonic: LD XX,nn
- Signature: 00xx 0001
- xx indicates the two byte register:
    - 00: BC
    - 01: DE
    - 10: HL
    - 11: SP
- Is followed by a two byte immediate value nn

# One byte indirect load (from register to memory)
- Mnemonic: LD (XX),A
- Signature: 000x 0010
- x indicates the two byte register to use as the address to load to
  - 0: BC
  - 1: DE

# Two byte increment
- Mnemonic INC XX
- Signature: 00xx 0011
- xx indicates the two byte register as in Two byte immediate load

# One byte increment (also can be indirect)
- Mnemonic: INC X
- Mnemonic: DEC (HL)
- Signature: 00xx x100
- xxx indicates the target to increment:
  - 000: B
  - 001: C
  - 010: D
  - 011: E
  - 100: H
  - 101: L
  - 110: (HL)
  - 111: A

# One byte decrement (can also be indirect)
- Mnemonic: DEC X
- Mnemonic: DEC (HL)
- Signature: 00xx x101
- xxx indicates the target to decrement as in One byte increment

# One byte immediate load (can also be indirect)
- Mnemonic LD X,n
- Signature : 00xx x110
- xxx indicates the  target to load into as in One byte increment

# Rotate A left with carry
- Mnemonic: RLC A
- Signature: 0000 0111
- The leftmost bit is shifted into the carry flag

# Two byte immediate indirect load (from SP to (nn))
- Mnemonic: LD (nn),SP
- Signature: 0000 1000

# Two byte add
- Mnemonic: ADD HL,XX
- HL = HL + XX
- Signature: 00xx 1001
- xx indicates the target to add to HL as in Two byte immediate load

# One byte indirect load (from memory to register)
- Mnemonic: LD A,(XX)
- Signature: 000x 1010
- x indicates the register to use as the address to load from as in One byte indirect load (from register to memory)

# Two byte decrement
- Mnemonic: DEC XX
- Signature: 00xx 1011
- xx indicates the target to decrement as in Two byte immediate load

# Rotate A right with carry
- Mnemonic: RRC A
- Signature: 0000 1111
- The bit shifted our the right side is placed in the carry flag

# Stop
- Mnemonic: STOP
- Signature: 0001 0000

# Immediate relative jump
- Mnemonic: JR n
- Signature: 0001 1000

# Rotate A right (without carry)
- Mnemonic: RR A
- Signature 0001 1111

# Conditional immediate relative jump
- Mnemonic: JR CC,n
- Signature: 001c c000
- cc is the condition to jump on:
  - 00: NZ (zero flag is not set)
  - 01: Z (zero flag is set)
  - 10: NC (carry flag is not set)
  - 11: C (carry flag is set)

# Convert A to packed BCD (binary coded decimal)
- Mnemonic: DAA
- Signature: 0010 0111

# Complement A
- Mnemonic: CPL
- Signature: 0010 1111

# Set the carry flag
- Mnemonic: SCF
- Signature: 0011 0111

# Complement the carry flag
- Mnemonic: CCF
- Signature: 0011 1111

# One byte load
- Mnemonic: LD XX,YY
- Signature: 01xx xyyy with the special exception of 0111 0110
- YY -> XX
- xxx and yyy are encoded as in One byte increment
- 0111 0110 would normally be LD (HL),(HL) (essentially a NO OP) but is instead HALT

# Add
- Mnemonic: ADD A,X
- Signature: 1000 0xxx
- This can be decoded similarly to an ADC since if bit 3 is set it indicates to use the carry flag and if it's unset it indicates to ignore the carry fag.
- xxx is encoded as in One byte increment

# Add with carry
- Mnemonic: ADC A,X
- Signature: 1000 1xxx
- xxx is encoded as in One byte increment

# Subtract
- Mnemonic: SUB A,X
- Signature: 1001 0xxx
- As with Add this can be decoded similarly to SBC since bit 3 indicates whether subtract the carry flag as well or not.
- xxx is encoded as in One byte increment

# Subtract with carry
- Mnemonic: SBC A,X
- Signature: 1001 1xxx
- xxx is encoded as in One byte increment

# Bitwise and
- Mnemonic: AND X
- The mnemonic is weird here since for most other operations we specify A explicitly, but here the target we are anding with is also A and the result is stored in A. The same style of mnemonic is used for all the logical operations.
- Signature: 1010 0xxx
- xxx is encoded as in One byte increment

# Bitwise xor
- Mnemonic: XOR X
- Signature: 1010 1xxx
- xxx is encoded as in One byte increment

# Bitwise or
- Mnemonic: OR X
- Signature 1011 0xxx
- xxx is encoded as in One byte increment

# Compare
- Mnemonic: CP X
- Signature: 1011 1xxx
- Again, the comparison is done between X and A implicitly, but the result is stored in the flags register.
- xxx is encoded as in One byte increment

# Conditional return
- Mnemonic: RET CC
- Signature: 110c c000
- cc is encoded as in Conditional immediate relative jump

# Pop (all pops are two bytes)
- Mnemonic: POP XX
- Signature: 11xx 0001
- xx is encoded as in Two byte immediate load

# Two byte conditional immediate jump (long jump?)
- Mnemonic: JP CC,nn
- Signature: 110c c011
- cc is encoded as in Conditional immediate relative jump

# Two byte immediate jump (long jump/unconditional)
- Mnemonic JP nn
- Signature 1100 0011

# Immediate conditional call
- Mnemonic CALL CC,nn
- Signature: 110c c100
- cc is encoded as in Conditional immediate relative jump

#  Push (all pushes are two bytes)
- Mnemonic PUSH XX
- Signature 11xx 0101
- xx is encoded as in Two byte immediate load

# One byte immediate add
- Mnemonic ADD A,n
- Signature 1100 0110

# Restart (more like a software interrupt call like x86's INT)
- Mnemonic: RST AA
- Signature: 11aa a111
- The address that we restart to is the value at aaa * 8. The table below explains this well enough:
  - 000: 0x00
  - 001: 0x08
  - 010: 0x10
  - 011: 0x18
  - 100: 0x20
  - 101: 0x28
  - 110: 0x30
  - 111: 0x38

# Return unconditionally
- Mnemonic RET
- Signature: 1100 1001

# CB Ops
- Mnemonic: none
- Signature: 1100 1011 (0xCB)
- This just indicates we need to look at the next byte to determine the actual instruction

# Call unconditionally
- Mnemonic: CALL nn
- Signature: 1100 1101 (0xCD)

# One byte immediate add with carry
- Mnemonic: ADC A,n
- Signature: 1100 1110 (0xCE)

# Halt and catch fire (or undefined instructions)
- Mnemonic: none
- Signature: any of:
  - 0xD2, 0xDB, 0xDD, 0xE2, 0xE3, 0xEB, 0xEC, 0xED, 0xF2, 0xF4, 0xFC, 0xFD
- Dragons etc.

# Return unconditionally and enable interrupts
- Mnemonic: RETI
- Signature: 1101 1001 (0xD9)

# One byte immediate subtract with carry
- Mnemonic: SBC A,n
- Signature: 1101 1110 (0xDE)

# One byte immediate page zero load (from register to memory)
- Mnemonic: LDH (n),A
- Signature: 1110 0000 (0xE0)
- The address used is (0xFF00 + n)

# One byte page zero load (from register to memory)
- Mnemonic: LDH (C),A
- Signature: 1110 0010 (0xE2)

# And with immediate value
- Mnemonic: AND n
- Signature: 1110 0110 (0xE6)

# Add immediate value to stack pointer
- Mnemonic: ADD SP,n
- Signature: 1110 1000 (0xE8)

# Jump (unconditional and indirect)
- Mnemonic: JP (HL)
- Signature: 1110 1001 (0xE9)

# Load immediate address (from register to memory)
- Mnemonic: LD (nn),A
- Signature: 1110 1010 (0xEA)

# Xor immediate value (and store it in A? yeah probably...)
- Mnemonic: XOR n
- Signature: 1110 1110 (0xEE)

# One byte immediate page zero load (from memory to register)
- Mnemonic: LDH A,(n)
- Signature: 1111 0000 (0xF0)

# Disable Interrupts
- Mnemonic: DI
- Signature: 1111 0011 (0xF3)

# Or with immediate value
- Mnemonic: OR n
- Signature: 1111 0110 (0xF6)

# Load into HL SP plus an immediate value
- Mnemonic: LD HL,SP+n
- Signature: 1111 1000 (0xF8)

# Load HL into SP
- Mnemonic: LD SP,HL
- Signature: 1111 1001 (0xF9)

# Load immediate address (from memory to register)
- Mnemonic: LD A,(nn)
- Signature: 1111 1010 (0xFA)

# Enable Interrupts
- Mnemonic: EI
- Signature: 1111 1011 (0xFB)

# Compare immediate value
- Mnemonic: CP A
- Signature: 1111 1110 (0xFE)

All the instructions that follow are the extended instructions that are prefixed by 0xCB.

# Rotate left with carry
- Mnemonic: RLC X
- Signature: 0xCB 0000 0xxx
- xxx indicates the target to rotate as in One byte increment

# Rotate right with carry
- Mnemonic: RRC X
- Signature 0xCB 0000 1xxx
- xxx indicates the target to rotate as in One byte increment

# Rotate left
- Mnemonic: RL X
- Signature 0xCB 0001 0xxx
- xxx indicates the target to rotate as in One byte increment

# Rotate right
- Mnemonic: RR X
- Signature 0xCB 0001 1xxx
- xxx indicates the target to rotate as in One byte increment

# Shift left
- Mnemonic: SLA X
- Signature: 0xCB 0010 0xxx
- xxx indicates the target to shift as in One byte increment

# Shift right (signed, means the MSB is sticky and will maintain its value)
- Mnemonic: SRA X
- Signature: 0xCB 0010 1xxx
- xxx indicates the target to rotate as in One byte increment

# Swap nibbles
- Mnemonic: SWAP X
- Signature -xCB 0011 0xxx
- xxx indicates the target to rotate as in One byte increment
- for example, this will turn 0xAB to 0xBA

# Shift right (unsigned)
- Mnemonic: SRL X
- Signature 0xCB 0011 1xxx
- xxx indicates the target to rotate as in One byte increment

# Bit test
- Mnemonic: BIT b,X
- Signature: 0xCB 01bb bxxx
- bbb indicates the bit to test. Should be obvious, but to be sure:
  - 000: 0
  - 001: 1
  - 010: 2
  - 011: 3
  - 100: 4
  - 101: 5
  - 110: 6
  - 111: 7
- xxx indicates the target to test as in One byte increment
- The zero flag is set if the bit is zero and unset otherwise

# Bit reset
- Mnemonic: RES b,X
- Signature: 0xCB 10bb bxxx
- bbb indicates the bit to reset as in Bit test
- xxx indicates the target register as in One byte increment

# Bit set
- Mnemonic: SET b,X
- Signature: 0xCB 11bb bxxx
- bbb indicates the bit to set as in Bit test
- xxx indicates the target register as in One byte increment
