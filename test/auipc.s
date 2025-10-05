_start:
    # Example 1: AUIPC alone
    auipc t0, 0          # t0 = PC (no offset)
    auipc t1, 1          # t1 = PC + (1 << 12) = PC + 4096
    auipc t2, 0xFFFFF     # t2 = PC - (1 << 12) = PC - 4096

.word 0xfeedfeed