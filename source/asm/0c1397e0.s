! 0x0c1397e0  size 0x2008  irq_private_stack, irq_private_stack_saved_sp
! Written by hand from the game's bytes (tools/match/to_asm.py takes routines,
! not data kept in .text): hand-written assembly; not Sega's original text.

	.text
	.balign	4

! The stack of the graphics library's interrupt wrapper, which its
! authors kept in .text between ocbwb_line and call_on_private_stack,
! and the word that remembers the stack it was entered on. All of it is
! zero in the image and written while the game runs, so this code has to
! be in RAM. The two wrappers that use it (call_on_private_stack,
! source/asm/0c13b7e8.s, and irq_entry_on_private_stack,
! source/asm/0c13b86c.s) reach both by address through their pools.

! irq_private_stack
!
! 8 KB of stack. The wrappers load r15 with its end, 0c13b7e0, and push
! downwards from there.
!
! In:    written by irq_entry_on_private_stack and call_on_private_stack
!        (FPSCR, FPUL, fr0-fr15, MACL, MACH and PR, 21 words) and by
!        whatever the routine they call pushes below that.
! Out:   8192 bytes, zero in the image; nothing is kept in them from
!        one use to the next.
	.global	_irq_private_stack
	.type	_irq_private_stack,@object
_irq_private_stack:
	.space	8192			! 0c1397e0 to 0c13b7df, all zero
	.size	_irq_private_stack,0x2000

! 0c13b7e0: the address the wrappers load as the top of the stack. The
! word that stands there is zero and is not used: the first push goes
! below it.
	.long	0x00000000

! irq_private_stack_saved_sp
!
! The stack pointer of the code that entered a wrapper, kept while r15
! is on irq_private_stack.
!
! In:    written by irq_entry_on_private_stack and call_on_private_stack
!        on the way in (the caller's r15); read by the same routine on
!        the way out, to go back to that stack.
! Out:   one word, zero in the image. It is a single word, so a wrapper
!        cannot be entered a second time before it returns.
	.global	_irq_private_stack_saved_sp
	.type	_irq_private_stack_saved_sp,@object
_irq_private_stack_saved_sp:
	.long	0x00000000		! no stack pointer saved yet
	.size	_irq_private_stack_saved_sp,0x4
