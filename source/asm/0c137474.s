! 0x0c137474  size 0x2a  context_save_dispatch_counted
! Written by hand from the game's bytes (tools/match/to_asm.py does not take a routine
! that runs into another file's routine): hand-written assembly; not Sega's original text.

	.text
	.balign	4

! Entry that runs on into context_save_dispatch_nested (0x0C13749E): it
! keeps r0, counts the nesting level, and on the first level saves the
! stack pointer and switches to the handler stack. The words it uses
! (0x0C137608-0x0C13761B) are context_save_dispatch_nested's own, after
! that routine, so its three pc-relative instructions are raw words here.
! In:  r0 = value to keep, r1 pushed on the stack by the caller (it is popped here)
! Out: falls into context_save_dispatch_nested with ssr and spc pushed; r1 restored, r0 = the word kept at 0x0C137618
	.global	_context_save_dispatch_counted
	.type	_context_save_dispatch_counted,@function
_context_save_dispatch_counted:
	mov	r0,r1
	.word	0xc769	! mova 0x0C13761C,r0 : just past the kept-r0 word
	mov.l	r1,@-r0	! keep r0 at 0x0C137618
	add	#-4,r0	! r0 = the nesting count word 0x0C137614
	mov.l	@r0,r1
	add	#1,r1	! one level deeper
	mov.l	r1,@r0
	add	#4,r0
	dt	r1	! T = (this is the first level)
	mov.l	@r0,r0	! r0 back from the kept word
	bf.s	.L0c13749a	! nested: stay on the current stack
	mov.l	@r15+,r1	! (slot) r1 back from the caller's push
	mov.l	r0,@-r15
	.word	0xc75e	! mova 0x0C137608,r0 : the saved stack pointer word
	add	#4,r15
	mov.l	r15,@r0	! save the interrupted stack pointer
	add	#-4,r15
	mov.l	@r15+,r0
	.word	0xdf5c	! mov.l 0x0C13760C,r15 : switch to the handler stack
.L0c13749a:
	stc.l	ssr,@-r15	! push the saved status and pc, as the nested entry expects
	stc.l	spc,@-r15
	.size	_context_save_dispatch_counted,0x2a
