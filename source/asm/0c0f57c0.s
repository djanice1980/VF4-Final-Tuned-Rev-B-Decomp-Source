! 0x0c0f57c0  size 0x10  vec_to_local_into_alt
! Written by hand from the game's bytes (tools/match/to_asm.py does not take a routine
! that runs into another file's routine): hand-written assembly; not Sega's original text.

	.text
	.balign	4

! Entry of vec_to_local_into for a direction: loads the vector and the
! matrix's translation as vec_to_local_into does, but branches past the
! subtraction of the translation into its common part at 0x0C0F57F4.
! In:  r4 = vector (3 floats), r5 = out (3 floats); the current matrix in the back bank (XMTRX)
! Out: as vec_to_local_into: out = the vector in the matrix's frame
	.global	_vec_to_local_into_alt
	.type	_vec_to_local_into_alt,@function
_vec_to_local_into_alt:
	fmov	@r4+,fr4	! fr4-fr6 = the vector
	fmov	@r4+,fr5
	add	#12,r5	! the results are stored downwards from out + 12
	fmov	@r4+,fr6
	fschg	! pair moves for the translation
	fmov	fr13,fr0	! dr0 = XD12 (x, y of the translation)
	.word	0xa012	! bra _vec_to_local_into+0x14 (0x0C0F57F4), past its three fsub
	fmov	fr15,fr2	! (slot) dr2 = XD14 (z, w)
	.size	_vec_to_local_into_alt,0x10
