; test.asm

.segment "ZEROPAGE"
    .org $00
    PTR: .res 2    ; Pointer for string copy

.segment "BANK0"
    .org $8000
    SEI             ; 78 - Disable interrupts
    CLD             ; D8 - Clear decimal mode
    LDA #<message   ; Load low byte of message address
    STA PTR         ; Store in zero page
    LDA #>message   ; Load high byte of message address
    STA PTR+1       ; Store in zero page
    LDX #$00        ; A2 00 - Initialize X register
    STX $FF00       ; 8E 00 FF - Select Bank 0 (for initialization)

loop:
    LDY #$00        ; Clear Y for indirect addressing
    LDA (PTR), Y    ; Load character from message using zero-page pointer
    BEQ done        ; If zero (end of string), jump to done
    STA $0400, X    ; Store character in framebuffer
    INC PTR         ; Increment pointer low byte
    BNE no_inc_high ; If no carry, skip high byte increment
    INC PTR+1       ; Increment pointer high byte
no_inc_high:
    INX             ; Increment X
    JMP loop        ; Jump back to loop
done:
    JMP $C000       ; Jump to Bank 1 for next part

message:
    .byte "Hello World!", 0

.segment "BANK1"
    .org $C000
    LDX #$00        ; A2 00 - Initialize X register
loop2:
    LDA $0400, X    ; Load character from framebuffer
    BEQ done2       ; If zero (end of string), jump to done
    INX             ; Increment X
    JMP loop2       ; Loop to keep display active
done2:
    BRK             ; 00 - Break

.segment "VECTORS"
    .org $FFFC
    .word $8000   ; Reset vector pointing to $8000 (start of Bank 0)