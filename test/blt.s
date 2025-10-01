_start:
    li a0, -2
    li a1, 1
    blt a0, a1, 8
    li a2, 0
    j 12
8:
    li a2, 1
    .word 0xfeedfeed
