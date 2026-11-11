! 0x0c12c780  size 0x4a  sq_copy_by_32
! Written by hand from the game's bytes (tools/match/to_asm.py does not take a routine
! that runs into another file's routine): hand-written assembly; not Sega's original text.

	.text
	.balign	4

! Copies a block through the store queues, 32 bytes a round, when the
! destination, the source and the length are all multiples of 32;
! otherwise it goes to words_copy's alignment test (0x0C12C7D6), which
! copies by 8 bytes or by words. The three literals it loads lie after
! words_copy (0x0C12C804-0x0C12C80F), in source/asm/0c12c7ce.s.
! In:  r4 = destination, r5 = source, r6 = bytes
! Out: the block copied; r0, r1, r4-r6, fr0-fr7 changed; both queue address registers set
	.global	_sq_copy_by_32
	.type	_sq_copy_by_32,@function
_sq_copy_by_32:
	tst	r6,r6	! T = (length is 0), for words_copy's test
	mov	r4,r0
	or	r5,r0
	or	r6,r0	! r0 = destination | source | length
	tst	#31,r0	! all multiples of 32?
	.word	0x8b24	! bf _words_copy+0x8 (0x0C12C7D6): no, let words_copy do it
	fschg		! pair moves from here on
	mov	r4,r0
	shlr16	r0
	shlr8	r0	! r0 = the top byte of the destination
	.word	0xd11d	! mov.l 0x0C12C80C,r1 : the queue address register 0xFF000038
	mov.l	r0,@r1	! both queues write to the destination's area
	mov.l	r0,@(4,r1)
	.word	0xd11b	! mov.l 0x0C12C808,r1 : the mask 0xE3FFFFFF
	and	r1,r4
	.word	0xd119	! mov.l 0x0C12C804,r1 : the store queue area 0xE0000000
	or	r1,r4	! r4 = the destination seen through the store queues
	shlr2	r6
	shlr2	r6
	shlr	r6	! r6 = rounds of 32 bytes
.L0c12c7a8:
	fmov	@r5+,fr0	! read 32 bytes as four pairs
	fmov	@r5+,fr2
	fmov	@r5+,fr4
	fmov	@r5+,fr6
	dt	r6
	fmov	fr0,@r4	! write them into the queue
	add	#8,r4
	fmov	fr2,@r4
	add	#8,r4
	fmov	fr4,@r4
	add	#8,r4
	fmov	fr6,@r4
	pref	@r4	! send the queue to memory
	bf.s	.L0c12c7a8
	add	#8,r4	! (slot) next 32 bytes
	rts
	fschg		! (slot) single moves again
	.size	_sq_copy_by_32,0x4a
