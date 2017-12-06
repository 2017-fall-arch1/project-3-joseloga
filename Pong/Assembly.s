    .file	"Assembly"

; this are notes wiht different frequency to make the song
d:   
    .word 0x4B0             ;d = 1200

e:   
    .word 0x5DC             ;e = 1500

f:   
    .word 0x3E8             ;f = 1000
b:   
    .word 0x384             ;b = 900
    
            .text
            .global scorepoint
            
scorepoint:               ;this method is to keep track with the score of the game
        
    cmp #1, r12             ; here we compare if the 1st player score if yes we add 1 to the score 
    JNE player2             ; if not we go to check to 2nd player 
    cmp #9 ,p1              ; if the player 1 reach 10 then we call the winner method "score_board"
    JNE plus                ; if yes we add 1 to the value of the player1 score 
    mov #0,r12
    call #score_board       ; here we draw the score and then play the winner song
    
plus: 
    add #1,p1               ; here we add 1 to the score 
    jmp end                     

player2:                   
    cmp #1, r13             ; here we check if the 2nd player scores 
    JNE end                 ; if not we go to end 
    cmp #9,p2               : if player 2 reach 10 then we call the winner method "score_board"
    JNE plus2               ;if score we jump to plus2
    mov #1,r12
    call #score_board       ; here we draw the score and then play the winner song
    
plus2:
    add #1,p2               ;here we add 1 to the second player score
    
end:        
    pop 0                   ; here we end the method
    
    
        .text
        .global beat

beat:                           ;this label its note with a tempo                                 
    mov e ,r12                  ;here we choose the note initialize at top
    call #buzzer_set_period     ;here we call the buzzer to play the nppte; 
    call #delayShort            ;here we determine the tempo for the note
    pop 0
    
        .text
        .global beat2

beat2:                          ;this label its note with a tempo                                 
    mov d ,r12                  ;here we choose the note initialize at top
    call #buzzer_set_period     ;here we call the buzzer to play the nppte; 
    call #delayShort            ;here we determine the tempo for the note
    pop 0
        .text
        .global beat3

    pop 0
beat3:                          ;this label its note with a tempo                                 
    mov f ,r12                  ;here we choose the note initialize at top
    call #buzzer_set_period     ;here we call the buzzer to play the nppte; 
    call #delayShort            ;here we determine the tempo for the note
    pop 0

beat4:                          ;this label its note with a tempo                                 
    mov b ,r12                  ;here we choose the note initialize at top
    call #buzzer_set_period     ;here we call the buzzer to play the nppte; 
    call #delayShort            ;here we determine the tempo for the note
    pop 0
         
        .text
        .global song

    pop 0
song:                           ;this method play the notes in a way to create a song for the winner
    call #beat                      
    call #beat2
    call #beat3
    call #beat2
    call #beat
    call #beat4
    pop 0
 
 
