_start:
    li a0, 0x8000000000000000
    li a1, 1
    srl a2, a0, a1
    .word 0xfeedfeed
