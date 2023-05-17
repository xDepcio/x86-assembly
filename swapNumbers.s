section .text
global  swapNumbers
swapNumbers:
    push    ebp
    mov     ebp, esp

    push    ebx
    push    esi
    push    edi

    mov     eax, [ebp + 8]  ; address of start of chars array in eax
getEnd:
    mov     dl, [eax]
    cmp     dl, 0
    je      saveEnd
    inc     eax
    jmp     getEnd

saveEnd:
    mov     esi, eax    ; pointer on the last element of string in eax
    mov     eax, [ebp+8]
    dec     eax
    inc     esi

getLeft:
    inc     eax
    mov     dl, [eax]   ; char from beggining in dl
    cmp     dl, '0'
    jl      getLeft
    cmp     dl, '9'
    jge     getLeft

getRight:
    dec     esi
    mov     cl, [esi]   ; char from end in cl
    cmp     cl, '0'
    jl      getRight
    cmp     cl, '9'
    jge     getRight

swap:
    cmp     esi, eax
    jle     end

    mov     [eax], cl
    mov     [esi], dl
    jmp     getLeft

end:
    mov     eax, [ebp+8]

    pop     edi
    pop     esi
    pop     ebx

    mov     esp, ebp
    pop     ebp
    ret


; removerng:
;     push    ebp
;     mov     ebp, esp
;     mov     eax, [ebp+8]
; begin:
;     mov     cl, [eax]
;     cmp     cl, 0
; ; # cmp ustawia flagi
;     jz      end
; ; # skok na podstawie flagi ustawionej przez cmp
;     add     cl, 1
;     mov     [eax], cl
;     inc     eax
;     jmp     begin
; end:
;     mov     esp, ebp
;     pop     ebp
;     ret

; # stos pełny schodzący
; # |
; # |
; # |
; # |
; # | char*    | + 8 from ebp
; # | RET      | + 4 from EBP
; # | EBP(old) | <- EBP points HERE <- ESP points also here after prolog.
; # | a        |
; # | b        |
; # | c        |
; #
; #
; #

; # begin:    // Przykład jakbyśmy alokowali zmienne lokalne na stosie
; # int a;    // sub esp, 4
; # a = 2;    // mov [ebp-4], 2
; # int b, c; // sub esp, 8
; # a = 3;    // mov [ebp-4], 3
; #
; #
