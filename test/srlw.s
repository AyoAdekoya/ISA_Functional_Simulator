_start:
    li a0, 0x80000000
    li a1, 1
    srlw a2, a0, a1
    .word 0xfeedfeed
