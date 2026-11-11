! 0x0c151a80  size 0x400  sq_save_stack
! Written by hand from the game's bytes (tools/match/to_asm.py takes routines,
! not data kept in .text): hand-written assembly; not Sega's original text.

	.text
	.balign	4

! sq_save_stack
!
! The buffer in which the graphics library keeps the contents of the
! store queues while it borrows them: 1 KB that its authors kept in
! .text, after elan_d0_set_for_list and its padding (source/sdk/kamui/
! elan_commands.c) and before compress2. It is used as a stack of
! 64-byte records that grows upwards: elan_layer_reset points
! sq_save_top at its start, each of the library's save sequences has
! the two 32-byte store queues written out at sq_save_top (pref on the
! store queue address of that place) and adds 64 to it, and the
! matching restore takes the 64 off again.
!
! In:    written and read through sq_save_top by the save and restore
!        code in source/sdk/kamui/ (pvr_irq.c, elan_setup.c,
!        elan_status.c); its address is taken by elan_layer_reset only.
! Out:   1024 bytes, room for 16 records; zero in the image. It is
!        written while the game runs, so this part of .text has to be
!        in RAM.
	.global	_sq_save_stack
	.type	_sq_save_stack,@object
_sq_save_stack:
	.space	1024			! 0c151a80 to 0c151e7f, all zero
	.size	_sq_save_stack,0x400
