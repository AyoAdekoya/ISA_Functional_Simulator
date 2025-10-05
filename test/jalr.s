# should be a 1
_start:
    li a0, 0x14
    jalr a2, 0(a0)
    li a2, 0
    li a3, 8008
    j end
branch:
    li a2, 1

end:
    .word 0xfeedfeed
