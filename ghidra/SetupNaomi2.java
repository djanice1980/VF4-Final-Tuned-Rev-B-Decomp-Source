// Pre-analysis setup for work/main.bin or work/test.bin (VF4 Final Tuned, Naomi 2).
// Adds the Naomi 2 address map around the loaded program and marks the
// entry point. Map from Flycast core/hw/mem/addrspace.cpp + pvr/elan.cpp.
// @category VF4FT
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.SourceType;

public class SetupNaomi2 extends GhidraScript {

	private void region(String name, long start, long size, boolean io) throws Exception {
		Memory mem = currentProgram.getMemory();
		Address a = toAddr(start);
		if (mem.getBlock(a) != null) {
			return;
		}
		MemoryBlock b = mem.createUninitializedBlock(name, a, size, false);
		b.setRead(true);
		b.setWrite(true);
		b.setExecute(false);
		b.setVolatile(io);
	}

	@Override
	protected void run() throws Exception {
		// This game is not position-independent: r12 is an ordinary callee-saved
		// register. The SuperH analyzer's default copies a caller's constant r12
		// into each callee's entry context (meant for a PIC GOT pointer); with
		// several callers the last one analyzed wins, and the decompiler then
		// treats that value as known (phantom arguments, run-to-run changes).
		setAnalysisOption(currentProgram, "SuperH4 Constant Reference Analyzer.Propagate constant R12", "false");
		Memory mem = currentProgram.getMemory();
		MemoryBlock prog = mem.getBlock(toAddr(0x0C020000L));
		prog.setName("main_prog");
		prog.setRead(true);
		prog.setWrite(true);
		prog.setExecute(true);

		// Hardware. Sizes cover the register windows, not exact decode.
		region("holly_regs",  0x005F6800L, 0x00003800L, true);  // system bus, G1/DIMM, G2, PVR core
		region("aica_regs",   0x00700000L, 0x00008000L, true);
		region("aica_ram",    0x00800000L, 0x00800000L, false); // 8 MB sound RAM
		region("vram",        0x04000000L, 0x01000000L, false); // 16 MB PowerVR2 VRAM
		region("elan_regs",   0x08000000L, 0x00010000L, true);  // Naomi 2 T&L chip
		region("elan_cmd",    0x09000000L, 0x00010000L, true);
		region("elan_ram",    0x0A000000L, 0x02000000L, false); // 32 MB
		region("ta_fifo",     0x10000000L, 0x01000000L, true);  // tile accelerator input
		region("sh4_p4",      0xFF000000L, 0x01000000L, true);  // on-chip peripherals
		// The game reaches the registers through the uncached P2 mirror
		// (0xA0000000 + physical). Unmapped, those stores looked like ordinary
		// memory, and the decompiler dropped one nothing reads afterwards (the
		// border-colour store before a fatal `for (;;)`): map the windows the
		// code uses as volatile. 0x025F6800 and 0x085F6800 are used with the
		// same register offsets as Holly's 0x005F6800 (which chip answers each
		// is not established here); 0x08800000 is read as registers too.
		region("holly_regs_p2", 0xA05F6800L, 0x00003800L, true);
		region("pvr2_regs_p2",  0xA25F6800L, 0x00003800L, true);
		region("pvr12_regs_p2", 0xA85F6800L, 0x00003800L, true);
		region("elan_8800_p2",  0xA8800000L, 0x00010000L, true);
		region("aica_regs_p2",  0xA0700000L, 0x00008000L, true);
		region("aica_rtc_p2",   0xA0710000L, 0x00000010L, true);

		// Main RAM around the program: BIOS work area below, BSS/heap above.
		region("ram_low",     0x0C000000L, 0x00020000L, false);
		// (from the program's end: 0x0C360000 for main.bin, 0x0C0A0000 for test.bin)
		long end = (prog.getEnd().getOffset() + 1 + 0xFFFF) & ~0xFFFFL;
		region("ram_high",    end, 0x0E000000L - end, false);

		Address entry = toAddr(0x0C020000L); // header 0x420: 0xAC020000 (P2 mirror)
		currentProgram.getSymbolTable().addExternalEntryPoint(entry);
		disassemble(entry);
		if (getFunctionAt(entry) == null) {
			createFunction(entry, "_start");
		} else {
			getFunctionAt(entry).setName("_start", SourceType.USER_DEFINED);
		}
	}
}
