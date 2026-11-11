! 0x0c137298  size 0x4  irq_entry_via_firmware_alt
! Written by hand from the game's bytes (tools/match/to_asm.py does not take a routine
! that runs into another file's routine): hand-written assembly; not Sega's original text.

	.text
	.balign	4

! A second way into irq_entry_via_firmware, placed two instructions
! before it: branches to 0x0C1372B6 inside it, past its firmware call.
! Nothing in the image reaches this entry.
! In:  the interrupted context, as irq_entry_via_firmware expects it after its first part
! Out: does not return here: continues in irq_entry_via_firmware
	.global	_irq_entry_via_firmware_alt
	.type	_irq_entry_via_firmware_alt,@function
_irq_entry_via_firmware_alt:
	.word	0xa00d	! bra _irq_entry_via_firmware+0x1a (0x0C1372B6)
	nop
	.size	_irq_entry_via_firmware_alt,0x4
