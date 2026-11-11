# Third-party code

The game program links several public libraries, and the repository's
reconstruction of those parts expresses their code. Two tools also carry
code from other projects. Each is listed here with its terms.

## zlib 1.1.4

`source/zlib/` (13 files) and `ghidra/types/zlib.h`. The image carries
zlib's own version strings ("deflate 1.1.4 Copyright 1995-2002 Jean-loup
Gailly", "inflate 1.1.4 Copyright 1995-2002 Mark Adler").

    Copyright (C) 1995-2002 Jean-loup Gailly and Mark Adler

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.

The files here are not zlib's original source text: they are a
reconstruction from the compiled program, written to compile to the same
bytes.

## newlib 1.8.2

`source/newlib/` (49 files): the C library routines the program takes
from newlib 1.8.2. They are a reconstruction from the compiled program,
not newlib's source text. newlib's notices for the files concerned:

**`libm/` (13 files)**

    Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.

    Developed at SunPro, a Sun Microsystems, Inc. business.
    Permission to use, copy, modify, and distribute this
    software is freely granted, provided that this notice
    is preserved.

**`libc/stdlib/div.c`, `libc/string/strtok_r.c`**

    Copyright (c) 1990 The Regents of the University of California.
    All rights reserved.

    Redistribution and use in source and binary forms are permitted
    provided that the above copyright notice and this paragraph are
    duplicated in all such forms and that any documentation,
    advertising materials, and other materials related to such
    distribution and use acknowledge that the software was developed
    by the University of California, Berkeley.  The name of the
    University may not be used to endorse or promote products derived
    from this software without specific prior written permission.
    THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

(`strtok_r.c` carries the same terms with the year 1988.)

**The other 34 files (`libc/ctype/`, `libc/stdlib/`, `libc/string/`, `libc/time/`)**

    Copyright (c) 1994, 1997 Cygnus Solutions.
    All rights reserved.

    Redistribution and use in source and binary forms are permitted
    provided that the above copyright notice and this paragraph are
    duplicated in all such forms and that any documentation,
    advertising materials, and other materials related to such
    distribution and use acknowledge that the software was developed
    at Cygnus Solutions.  Cygnus Solutions may not be used to
    endorse or promote products derived from this software without
    specific prior written permission.
    THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

The routines under `source/libc/` (the printf and scanf families, strtod,
strtol, strtoul) are not newlib's; their origin is not identified, and
they fall under `NOTICE.md` with the rest of the program.

## libgcc (GCC 3.2.3)

`source/libgcc/_fixunssfsi.c`, and the five routines of GCC's SH
`lib1funcs` among `source/asm/` (`__movstr_i4_odd`, `__movstr_i4_even`,
`__movstrSI12_i4`, `__sdivsi3_i4`, `__udivsi3_i4`). These are compiler
support routines that GCC adds to every program it builds; the files
here are a reconstruction from the compiled program. GCC's terms for
them are the GNU General Public License, version 2 or later
(`licenses/GPL-2.0.txt`), with this exception:

    In addition to the permissions in the GNU General Public License, the
    Free Software Foundation gives you unlimited permission to link the
    compiled version of this file into combinations with other programs,
    and to distribute those combinations without any restriction coming
    from the use of this file.  (The General Public License restrictions
    do apply in other respects; for example, they cover modification of
    the file, and distribution when not linked into a combine
    executable.)

    libgcc2.c: Copyright (C) 1989, 1992, 1993, 1994, 1995, 1996, 1997,
    1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
    config/sh/lib1funcs.asm: Copyright (C) 1994, 1995, 1997, 1998, 1999,
    2000, 2001 Free Software Foundation, Inc.

## MAME: the DIMM image decryption

`tools/dimm_decrypt.c` carries the DES tables and routines of MAME's
`src/mame/sega/naomigd.cpp` (copyright-holders: Olivier Galibert), with
C++ to C syntax changes only.

    BSD 3-Clause License

    Copyright (c) Olivier Galibert
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its
       contributors may be used to endorse or promote products derived from
       this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## Ghidra: the SuperH4 language files

`ghidra/lang/SuperH4_vf4ft/` is a changed copy of the SuperH4 processor
module of Ghidra (National Security Agency), under the Apache License,
Version 2.0 (`licenses/Apache-2.0.txt`). The changes are marked "vf4ft"
in the files and described in `ghidra/lang/install.sh`.

## Not in the repository

The viewer page (`tools/viewer.html`) loads three.js (MIT License) from
a public CDN when it is opened. GCC 3.2.3 and newlib 1.8.2 are
downloaded by the commands in the README into `build/`, which is not
part of the repository.
