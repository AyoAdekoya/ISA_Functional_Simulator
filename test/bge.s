_start:
    li a0, 3
    li a1, 3
    bge a0, a1, branch
    li a2, 0
    j 12
8:
    li a2, 1
    .word 0xfeedfeed
