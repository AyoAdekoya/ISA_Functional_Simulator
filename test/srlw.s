_start:
    li a0, 0xff000000
    li a1, 2
    srlw a2, a0, a1
    .word 0xfeedfeed
