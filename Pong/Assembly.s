    .file	"Assembly"

   
    
            .text
            .global scorepoint
scorepoint:
        
    cmp #1, r12
    JNE player2
    cmp #9 ,p1
    JNE plus
    mov #0,r12
    call #score_board

plus: 
    add #1,p1
    jmp end

player2:
    cmp #1, r13
    JNE plus2
    cmp #9,p2
    JNE plus2
    mov #1,r12
    call #score_board

plus2:
    add #1,p2
    
end:
    pop 0
    
 /*
count:      .data
            .word 0
            .text
    
wdt_c_handler2:
      /*p1out |=green led
      add #1,&count
while:cmp #5,&count
      JNE DONE
      
      mov &ml0,r12
      mov &fieldFence,r13
      mov &ml1,r14
      mov &ml3,r15
      call# mlAdvanceBall      
    
    mov #2,r12
    call score_board
    
    mov #0, r12
    call buzzer_set_period
    mov #1,redrawscreen
    jmp
        
    
DONE:P1OUT &= ~GREEN_LED;
    */
