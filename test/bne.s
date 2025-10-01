_start:
    li a0, 5
    li a1, 3
    bne a0, a1, 8
    li a2, 0
    j 12
8:
    li a2, 1
    .word 0xfeedfeed
