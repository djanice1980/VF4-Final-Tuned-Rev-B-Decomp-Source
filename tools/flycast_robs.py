"""The two fighters' records in a Flycast save state: character, the frame
difference robs_frame_advantage_update keeps (rob +0x16A), the hit result,
the attack frames of the current move, the stun and the frames left
(rob_frames_left); the frame difference is what the game computes while
either fighter is in a hit or guard state: side 0's frames left minus side
1's, so a negative value is side 0's advantage (tools/flycast_state.py
reads the RAM).

    python3 -I tools/flycast_robs.py STATE work/main.bin"""
import os, struct, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import flycast_state as F

ROBS, ROB_SIZE = 0x0C37BA30, 0x86C
SIDE_CHARA = 0x0C29F9DC
CHARA_RECORDS = 0x0C1678E4  # 0x4C bytes per character (chara_data_get)
BANK_FILES = 0x0C29FFD4     # sound_bank_files


def chara_name(r, c):
    """the character's name from its own sound banks: the effect bank
    (se_ch_<name>.bin) unless that is the shared default, else the voice
    bank (vo_<name>.bin); these are the files' internal names"""
    at = lambda a: a - F.RAM_BASE
    w = lambda a: struct.unpack_from("<I", r, at(a))[0]
    h = lambda a: struct.unpack_from("<h", r, at(a))[0]
    s = lambda a: r[at(a):r.index(b"\0", at(a))].decode("latin1")
    snd = w(CHARA_RECORDS + 0x4C * c + 0x44)
    vo, se = h(snd), h(snd + 2)
    name = s(w(BANK_FILES + 4 * se))
    if name == "se_ch_default.bin" and vo >= 0:
        return s(w(BANK_FILES + 4 * vo))[3:-4]
    return name[6:-4]


def frames_left(r, b):
    """rob_frames_left (0x0C06AAC0): frames until the rob is free"""
    at = lambda a: a - F.RAM_BASE
    h = lambda o: struct.unpack_from("<h", r, at(b + o))[0]
    f = struct.unpack_from("<I", r, at(b + 0x20))[0]
    m, cur, n = h(0x12), h(10), 0
    if f & 1:
        if f & 0x1000000:
            n = max(m, h(0x2EA) - cur)
        elif f & 0x8200:
            n = max(m, h(0x2E8) - cur)
        elif struct.unpack_from("<I", r, at(b + 0x324))[0] & 0x4000 and f & 2:
            n = h(0x2E6) - cur
        else:
            n = h(0x2EA) - cur
    return max(n, 0)


def main():
    r = F.ram(sys.argv[1], sys.argv[2])
    at = lambda a: a - F.RAM_BASE
    rd = lambda fmt, a: struct.unpack_from("<" + fmt, r, at(a))[0]
    for side in range(2):
        b = ROBS + side * ROB_SIZE
        c = r[at(SIDE_CHARA) + side]
        hp = rd("I", b + 0x3C)
        res = rd("I", hp + 0x3C) if F.RAM_BASE <= hp < F.RAM_BASE + F.RAM_SIZE else None
        print("side %d: chara %d (%s)" % (side, c, chara_name(r, c)))
        print("  frame difference (+0x16A) %d" % rd("h", b + 0x16A))
        print("  state flags (+0x20) %#010x" % rd("I", b + 0x20))
        print("  move frame window: first active %d, active end %d, recovery end %d (+0x2E6/+0x2E8/+0x2EA)"
              % (rd("h", b + 0x2E6), rd("h", b + 0x2E8), rd("h", b + 0x2EA)))
        print("  stun (+0x12) %d, current frame (+0x0A) %d, frames left %d" % (rd("h", b + 0x12), rd("h", b + 10), frames_left(r, b)))
        print("  hit result word (+0x3C -> +0x3C) %s" % (hex(res) if res is not None else "pointer %#x" % hp))


if __name__ == "__main__":
    main()
