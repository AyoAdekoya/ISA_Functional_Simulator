_start:
    li a0, 0x8000000000000000
    li t0, 0xffffffffffffffff
    li a2, 0xffffffff
    srl a1, a0, 1
    srl a3, a2, 2
    srl t1, t0, 63
    .word 0xfeedfeed