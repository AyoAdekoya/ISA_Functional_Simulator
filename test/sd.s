_start:
    li sp, 0
    li a0, 0x1122334455667788
    sd a0, 0x50(sp)
    ld a1, 0(sp)
    .word 0xfeedfeed
