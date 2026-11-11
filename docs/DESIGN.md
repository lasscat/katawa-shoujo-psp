# Design

## Why a new engine

Katawa Shoujo is a Ren'Py game. Ren'Py needs a full Python interpreter, OpenGL ES 2 and several
hundred megabytes of memory. A PSP has a 333 MHz MIPS CPU and 24 MB of memory available to
homebrew on the original model (52 MB on later ones). So the work is split in two:

```
 upstream game (Ren'Py scripts, 1080p art, Vorbis, VP9)
        |
        |  tools/kstool  (Python, runs on a computer)
        v
 data.kpk  (compiled script, 480x270 images, re-encoded audio)
        |
        |  src/  (C99 engine; same code on PSP and desktop)
        v
 PSP
```

Everything expensive or complicated happens in the converter. The engine only plays back what it
is given, and never parses text formats at run time.

## Facts about the input

Measured on the pinned upstream version (`versions.env`):

- Native resolution 1920x1080. A quarter of that is 480x270, so the PSP screen (480x272) is an
  exact 4:1 downscale with a one pixel border top and bottom. No cropping or letterboxing decisions.
- 31 story script files, 68,920 statements. Dialogue and narration are 47% of them, `show` 19%,
  `with` 19%. Game logic is small: about 350 `if` statements over boolean flags and a few counters,
  42 menus, about 850 one-line Python statements of which 715 are `renpy.music.set_volume(...)`.
- Character sprites are 1080 pixels tall PNGs; at quarter size a sprite is about 90x270.
- Audio is Ogg Vorbis, 44.1 kHz stereo. Video is VP9 in Matroska, which a PSP cannot decode.

## Engine (`src/`)

Portable core, no SDL and no PSP headers, unit tested on the host:

| Module | Job |
|---|---|
| `crc32` | checksums for pack and save files |
| `pack` | find and read entries in `data.kpk` |
| `vm` | run compiled script: text, flags, jumps, calls, menus; emits display and audio commands |
| `scene` | what is on screen: background, sprites with position and alpha, running transition |
| `text` | line breaking and glyph layout for a bitmap font |
| `save` | serialise and restore VM and scene state |

Front end (`src/main.c`, later `src/platform/`): SDL2 for video, input and audio on both targets.
pspdev ships SDL2 with a hardware-accelerated renderer, so one front end serves PSP and desktop.
PSP-specific code is added only where SDL is not enough (memory stick paths, power callbacks,
video playback).

Rules that follow from the hardware:

- **Memory budget 20 MB** for the whole process, so the game also runs on a PSP-1000. One
  background (480x272, 16-bit: 255 KB), a small sprite cache, one streaming music track, short
  sound effects loaded on demand.
- **No allocation in the frame loop.** Buffers are sized when a scene or script loads.
- **No floating point in file formats.** Times are milliseconds, positions are pixels, alpha and
  volume are 0-255.
- **Little-endian fixed-width integers** in every file, read byte by byte, never by casting structs.

## Converter (`tools/kstool/`)

Python 3.9+, standard library only where possible, external tools (`ffmpeg`) for media.

1. **Script compiler.** A line-oriented parser for the subset of Ren'Py script the game uses, not a
   general Ren'Py parser and not a Python interpreter. Named transitions and positions
   (`locationchange`, `twoleft`, ...) are resolved from a table in the converter to plain numbers.
   Python one-liners are matched against a fixed list of known patterns (set flag, add to counter,
   set channel volume, unlock achievement). Anything unrecognised is reported, never guessed.
2. **Coverage report.** Every statement is counted as compiled or unsupported, by kind. The total
   is the project's progress measure and is recorded in `STATUS.md`.
3. **Asset pipeline.** Images are scaled by exactly 1/4 and stored in a format the PSP decodes
   quickly; audio is re-encoded to mono or low-bitrate stereo Vorbis; fonts are baked into bitmap
   atlases for the characters the script actually uses.
4. **Pack writer.** Everything goes into one `data.kpk` so the memory stick sees one large file
   instead of thousands of small ones.

## Decisions

| # | Decision | Reason |
|---|---|---|
| D1 | Own engine instead of porting Ren'Py or reusing ONScripter | Ren'Py cannot fit; ONScripter would force the script into another engine's model and limits |
| D2 | SDL2 on both targets | one front end, desktop build behaves like the PSP build |
| D3 | Hand-written subset parser, not Ren'Py's own | script statements are simple and regular; avoids a 100 MB SDK dependency; unsupported input is counted |
| D4 | Compile scripts to bytecode on the host | no parser, no string handling and no name lookups on the PSP |
| D5 | Quarter-resolution assets, 16:9 | exact fit for the upstream art |
| D6 | Video deferred to M4 | needs a PSP-specific decoder path; the story does not depend on it |
| D7 | English first | other languages need larger glyph sets; the text layer is built on UTF-8 so they can follow |
