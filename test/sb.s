_start:
    li sp, 0x50
    li a0, 0x7fffffffffffffff
    sb a0, 0(sp) # Store a byte i.e 0xFF
    lw a1, 0(sp) # Try to load a word but only a byte should appear i.e 0xFF
    .word 0xfeedfeed
