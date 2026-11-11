! 0x0c12d4c0  size 0x8  mtx_rot_x_rad
! Written by hand from the game's bytes (tools/match/to_asm.py does not take a routine
! that runs into another file's routine): hand-written assembly; not Sega's original text.

	.text
	.balign	4

! Rotates the current matrix about x by an angle in radians: loads the
! branch offset of mtx_rot_x_sincos and joins the tail that mtx_rot_y_rad
! and mtx_rot_z_rad share (0x0C12D4E8), which turns the radians into a
! 16-bit angle, takes sine and cosine with fsca and branches on by r1.
! In:  fr4 = angle in radians; the current matrix in XMTRX
! Out: as mtx_rot_x_sincos with the angle's sine and cosine
	.global	_mtx_rot_x_rad
	.type	_mtx_rot_x_rad,@function
_mtx_rot_x_rad:
	mov.w	.L0c12d4c6,r1	! r1 = the braf offset that reaches mtx_rot_x_sincos
	.word	0xa011	! bra _mtx_rot_z_rad+0x8 (0x0C12D4E8), the shared tail
	nop
.L0c12d4c6:
	.word	0x000c	! 0x0C12D500 - 0x0C12D4F4: from the tail's braf to mtx_rot_x_sincos
	.size	_mtx_rot_x_rad,0x8
