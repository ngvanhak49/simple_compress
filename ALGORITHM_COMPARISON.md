# Algorithm Comparison

## Simple Compress vs RLE vs PackBits

### Simple Compress (This Implementation)
**Format:** `[count_with_MSB | data]`
- **Encoding:** MSB=1 for run, MSB=0 for literal data
- **Run format:** `[0x80|count] [byte]` (2 bytes for 2-127 repeats)
- **Literal format:** `[byte]` (1 byte)
- **Constraint:** Input must be < 128 (MSB=0)
- **Max run length:** 127

**Example:**
```
Input:  [0x7F × 5] [0x20] [0x30 × 3]
Output: [0x85 0x7F] [0x20] [0x83 0x30]
        (2 bytes)   (1 byte) (2 bytes)
```

**Pros:**
- Simple single-bit flag (MSB)
- In-place compression
- No escape sequences needed
- Efficient for mixed data

**Cons:**
- Limited to 7-bit input data
- Single-byte literals not compressed

---

### Classic RLE (Run-Length Encoding)
**Format:** `[count] [data] [count] [data]...`
- **Encoding:** Always pairs of count + data
- **No distinction** between runs and literals
- **All data encoded** as count+value pairs

**Example:**
```
Input:  [0xAA × 5] [0x20] [0xBB × 3]
Output: [0x05 0xAA] [0x01 0x20] [0x03 0xBB]
        (2 bytes)   (2 bytes)   (2 bytes)
```

**Pros:**
- Simple algorithm
- Works with any byte values
- Predictable output size

**Cons:**
- Always 2× size for non-repeating data
- Inefficient for single bytes
- Poor compression ratio for varied data

---

### PackBits (Apple/TIFF format)
**Format:** `[header] [data...]`
- **Header byte:**
  - `0-127`: Copy next (n+1) literal bytes
  - `128`: No-op
  - `129-255`: Repeat next byte (257-n) times

**Example:**
```
Input:  [0xAA × 5] [0x20] [0xBB × 3]
Output: [0xFC 0xAA] [0x00 0x20] [0xFE 0xBB]
        (2 bytes)   (2 bytes)   (2 bytes)

Input:  [0x11 0x22 0x33 0x44 0x55]
Output: [0x04 0x11 0x22 0x33 0x44 0x55]
        (6 bytes - includes all literals)
```

**Pros:**
- Works with any byte values
- Efficient for literal sequences
- Industry standard (TIFF, PCX, etc.)
- Good balance for real-world data

**Cons:**
- More complex encoding logic
- Header calculation overhead
- Max run length: 128

---

## Performance Comparison

| Algorithm | Best Case | Worst Case | Average | Constraints |
|-----------|-----------|------------|---------|-------------|
| **Simple Compress** | 50% (long runs) | 100% (no runs) | 70-90% | Input < 128 |
| **Classic RLE** | 50% (long runs) | 200% (all singles) | 100-150% | None |
| **PackBits** | 50% (long runs) | 101% (worst pattern) | 80-100% | None |

---

## Use Cases

### Simple Compress
- 7-bit ASCII text with repetitions
- Sensor data with limited range
- Pre-processed data streams
- Memory-constrained environments

### Classic RLE
- Simple repetitive data (fax, simple graphics)
- Educational purposes
- Legacy systems

### PackBits
- Image compression (TIFF, BMP)
- Mixed text and binary data
- General-purpose compression
- Production systems

---

## Summary

**Simple Compress** trades input range (7-bit only) for simplicity and efficiency. It's ideal when data is known to be < 128 and you need fast, in-place compression.

**Classic RLE** is the simplest but least efficient for real-world data due to poor handling of non-repeating sequences.

**PackBits** is the most versatile and balanced, widely used in industry for its good compression across different data patterns.
