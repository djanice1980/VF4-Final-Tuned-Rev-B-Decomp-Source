! 0x0c1372e8  size 0x4  exception_return_bare_0c1372e8
! Written by hand from the game's bytes (tools/match/to_asm.py takes routines
! that src/ lists, and this code is not one): hand-written assembly; not Sega's original text.

	.text
	.balign	4

! exception_return_bare_0c1372e8
!
! A bare return from an exception: rte and the nop in its delay slot,
! standing between the pool of irq_entry_via_firmware (0c13729c) and
! firmware_note_and_return (0c1372ec).
!
! Nothing in the image reaches it. No branch has it as its target, no
! word holds its address (in any of the three mirrors of memory, at any
! alignment), no pc-relative load or mova points at it, and what comes
! before it is a pool, so nothing runs into it from above. It is not
! padding either: the fill in this code is nops alone, and the routine
! after it starts on a 4-byte boundary with or without it.
!
! It stands to firmware_note_and_return as irq_entry_via_firmware_alt
! (0c137298) stands to irq_entry_via_firmware: each is two instructions
! placed right before the routine that calls the firmware, and each does
! that routine's job without the call (there: the hand-over to
! context_save_entry; here: the return from the exception). So it reads
! as the plain form of firmware_note_and_return, the way out of an
! exception when the firmware is not to be told, and belongs with that
! routine, not with irq_entry_via_firmware before it.
!
! In:    nothing in registers. Exception state: SPC and SSR hold the PC
!        and SR to go back to.
! Out:   does not return to a caller: the code at SPC resumes with
!        SR = SSR. No register is changed.
! Notes: it is code, but it has no entry in the function list, so it
!        carries a name made from its address and is typed as an object
!        (not counted as a function). What would send the processor
!        here (code outside the image, or a build of this code with the
!        firmware calls left out) is not established.
	.global	_exception_return_bare_0c1372e8
	.type	_exception_return_bare_0c1372e8,@object
_exception_return_bare_0c1372e8:
	rte				! back to the interrupted code (PC = SPC, SR = SSR)
	nop				! (delay slot)
	.size	_exception_return_bare_0c1372e8,0x4
