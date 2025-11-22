# Simple Compress Algorithm Flowcharts

## Compression Algorithm (simple_compress)

```mermaid
flowchart TD
    Start([START]) --> Input[Input: data array and length]
    Input --> Validate{Validate all<br/>bytes < 128?}
    Validate -->|NO| Error[Return -1<br/>Validation Error]
    Validate -->|YES| Init[read_pos = 0<br/>write_pos = 0]
    Init --> Loop{read_pos < len?}
    Loop -->|NO| Return[Return write_pos<br/>compressed length]
    Loop -->|YES| GetByte[current_byte = data at read_pos]
    GetByte --> Count[Count consecutive<br/>same bytes<br/>max 127]
    Count --> CheckCount{count == 1?}
    CheckCount -->|YES| WriteSingle[Write 1 byte:<br/>data at write_pos]
    CheckCount -->|NO| WriteRLE[Write 2 bytes:<br/>0x80 OR count<br/>then data byte]
    WriteSingle --> Update[write_pos++<br/>read_pos += count]
    WriteRLE --> Update
    Update --> Loop
    Return --> End([END])
    Error --> End
```

## Decompression Algorithm (simple_decompress)

```mermaid
flowchart TD
    Start([START]) --> Input[Input: compressed array<br/>Output: output buffer]
    Input --> Init[read_pos = 0<br/>write_pos = 0]
    Init --> Loop{read_pos <<br/>compressed_len?}
    Loop -->|NO| Return[Return write_pos<br/>decompressed length]
    Loop -->|YES| ReadByte[byte = compressed at read_pos<br/>read_pos++]
    ReadByte --> CheckMSB{byte & 0x80?<br/>MSB = 1?}
    CheckMSB -->|YES - RLE| ExtractCount[count = byte & 0x7F<br/>Extract lower 7 bits]
    CheckMSB -->|NO - Literal| WriteLiteral[Write byte to output<br/>write_pos++]
    ExtractCount --> ReadData[data_byte = compressed at read_pos<br/>read_pos++]
    ReadData --> WriteLoop[Write data_byte<br/>count times to output<br/>write_pos += count]
    WriteLoop --> Loop
    WriteLiteral --> Loop
    Return --> End([END])
```

## Validation Algorithm (simple_validate)

```mermaid
flowchart TD
    Start([START]) --> Input[Input: data array and length]
    Input --> CheckNull{data NULL<br/>or len <= 0?}
    CheckNull -->|YES| ErrorNull[Return -1]
    CheckNull -->|NO| InitLoop[i = 0]
    InitLoop --> Loop{i < len?}
    Loop -->|NO| Success[Return 0<br/>Valid]
    Loop -->|YES| CheckByte{data at i >= 128?<br/>MSB = 1?}
    CheckByte -->|YES| PrintError[Print error:<br/>index and value]
    CheckByte -->|NO| Increment[i++]
    PrintError --> ErrorInvalid[Return -1<br/>Invalid]
    Increment --> Loop
    Success --> End([END])
    ErrorNull --> End
    ErrorInvalid --> End
```

## Complete Process Flow

```mermaid
flowchart LR
    subgraph Compression
        A[Input Data<br/>7-bit values] --> B[Validate<br/>all < 128]
        B --> C[Count<br/>consecutive]
        C --> D{Single or<br/>Multiple?}
        D -->|Single| E[Write literal]
        D -->|Multiple| F[Write RLE<br/>0x80 OR count + data]
        E --> G[Compressed Data]
        F --> G
    end
    
    subgraph Decompression
        G --> H[Read byte]
        H --> I{Check MSB}
        I -->|MSB=0| J[Copy literal]
        I -->|MSB=1| K[Extract count<br/>Replicate byte]
        J --> L[Original Data]
        K --> L
    end
```

## Data Encoding Visualization

```mermaid
graph LR
    subgraph Input[Input Data - MSB=0]
        I1[0x7F] --> I2[0x7F] --> I3[0x7F] --> I4[0x20] --> I5[0x30] --> I6[0x30] --> I7[0x30]
    end
    
    subgraph Output[Compressed - Mixed MSB]
        O1[0x83<br/>MSB=1<br/>count=3] --> O2[0x7F<br/>MSB=0<br/>data] --> O3[0x20<br/>MSB=0<br/>literal] --> O4[0x83<br/>MSB=1<br/>count=3] --> O5[0x30<br/>MSB=0<br/>data]
    end
    
    Input -->|Compress| Output
    Output -->|Decompress| Input
```

## Binary Format Diagram

```mermaid
graph TD
    subgraph RLE Token
        A[Byte with MSB=1]
        A --> B[Bit 7: 1<br/>MSB Flag]
        A --> C[Bits 6-0:<br/>Count 1-127]
    end
    
    subgraph Data Byte
        D[Byte with MSB=0]
        D --> E[Bit 7: 0<br/>MSB Clear]
        D --> F[Bits 6-0:<br/>Data Value 0-127]
    end
    
    subgraph Literal
        G[Single Occurrence]
        G --> D
    end
    
    subgraph Run
        H[Multiple Occurrences]
        H --> A
        A --> I[Followed by]
        I --> D
    end
```

---

## How to View These Diagrams

### In VS Code:
1. Install **Markdown Preview Mermaid Support** extension
2. Open this file and press `Ctrl+Shift+V` (or `Cmd+Shift+V` on Mac)
3. View rendered Mermaid diagrams

### Online:
1. Copy any diagram block
2. Paste into [Mermaid Live Editor](https://mermaid.live)
3. View and export as SVG/PNG

### In Documentation:
- GitHub automatically renders Mermaid diagrams
- GitLab supports Mermaid natively
- Many documentation tools support Mermaid

---

## Algorithm Complexity

```mermaid
graph LR
    A[Time Complexity] --> B[O n<br/>Single pass]
    C[Space Complexity] --> D[O 1<br/>In-place]
    E[Best Case] --> F[50%<br/>All repetitions]
    G[Worst Case] --> H[100%<br/>No repetitions]
```
