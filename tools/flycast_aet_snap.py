"""Dump the loaded AET scene files and the AET module state from a running game,
through the GDB server of a Flycast built with -DENABLE_GDB_SERVER=ON (port 3263):

    AET_SNAP_OUT=work/aet_live/sel1 gdb -q -batch -nx -x tools/flycast_aet_snap.py

Attaches, reads the 30 aet_sets records (0x0C4ECC04, 0x90 bytes: name, +0x80
state, +0x84 loaded, +0x88 buffer, +0x8C stack entry) and the AET stack
(0x0C4EDCE4), saves every set with a buffer as OUT/NN_<name>.mem (its stack
block, relocated as the game holds it), OUT/index.tsv (set, name, state,
loaded, buffer, size) and OUT/aet_state_0c4ecc04.bin (RAM 0x0C4ECC04 ..
0x0C50E898: the sets, the stack, aet_objs, aet_stills and their caches), then
detaches. Reads only. `tools/aet.py checkdir OUT work/assets` checks them."""
import gdb, os, struct

OUT = os.environ.get("AET_SNAP_OUT", "work/aet_live/s0")
SETS, STACK = 0x0C4ECC04, 0x0C4EDCE4
STATE_LO, STATE_HI = 0x0C4ECC04, 0x0C50E898  # aet_sets .. the end of aet_cur_cache


def mem(a, n):
    return bytes(gdb.selected_inferior().read_memory(a, n))


gdb.execute("set pagination off")
gdb.execute("set confirm off")
gdb.execute("set architecture sh4")
gdb.execute("set endian little")
gdb.execute("target remote 127.0.0.1:3263")
os.makedirs(OUT, exist_ok=True)

st = mem(STACK, 8 + 30 * 16)
n, top = struct.unpack_from("<iI", st, 0)
blocks = {}
for i in range(30):
    used, s, a, sz = struct.unpack_from("<b3xiIi", st, 8 + 16 * i)
    if used:
        blocks[a] = (s, sz)
print("stack: %d blocks, top %08x" % (n, top))

rows = []
for s in range(30):
    r = mem(SETS + 0x90 * s, 0x90)
    name = r[:0x80].split(b"\0", 1)[0].decode("latin1")
    state, loaded, buf, ent = struct.unpack_from("<iiII", r, 0x80)
    size = blocks.get(buf, (None, 0))[1]
    rows.append((s, name, state, loaded, buf, size, ent))
    if buf and size:
        with open(os.path.join(OUT, "%02d_%s.mem" % (s, name or "noname")), "wb") as f:
            f.write(mem(buf, size))
    if name or buf:
        print("set %2d %-26s state %d loaded %d buf %08x size %6x entry %08x" % (s, name, state, loaded, buf, size, ent))

with open(os.path.join(OUT, "index.tsv"), "w") as f:
    f.write("set\tname\tstate\tloaded\tbuffer\tsize\n")
    for s, name, state, loaded, buf, size, ent in rows:
        f.write("%d\t%s\t%d\t%d\t%08x\t%x\n" % (s, name, state, loaded, buf, size))
with open(os.path.join(OUT, "aet_state_%08x.bin" % STATE_LO), "wb") as f:
    f.write(mem(STATE_LO, STATE_HI - STATE_LO))
gdb.execute("detach")
print("detached; wrote", OUT)
