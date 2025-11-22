# Simple Compress Algorithm Flowchart

## Compression Flow (simple_compress)

```
                    START
                      |
                      v
              ┌───────────────┐
              │ Input: data[] │
              │    and len    │
              └───────┬───────┘
                      |
                      v
              ┌───────────────┐
              │  Validate     │
              │  All bytes    │
              │    < 128?     │
              └───────┬───────┘
                      |
            ┌─────────┴─────────┐
            v                   v
          [YES]               [NO]
            |                   |
            |                   v
            |           ┌──────────────┐
            |           │ Return -1    │
            |           │ (Error)      │
            |           └──────────────┘
            |
            v
    ┌──────────────────┐
    │ read_pos = 0     │
    │ write_pos = 0    │
    └─────────┬────────┘
              |
              v
    ┌──────────────────────┐
    │ read_pos < len?      │
    └─────────┬────────────┘
              |
        ┌─────┴─────┐
        v           v
      [NO]        [YES]
        |           |
        |           v
        |   ┌──────────────────┐
        |   │ current_byte =   │
        |   │ data[read_pos]   │
        |   └────────┬─────────┘
        |            |
        |            v
        |   ┌──────────────────┐
        |   │ Count consecutive│
        |   │ same bytes       │
        |   │ (max 127)        │
        |   └────────┬─────────┘
        |            |
        |            v
        |   ┌──────────────────┐
        |   │  count == 1?     │
        |   └────────┬─────────┘
        |            |
        |      ┌─────┴─────┐
        |      v           v
        |    [YES]       [NO]
        |      |           |
        |      v           v
        | ┌─────────┐  ┌──────────────┐
        | │ Write   │  │ Write        │
        | │ 1 byte: │  │ 2 bytes:     │
        | │ data    │  │ [0x80|count] │
        | │         │  │ [byte]       │
        | └────┬────┘  └──────┬───────┘
        |      |              |
        |      v              v
        | ┌─────────────────────┐
        | │ write_pos++         │
        | │ read_pos += count   │
        | └──────────┬──────────┘
        |            |
        |            v
        |   [Loop back to check]
        |            |
        v            |
    ┌───────────────┘
    |
    v
┌──────────────────┐
│ Return           │
│ write_pos        │
│ (compressed len) │
└──────────────────┘
         |
         v
       END
```

## Decompression Flow (simple_decompress)

```
                    START
                      |
                      v
          ┌───────────────────────┐
          │ Input: compressed[]   │
          │ Output: output[]      │
          └───────────┬───────────┘
                      |
                      v
              ┌──────────────┐
              │ read_pos = 0 │
              │ write_pos = 0│
              └──────┬───────┘
                     |
                     v
          ┌──────────────────────┐
          │ read_pos <           │
          │ compressed_len?      │
          └──────────┬───────────┘
                     |
               ┌─────┴─────┐
               v           v
             [NO]        [YES]
               |           |
               |           v
               |   ┌─────────────────┐
               |   │ byte =          │
               |   │ compressed[pos] │
               |   └────────┬────────┘
               |            |
               |            v
               |   ┌─────────────────┐
               |   │ byte & 0x80?    │
               |   │ (MSB = 1?)      │
               |   └────────┬────────┘
               |            |
               |      ┌─────┴─────┐
               |      v           v
               |  [MSB=1]      [MSB=0]
               |  (Run)        (Literal)
               |      |           |
               |      v           v
               | ┌─────────┐  ┌──────────┐
               | │Extract  │  │Copy byte │
               | │count    │  │directly  │
               | │(7 bits) │  │to output │
               | └────┬────┘  └─────┬────┘
               |      |             |
               |      v             |
               | ┌─────────┐        |
               | │Read next│        |
               | │data byte│        |
               | └────┬────┘        |
               |      |             |
               |      v             |
               | ┌─────────┐        |
               | │Write    │        |
               | │'count'  │        |
               | │times    │        |
               | └────┬────┘        |
               |      |             |
               |      v             v
               | ┌──────────────────────┐
               | │ Update read_pos,     │
               | │ write_pos            │
               | └──────────┬───────────┘
               |            |
               |            v
               |   [Loop back to check]
               |            |
               v            |
          ┌────────────────┘
          |
          v
    ┌──────────────────┐
    │ Return           │
    │ write_pos        │
    │ (decompressed    │
    │  length)         │
    └──────────────────┘
          |
          v
        END
```

## Validation Flow (simple_validate)

```
          START
            |
            v
    ┌──────────────┐
    │ Input: data[]│
    │    and len   │
    └──────┬───────┘
           |
           v
    ┌──────────────┐
    │   i = 0      │
    └──────┬───────┘
           |
           v
    ┌──────────────┐
    │   i < len?   │
    └──────┬───────┘
           |
      ┌────┴────┐
      v         v
    [NO]      [YES]
      |         |
      |         v
      |  ┌─────────────┐
      |  │ data[i]     │
      |  │  >= 128?    │
      |  └──────┬──────┘
      |         |
      |    ┌────┴────┐
      |    v         v
      |  [YES]     [NO]
      |    |         |
      |    v         v
      | ┌──────┐  ┌─────┐
      | │Print │  │ i++ │
      | │Error │  └──┬──┘
      | └───┬──┘     |
      |     |        v
      |     v    [Loop back]
      | ┌──────┐
      | │Return│
      | │  -1  │
      | └──────┘
      |
      v
  ┌──────┐
  │Return│
  │  0   │
  └──────┘
      |
      v
    END
```

## Data Format Visualization

```
Input Data (7-bit values):
┌────┬────┬────┬────┬────┬────┬────┬────┐
│ 7F │ 7F │ 7F │ 20 │ 30 │ 30 │ 30 │ 40 │
└────┴────┴────┴────┴────┴────┴────┴────┘
 0111 0111 0111 0010 0011 0011 0011 0100
 1111 1111 1111 0000 0000 0000 0000 0000
 (All MSB = 0)

                    ↓ Compress ↓

Compressed Data:
┌────┬────┬────┬────┬────┬────┐
│ 83 │ 7F │ 20 │ 83 │ 30 │ 40 │
└────┴────┴────┴────┴────┴────┘
 1000 0111 0010 1000 0011 0100
 0011 1111 0000 0011 0000 0000
  ↑           ↑           ↑
MSB=1       MSB=0       MSB=1
(Count=3)   (Data)      (Count=3)

Legend:
- MSB=1 (1xxx xxxx): RLE token, count in lower 7 bits
- MSB=0 (0xxx xxxx): Literal data byte
```

## Key Decision Points

1. **Validation:** All input bytes < 128? → Proceed or Fail
2. **Counting:** How many consecutive same bytes? → 1 or 2+
3. **Encoding:** 
   - Single byte (count=1) → Write as-is
   - Multiple bytes (count≥2) → Write [0x80|count] [byte]
4. **Decoding:**
   - MSB=0 → Copy literal
   - MSB=1 → Extract count and replicate next byte

---

**Time Complexity:** O(n) - Single pass through data
**Space Complexity:** O(1) - In-place compression
