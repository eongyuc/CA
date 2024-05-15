start   lww     0       4       four     reg4에 4 넣기.
        lw      0       1       one      reg1에 1 넣기.
        jalr    4       2                address 4로 이동하기.
        halt    
        add     4       1       2        reg2 = reg1 + reg4
        halt
four    .fill   4
one     .fill   1
