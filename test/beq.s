_start:
    li a0, 7
    li a1, 8
    beq a0, a1, branch
    li a2, 9
    j end
branch:
    li a2, 1

end:
    .word 0xfeedfeed
