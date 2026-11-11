! 0x0c12c740  size 0xe  jump_stub, jump_stub_target, jump_stub_end
! Written by hand from the game's bytes (tools/match/to_asm.py takes routines,
! not data kept in .text): hand-written assembly; not Sega's original text.

	.text
	.balign	4

! A code template, kept in .text after code_jump_patch
! (source/gfxlib/gfx_helpers.c) and never run where it stands.
! code_jump_patch(routine, target) stores the target in
! jump_stub_target and then copies the 12 bytes from jump_stub up to
! jump_stub_end over the first 12 bytes of the routine
! (code_halfwords_copy_purged), so that a call of the routine goes to
! the target instead. The game does this once: target_passes_install
! turns the entry of ta_input_finish into a jump to
! list_input_end_with_passes.
!
! The three names are the three addresses the C code uses, so each is an
! object of its own: the code, the word it loads, and the end of the copy.

! jump_stub
!
! The template's four instructions: a jump through the word that
! follows them.
!
! In:    read as data by code_jump_patch (the start of the copy). In a
!        copy, when the patched routine is called: nothing is looked
!        at; r4-r7, pr and the stack go on to the target untouched.
! Out:   8 bytes of code. A copy leaves r0 = the target and goes on
!        there; the target returns to the patched routine's caller.
! Uses:  r0 is changed.
! Notes: the load is pc-relative (the word 8 bytes after the first
!        instruction, at a multiple of 4), so a copy works only at an
!        address that is a multiple of 4. The second nop is never run:
!        it fills the code out to 8 bytes.
	.global	_jump_stub
	.type	_jump_stub,@object
_jump_stub:
	mov.l	.Ltarget,r0		! r0 = the word after the code (the copy's own, once copied)
	jmp	@r0			! go to the target for good
	nop				! (delay slot)
	nop				! filler up to the word, never run
	.size	_jump_stub,0x8

! jump_stub_target
!
! The address the stub jumps to, copied along with the code as its
! last four bytes.
!
! In:    written by code_jump_patch (its second argument) just before
!        it copies the stub; read by the copy's first instruction.
! Out:   0 in the image; while the game runs, the target of the last
!        patch made (list_input_end_with_passes).
	.global	_jump_stub_target
	.type	_jump_stub_target,@object
_jump_stub_target:
.Ltarget:
	.long	0x00000000		! no target yet: filled in before each copy
	.size	_jump_stub_target,0x4

! jump_stub_end
!
! The end of the template: code_jump_patch passes this address as the
! place where the copy stops. Only the address is used.
!
! In:    its address is taken by code_jump_patch; nothing reads or
!        writes what is there.
! Out:   the halfword that happens to stand there, a nop: the first of
!        the nops that fill .text up to dcache_purge_lines (0c12c760).
	.global	_jump_stub_end
	.type	_jump_stub_end,@object
_jump_stub_end:
	nop				! not part of the copy
	.size	_jump_stub_end,0x2
