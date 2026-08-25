; =============================================================================
; @file bignum_prime_gen.asm
; @brief x86-64 implementation of exact-bit-length probable-prime generation.
; @details System V AMD64 implementation. Candidate generation and primality
; validation are delegated to existing production bignum_random and
; bignum_is_prime APIs; retry, candidate shaping, validation and publication
; are implemented here without heap allocation or mutable global state.
; =============================================================================
BITS 64
DEFAULT REL

%define CAPACITY         32
%define LEN_OFFSET       256
%define RECORD_SIZE      264
%define FRAME_SIZE       568
%define BOUND_OFFSET     0
%define CANDIDATE_OFFSET 264
%define ATTEMPT_SLOT     -56
%define PRIME_SLOT       -64
%define CANDIDATE_SLOT   -72
%define SUCCESS          0
%define ERROR_NULL       -1
%define ERROR_BITS       -2
%define ERROR_ROUNDS     -3
%define ERROR_ATTEMPTS   -4
%define ERROR_RANDOM     -5
%define ERROR_PRIMALITY  -6

SECTION .text
global bignum_prime_gen
extern bignum_random
extern bignum_is_prime

; bignum_prime_gen(out=rdi, bits=rsi, rounds=rdx, max_attempts=rcx)
bignum_prime_gen:
    push    rbp
    mov     rbp, rsp
    push    rbx
    push    r12
    push    r13
    push    r14
    push    r15
    sub     rsp, FRAME_SIZE

    mov     r12, rdi
    mov     r13, rsi
    mov     r14, rdx
    mov     r15, rcx

    test    r12, r12
    jz      .error_null
    cmp     r13, 2
    jb      .error_bits
    cmp     r13, CAPACITY * 64
    ja      .error_bits
    test    r14, r14
    jz      .error_rounds
    test    r15, r15
    jz      .error_attempts

    lea     rbx, [rsp + BOUND_OFFSET]
    lea     r11, [rsp + CANDIDATE_OFFSET]
    mov     [rbp + CANDIDATE_SLOT], r11

    ; Construct normalized exclusive bound 2^bits.
    mov     rdi, rbx
    xor     eax, eax
    mov     ecx, CAPACITY + 1
    rep stosq
    mov     rax, r13
    dec     rax
    mov     rcx, rax
    shr     rax, 6
    and     ecx, 63
    mov     rdx, 1
    shl     rdx, cl
    mov     [rbx + rax * 8], rdx
    inc     rax
    mov     [rbx + LEN_OFFSET], rax
    mov     qword [rbp + ATTEMPT_SLOT], 0

.retry:
    ; candidate = uniform random sample in [0, bound).
    mov     r11, [rbp + CANDIDATE_SLOT]
    mov     rdi, r11
    mov     rsi, rbx
    call    bignum_random
    test    eax, eax
    jnz     .error_random

    ; Force exact requested high bit and oddness.
    mov     r11, [rbp + CANDIDATE_SLOT]
    mov     rax, r13
    dec     rax
    mov     rcx, rax
    shr     rax, 6
    and     ecx, 63
    mov     rdx, 1
    shl     rdx, cl
    or      [r11 + rax * 8], rdx
    or      qword [r11], 1
    inc     rax
    mov     [r11 + LEN_OFFSET], rax

    ; Validate candidate with Miller--Rabin.
    mov     r11, [rbp + CANDIDATE_SLOT]
    mov     rdi, r11
    mov     rsi, r14
    lea     rdx, [rbp + PRIME_SLOT]
    call    bignum_is_prime
    test    eax, eax
    jnz     .error_primality
    cmp     dword [rbp + PRIME_SLOT], 0
    jne     .publish

    inc     qword [rbp + ATTEMPT_SLOT]
    mov     rax, [rbp + ATTEMPT_SLOT]
    cmp     rax, r15
    jb      .retry

.error_primality:
    mov     eax, ERROR_PRIMALITY
    jmp     .epilogue

.publish:
    mov     rdi, r12
    mov     rsi, [rbp + CANDIDATE_SLOT]
    mov     ecx, CAPACITY + 1
    rep movsq
    mov     eax, SUCCESS
    jmp     .epilogue

.error_null:
    mov     eax, ERROR_NULL
    jmp     .epilogue
.error_bits:
    mov     eax, ERROR_BITS
    jmp     .epilogue
.error_rounds:
    mov     eax, ERROR_ROUNDS
    jmp     .epilogue
.error_attempts:
    mov     eax, ERROR_ATTEMPTS
    jmp     .epilogue
.error_random:
    mov     eax, ERROR_RANDOM

.epilogue:
    add     rsp, FRAME_SIZE
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     rbx
    pop     rbp
    ret

SECTION .note.GNU-stack noalloc noexec nowrite progbits
