

	.file	"Assembly"




            .text
            .global scorepoint
scorepoint:
        
cmp #1, r12
JNE player2
cmp #9 ,p2
JNE plus
mov #0,r12
call #score_board

plus: 
    add #1,p1
    jmp end

player2:
    JNE end
    cmp #9, p2
    JNE plus2
    mov #1,r12
    call #score_board

plus2:
    add #1,p2
    
end:
    pop 0
