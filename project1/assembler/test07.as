        lw      0       2       thirty
        lw      0       1       five
start   add     1       1       1
        beq     2       1       done
        beq     0       0       start
        noop
done    halt    
five    .fill   5
thir    .fill   40
