        lw      0       2       thirty    load reg2 with the value at memory address 'thirty'
        lw      0       1       five      load reg1 with the value at memory address 'five'
start   add     1       1       1         reg1 = reg1 + reg1
        beq     2       1       done      jump to 'done' if reg0 == reg1
        beq     0       0       start     jump back to 'start'
        noop                              no operation
done    halt                              end of program
five    .fill   5                         memory address 'five'에 5저장
thirty  .fill   40
