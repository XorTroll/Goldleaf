.section .text.nroEntrypointTrampoline, "ax", %progbits

.global nroEntrypointTrampoline
.type   nroEntrypointTrampoline, %function
.align 2

.global __libnx_exception_entry
.type   __libnx_exception_entry, %function

.cfi_startproc

nroEntrypointTrampoline:

    // Reset stack pointer.
    adrp x8, __stack_top //Defined in libnx.
    ldr  x8, [x8, #:lo12:__stack_top]
    mov  sp, x8

    // Call NRO.
    blr  x2

    // Save retval
    adrp x1, g_lastRet
    str  w0, [x1, #:lo12:g_lastRet]

    // Reset stack pointer and load next NRO.
    adrp x8, __stack_top
    ldr  x8, [x8, #:lo12:__stack_top]
    mov  sp, x8

    b    loadNro

.cfi_endproc

.section .text.__libnx_exception_entry, "ax", %progbits
.align 2

.cfi_startproc

__libnx_exception_entry:
    adrp x7, g_nroAddr
    ldr  x7, [x7, #:lo12:g_nroAddr]
    cbz  x7, __libnx_exception_entry_fail
    br   x7

__libnx_exception_entry_fail:
    mov w0, #0xf801
    bl svcReturnFromException
    b .

.cfi_endproc