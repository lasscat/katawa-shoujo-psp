# File formats

All integers are unsigned little-endian unless stated. `u8`, `u16`, `u32` are 1, 2 and 4 bytes.
Formats carry a version in their magic; a reader must reject a magic it does not know.

## Pack archive (`.kpk`, magic `KPK1`)

One file holding every asset and compiled script.

```
offset  size  field
0       4     magic         "KPK1"
4       4     entry_count   u32
8       4     index_offset  u32, from start of file
12      4     index_crc     u32, CRC-32 of the index bytes
16      ...   entry data, each entry padded with zero bytes to a multiple of 4
index_offset  entry_count * 16 bytes of index
```

Index entry (16 bytes), sorted by `name_hash` ascending so a reader can binary search:

```
0   4   name_hash   u32, CRC-32 of the entry name as UTF-8 bytes
4   4   offset      u32, from start of file
8   4   size        u32, in bytes, without padding
12  4   data_crc    u32, CRC-32 of the entry data
```

Entry names are lower-case paths with forward slashes, for example `bg/school_gate.img`. Names are
not stored. The pack writer fails if two names have the same hash.

Limits: at most 65,535 entries; file size below 2 GB.

## Compiled script (`.ksb`, magic `KSB1`)

Stored as pack entries named `script/<label-file>.ksb`.

```
0    4   magic          "KSB1"
4    4   code_size      u32
8    4   string_size    u32
12   4   label_count    u32
16   ... code           code_size bytes
...  ... strings        string_size bytes: UTF-8 strings, each ending in a zero byte
...  ... labels         label_count * 8 bytes: name_hash u32, code_offset u32, sorted by name_hash
```

Code is a sequence of instructions: one opcode byte followed by its operands. A string operand is a
`u32` offset into the string section. A label operand is a `u32` CRC-32 of the label name.

| Opcode | Name | Operands | Meaning |
|---|---|---|---|
| 0x00 | `END` | - | end of script; return to the main menu |
| 0x01 | `SAY` | speaker `u16`, text string | show a line and wait for the player. Speaker 0 is the narrator |
| 0x02 | `JUMP` | label | continue at label |
| 0x03 | `CALL` | label | push the next instruction address, continue at label |
| 0x04 | `RETURN` | - | pop an address and continue there; at an empty stack behaves like `END` |

Opcodes are added here first, then to the compiler and the VM in the same change.

## Save file

Not defined yet (M3).
