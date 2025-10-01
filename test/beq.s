_start:
    li a0, 7
    li a1, 7
    beq a0, a1, branch
    li a2, 0
    j 12
branch:
    li a2, 1
    .word 0xfeedfeed
