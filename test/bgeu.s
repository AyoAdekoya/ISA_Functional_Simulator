_start:
    li a0, 1
    li a1, -1
    bgeu a0, a1, branch
    li a2, 0
    j end
branch:
    li a2, 1

end:
    .word 0xfeedfeed
