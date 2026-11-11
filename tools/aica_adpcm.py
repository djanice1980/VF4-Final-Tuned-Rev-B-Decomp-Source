"""AICA (Yamaha) 4-bit ADPCM, as the chip's formula and Flycast's DecodeADPCM
(core/hw/aica/sgc_if.cpp) decode it.

Each byte holds two samples, low nibble first. Signal starts at 0, step at
0x7F. A nibble adds (1 - 2*bit3) * ((step * (2*(nibble & 7) + 1)) >> 3): the
magnitude is truncated before the sign is applied, so +n and -n steps are
equal. (Rounding the signed product down instead loses one unit on most
negative steps and drifts the signal downward through a wave: effects that
end in silence ended at -4,600 to -18,300.) Step is clamped to
0x7F..0x6000 and the signal to 16 bits.
"""
import numpy as np

# step multipliers (8.8 fixed point) for nibble & 7
_QUANT = np.array([230, 230, 230, 230, 307, 409, 512, 614], dtype=np.int64)
# delta magnitudes in eighths of the step for nibble & 7
_MAG = np.array([1, 3, 5, 7, 9, 11, 13, 15], dtype=np.int64)


def decode(data):
    """bytes -> int16 numpy array (two samples per byte)."""
    raw = np.frombuffer(data, dtype=np.uint8)
    nib = np.empty(raw.size * 2, dtype=np.uint8)
    nib[0::2] = raw & 0x0F
    nib[1::2] = raw >> 4
    out = np.empty(nib.size, dtype=np.int16)
    sig, step = 0, 0x7F
    mag, quant = _MAG.tolist(), _QUANT.tolist()
    for i, n in enumerate(nib.tolist()):
        d = min((step * mag[n & 7]) >> 3, 0x7FFF)
        sig = sig - d if n & 8 else sig + d
        if sig > 32767:
            sig = 32767
        elif sig < -32768:
            sig = -32768
        step = (step * quant[n & 7]) >> 8
        if step < 0x7F:
            step = 0x7F
        elif step > 0x6000:
            step = 0x6000
        out[i] = sig
    return out
