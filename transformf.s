section .text
global transformf
; char* transformPixels(
    ;     char* pixels, - rdi
    ;     char* pixels_copy, - rsi
    ;     int origin_x, - rdx
    ;     int origin_y, - rcx
    ;     int radius, - r8
    ;     int angle_deg, - r9
    ;     int width, - [rbp+16]
    ;     int height - [rbp+24]
    ; );
transformf:
    push    rbp
    mov     rbp, rsp

    sub     rsp, 8
    mov     [rsp], dword 0x40490fdb ; store PI as 32bit floating number at [rbp-8]
    sub     rsp, 8      ; (fp_32)angle in radians at [rbp-16]
    sub     rsp, 8      ; distance_from_center at [rbp-24]
    sub     rsp, 8      ; (fp_32)rot_angle at [rbp-32]
    sub     rsp, 8      ; (fp_32)sin(rot_angle) at [rbp-40]
    sub     rsp, 8      ; (fp_32)cos(rot_angle) at [rbp-48]
    sub     rsp, 8      ; (int_32)padding_size at [rbp-56]

    push    rbx
    push    r12
    push    r13
    push    r14
    push    r15

    mov     eax, dword [rbp+16]
    imul    eax, dword [rbp+24]
    mov     r10d, eax    ; total pixels amount in r10d

    cvtsi2ss xmm0, r9d
    mov     eax, dword 180
    cvtsi2ss xmm1, eax
    movss   xmm2, dword [rbp-8]
    divss   xmm0, xmm1
    mulss   xmm0, xmm2
    movss   [rbp-16], xmm0 ; store angle in radians as floating point number at [rbp-16]

    mov     eax, [rbp+24]
    sub     eax, ecx
    mov     ecx, eax    ; origin_y = height - origin_y

    mov     eax, 0b11
    and     eax, [rbp+16]
    mov     [rbp-56], eax   ; store (int_32)padding_size at [rbp-56]

    mov     r11d, 0     ; current pixel num in r11d
tloop:
    mov     r14d, edx
    mov     eax, r11d
    xor     rdx, rdx
    div     dword [rbp+16]
    mov     r12d, edx   ; store curr x in r12d
    mov     r13d, eax   ; store curr y in r13d
    mov     edx, r14d

    mov     r14d, r12d
    sub     r14d, edx   ; dx in r14d
    mov     r15d, r13d
    sub     r15d, ecx   ; dy in r15d

    imul    r14d, r14d
    imul    r15d, r15d
    add     r14d, r15d  ; distance from center^2 in r14d

    mov     r15d, r8d
    imul    r15d, r15d  ; radius^2 in r15d

    cmp     r14d, r15d
    jg      next_px

valid_px:
    mov     [rbp-24], r14d
    fild    dword [rbp-24]
    fsqrt
    fistp   dword [rbp-24]  ; (int_32)distance_from_center stored at [rbp-24]

    mov     r14d, r8d
    sub     r14d, [rbp-24]
    cvtsi2ss xmm0, r14d
    cvtsi2ss xmm1, r8d
    divss   xmm0, xmm1
    movss   xmm1, [rbp-16]
    mulss   xmm0, xmm1
    movss   [rbp-32], xmm0  ; (fp_32)rot_angle in radians of curr pixel at [rbp-32]

    fld    dword [rbp-32]
    fsin
    fstp   dword [rbp-40]  ; store (fp_32)sin(rot_angle) at [rbp-40]
    fld    dword [rbp-32]
    fcos
    fstp   dword [rbp-48]  ; store (fp_32)cos(rot_angle) at [rbp-48]
    mov     r14d, r12d
    sub     r14d, edx       ; store (int_32)x-origin_x in r14d
    mov     r15d, r13d
    sub     r15d, ecx       ; store (int_32)y-origin_y in r14d
    movss   xmm0, [rbp-40]
    movss   xmm1, [rbp-48]
    cvtsi2ss xmm2, r14d
    cvtsi2ss xmm3, r15d
    mulss   xmm1, xmm2
    mulss   xmm0, xmm3
    subss   xmm1, xmm0
    cvtss2si r14d, xmm1     ; out_x - origin_x in r14d
    movss   xmm0, [rbp-40]
    movss   xmm1, [rbp-48]
    mulss   xmm0, xmm2
    mulss   xmm1, xmm3
    addss   xmm0, xmm1
    cvtss2si r15d, xmm0     ; out_y - origin_y in r15d
    add     r14d, edx       ; out_x in r14d
    add     r15d, ecx       ; out_y in r15d

    mov     eax, 0
    cmp     r14d, eax
    cmovl   r14d, eax
    mov     ebx, [rbp+16]
    dec     ebx
    cmp     r14d, ebx
    cmovge  r14d, ebx
    cmp     r15d, eax
    cmovl   r15d, eax
    mov     ebx, [rbp+24]
    dec     ebx
    cmp     r15d, ebx
    cmovge  r15d, ebx

    mov     eax, [rbp-56]
    imul    eax, r15d
    imul    r15d, [rbp+16]
    add     r15d, r14d
    lea     r14, [r15d*2 + r15d]   ; offset of source pixel in r14
    add     r14d, eax
    add     r14, rdi               ; address of source pixel in r14
    mov     eax, [rbp-56]
    imul    eax, r13d
    lea     rbx, [r11d*2 + r11d]
    add     ebx, eax
    lea     r15, [rsi + rbx]       ; address of dest pixel in r15
    mov     al, [r14]              ; src pixel R value in al
    mov     [r15], al
    mov     al, [r14+1]            ; src pixel G value in al
    mov     [r15+1], al
    mov     al, [r14+2]            ; src pixel B value in al
    mov     [r15+2], al

next_px:
    inc     r11d
    cmp     r11d, r10d
    jne     tloop

end:
    mov     rax, rsi

    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     rbx

    mov     rsp, rbp
    pop     rbp
    ret
