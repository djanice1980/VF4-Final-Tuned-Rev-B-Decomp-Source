# Notice

This is a preservation and study project. It is not affiliated with,
endorsed by or sponsored by Sega. *Virtua Fighter* is a trademark of
Sega; it is used here only to say which program the project studies.

## What is in the repository, and under what terms

| Material | Where | Terms |
|----------|-------|-------|
| The project's own work: tools, scripts, Ghidra scripts, documents, the status page; the names, comments, descriptions, type headers and tables the project wrote | `tools/`, `ghidra/`, `docs/`, `progress/`, `README.md`, and the annotations in the source trees | MIT License (`LICENSE`) |
| The reconstructed source of the game program | `source/`, `source_test/`, `src/`, `src_test/` | see below |
| Code from other projects | listed in `THIRD-PARTY.md` | each project's own terms |

## The reconstructed source

The C and assembly under `source/`, `source_test/`, `src/` and `src_test/`
was written by studying the program on the game's GD-ROM, so that it
compiles to that program's exact bytes. The program it expresses is the
work of its owners: *Virtua Fighter 4: Final Tuned* is © Sega, and the
libraries linked into it carry the notices the image itself holds (Sega
Enterprises, WOW Entertainment).

The project claims no rights over that program and cannot grant any. The
MIT License in `LICENSE` covers what the project added to those files
(names, comments, descriptions, the arrangement into files); it is not a
license to the game. The reconstruction is published for study,
preservation and interoperability.

The repository holds no file of the game: no binary, no asset, no SDK
file. Nothing here runs or builds the game without your own dump of the
disc, and the tools check that dump against its published hash before
using it.

## If you are a rights holder

Open an issue on the repository, or write to the repository owner
through their GitHub profile, and say what concerns you.

This notice describes the project's position. It is not legal advice.
