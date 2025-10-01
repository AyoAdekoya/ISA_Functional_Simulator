_start:
    li a0, 16
    jalr a2, 0(a0)
    li a2, 0
    j 12
8:
    li a3, 0
    .word 0xfeedfeed
