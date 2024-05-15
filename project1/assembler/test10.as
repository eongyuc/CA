        lw      0       1       input1    reg1 = 5
        lw      0       2       neg1      reg2 = -1
        lw      0       10      input2    reg3 = 4
        beq     0       1       end       if(reg1 == 0) { end }
        noop                              nothing
loop    add     1       1       1         reg1 = reg1 + reg1
        add     3       2       3         reg3--
        beq     3       0       end       if reg3 == 0, goto loop.
        beq     0       0       loop
end     halt                              end of program
input1  .fill   5                         
input2  .fill   4
neg1    .fill   -1
