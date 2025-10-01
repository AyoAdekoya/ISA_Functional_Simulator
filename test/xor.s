 _start:
    li a0, 0xF0F0
    li a1, 0x0F0F
    xor a2, a0, a1
    .word 0xfeedfeed
