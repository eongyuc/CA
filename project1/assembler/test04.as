        lw      0       1       one      reg1 = 1
        lw      0       2       two      reg2 = 2
        add     1       2       3        reg3 = reg1 + reg2
        lw      0       4       five     reg4 = 5
        add     3       4       4        reg4 = reg3 + reg4
        sw      0       4       store    store에 reg4 저장.
        halt
one     .fill   1                        put 1
two     .fill   2                        put 2
three   .fill   3                        put 3
four    .fill   4                        put 4
five    .fill   5                        put 5
store   .fill   10                       put 10
