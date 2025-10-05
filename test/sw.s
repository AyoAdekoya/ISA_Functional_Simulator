_start:
    li sp, 0x50
    li a0, 0xdeadbeef
    sw a0, 0(sp)
    ld a1, 0(sp)
    lw a2, 0(sp)
    .word 0xfeedfeed
