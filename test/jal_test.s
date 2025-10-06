# File: jal_test2.s

_start:
    add     t2, zero, 0x34     # t2 = 0x34
    jal     s0, after1         # s0 = 0x8
    li      s1, 111            # s1 = 0
after1:
    la      t0, 0x10           # t0 = 0x10
    sub     t1, s0, t2
    sd      t1, 0(t2)          # expect M[result] = 0xd4

    jal     x0, back_here
here:
    li      a1, 33             # should not happen; a1 = 0
    .word 0xfeedfeed

back_here:
    li      a5, 9              # a5 = 0x9

done:
    li      a0, 5
    sd      a0, 8(t2)          # expect M[result+8] = 5
    .word 0xfeedfeed

result:
    .quad 0, 0

    .word 0xdeadbeef
    