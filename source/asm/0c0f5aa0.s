! 0x0c0f5aa0  size 0x10  mtx_rot_zyx_s16_ptr
! Written by hand from the game's bytes (tools/match/to_asm.py does not take a routine
! that runs into another file's routine): hand-written assembly; not Sega's original text.

	.text
	.balign	4

! Entry of mtx_rot_zyx_s16 that takes the three 16-bit angles through a
! pointer: loads them into the argument registers and runs straight on
! into mtx_rot_zyx_s16, which starts right after the padding nop.
! In:  r4 = pointer to three signed 16-bit angles (x, y, z)
! Out: as mtx_rot_zyx_s16, entered with r4, r5, r6 = the three angles
	.global	_mtx_rot_zyx_s16_ptr
	.type	_mtx_rot_zyx_s16_ptr,@function
_mtx_rot_zyx_s16_ptr:
	mov	r4,r5	! r5 and r6 will address the second and third angle
	mov	r4,r6
	mov.w	@r4,r4	! r4 = first angle
	add	#2,r5
	mov.w	@r5,r5	! r5 = second angle
	add	#4,r6
	mov.w	@r6,r6	! r6 = third angle
	nop		! falls into mtx_rot_zyx_s16 at 0x0C0F5AB0
	.size	_mtx_rot_zyx_s16_ptr,0x10
