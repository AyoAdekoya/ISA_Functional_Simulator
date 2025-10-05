_start:
    li a0, 0x0
    li a1, 0x0
    not a2, a0
    not a3, a1
    and a4, a2, a3
    .word 0xfeedfeed
