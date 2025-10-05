_start:
    jal a2, end
    li a1, 3
    j 12
end:
    li a3, 2
    .word 0xfeedfeed
