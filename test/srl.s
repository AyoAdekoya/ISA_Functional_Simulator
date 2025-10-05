_start:
    li a0, 0x8000000000000000
    li t0, 0xffffffffffffffff
    li a2, 0xffffffff
    li a1, 1
    li a3, 2
    li t1, 63
    srl a4, a0, a1
    srl a5, a2, a3
    srl t2, t0, t1 
    .word 0xfeedfeed
