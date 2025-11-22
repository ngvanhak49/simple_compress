#include <gtest/gtest.h>

extern "C" {
    int simple_validate(uint8_t *data, int len);
    int simple_compress(uint8_t *data, int len);
    int simple_decompress(uint8_t *data, int len, uint8_t *output, int max_len);
}

// Test simple_compress function
TEST(SimpleCompressTest, CompressNullPointer) {
    EXPECT_EQ(simple_compress(nullptr, 10), 0);
}

TEST(SimpleCompressTest, CompressInvalidLength) {
    uint8_t data[] = {1, 2, 3};
    EXPECT_EQ(simple_compress(data, 0), 0);
    EXPECT_EQ(simple_compress(data, -1), 0);
}

TEST(SimpleCompressTest, CompressInvalidInput) {
    uint8_t data[] = {0, 1, 128, 100}; // Invalid: contains byte >= 128
    EXPECT_EQ(simple_compress(data, 4), -1);
}

TEST(SimpleCompressTest, CompressSingleByte) {
    uint8_t data[] = {65};
    int result = simple_compress(data, 1);
    EXPECT_EQ(result, 1);
    EXPECT_EQ(data[0], 65);
}

TEST(SimpleCompressTest, CompressNoRepeats) {
    uint8_t data[] = {1, 2, 3, 4, 5};
    int result = simple_compress(data, 5);
    EXPECT_EQ(result, 5);
    EXPECT_EQ(data[0], 1);
    EXPECT_EQ(data[1], 2);
    EXPECT_EQ(data[2], 3);
    EXPECT_EQ(data[3], 4);
    EXPECT_EQ(data[4], 5);
}

TEST(SimpleCompressTest, CompressSimpleRun) {
    uint8_t data[10] = {65, 65, 65, 65};
    int result = simple_compress(data, 4);
    EXPECT_EQ(result, 2);
    EXPECT_EQ(data[0], 0x84); // 0x80 | 4 (run of 4)
    EXPECT_EQ(data[1], 65);   // Value 'A'
}

TEST(SimpleCompressTest, CompressMixedData) {
    uint8_t data[20] = {65, 65, 65, 66, 67, 67};
    int result = simple_compress(data, 6);
    EXPECT_EQ(result, 5);
    EXPECT_EQ(data[0], 0x83); // Run of 3 'A's
    EXPECT_EQ(data[1], 65);
    EXPECT_EQ(data[2], 66);   // Single 'B'
    EXPECT_EQ(data[3], 0x82); // Run of 2 'C's
    EXPECT_EQ(data[4], 67);
}

TEST(SimpleCompressTest, CompressLongRun) {
    uint8_t data[130];
    for (int i = 0; i < 130; i++) {
        data[i] = 88; // 'X'
    }
    int result = simple_compress(data, 130);
    // Should split into runs: 127 + 3
    EXPECT_EQ(result, 4);
    EXPECT_EQ(data[0], 0xFF); // 0x80 | 127
    EXPECT_EQ(data[1], 88);
    EXPECT_EQ(data[2], 0x83); // 0x80 | 3
    EXPECT_EQ(data[3], 88);
}

// Test simple_decompress function
TEST(SimpleCompressTest, DecompressNullPointer) {
    uint8_t output[10];
    EXPECT_EQ(simple_decompress(nullptr, 10, output, 10), -1);
}

TEST(SimpleCompressTest, DecompressNullOutput) {
    uint8_t data[] = {1, 2, 3};
    EXPECT_EQ(simple_decompress(data, 3, nullptr, 10), -1);
}

TEST(SimpleCompressTest, DecompressInvalidLength) {
    uint8_t data[] = {1, 2, 3};
    uint8_t output[10];
    EXPECT_EQ(simple_decompress(data, 0, output, 10), -1);
    EXPECT_EQ(simple_decompress(data, -1, output, 10), -1);
}

TEST(SimpleCompressTest, DecompressSingleByte) {
    uint8_t data[] = {65};
    uint8_t output[10];
    int result = simple_decompress(data, 1, output, 10);
    EXPECT_EQ(result, 1);
    EXPECT_EQ(output[0], 65);
}

TEST(SimpleCompressTest, DecompressNoRuns) {
    uint8_t data[] = {1, 2, 3, 4, 5};
    uint8_t output[10];
    int result = simple_decompress(data, 5, output, 10);
    EXPECT_EQ(result, 5);
    EXPECT_EQ(output[0], 1);
    EXPECT_EQ(output[1], 2);
    EXPECT_EQ(output[2], 3);
    EXPECT_EQ(output[3], 4);
    EXPECT_EQ(output[4], 5);
}

TEST(SimpleCompressTest, DecompressSimpleRun) {
    uint8_t data[] = {0x84, 65}; // Run of 4 'A's
    uint8_t output[10];
    int result = simple_decompress(data, 2, output, 10);
    EXPECT_EQ(result, 4);
    EXPECT_EQ(output[0], 65);
    EXPECT_EQ(output[1], 65);
    EXPECT_EQ(output[2], 65);
    EXPECT_EQ(output[3], 65);
}

TEST(SimpleCompressTest, DecompressMixedData) {
    uint8_t data[] = {0x83, 65, 66, 0x82, 67}; // 3 'A's, 1 'B', 2 'C's
    uint8_t output[10];
    int result = simple_decompress(data, 5, output, 10);
    EXPECT_EQ(result, 6);
    EXPECT_EQ(output[0], 65);
    EXPECT_EQ(output[1], 65);
    EXPECT_EQ(output[2], 65);
    EXPECT_EQ(output[3], 66);
    EXPECT_EQ(output[4], 67);
    EXPECT_EQ(output[5], 67);
}

TEST(SimpleCompressTest, DecompressBufferTooSmall) {
    uint8_t data[] = {0x85, 65}; // Run of 5 'A's
    uint8_t output[3]; // Buffer only holds 3
    int result = simple_decompress(data, 2, output, 3);
    EXPECT_EQ(result, -1); // Should fail due to insufficient buffer
}

// Integration test: Compress then decompress
TEST(SimpleCompressTest, CompressDecompressRoundTrip) {
    uint8_t original[] = {65, 65, 65, 66, 67, 67, 67, 67};
    uint8_t compressed[20];
    uint8_t decompressed[20];
    
    // Copy to compressed buffer
    for (int i = 0; i < 8; i++) {
        compressed[i] = original[i];
    }
    
    // Compress
    int compressed_len = simple_compress(compressed, 8);
    EXPECT_GT(compressed_len, 0);
    EXPECT_LT(compressed_len, 8); // Should be smaller
    
    // Decompress
    int decompressed_len = simple_decompress(compressed, compressed_len, decompressed, 20);
    EXPECT_EQ(decompressed_len, 8);
    
    // Verify data matches
    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(decompressed[i], original[i]);
    }
}

TEST(SimpleCompressTest, CompressDecompressLargeData) {
    uint8_t original[200];
    uint8_t compressed[300];
    uint8_t decompressed[300];
    
    // Create pattern: 50 'A's, 50 'B's, 50 'C's, 50 'D's
    for (int i = 0; i < 50; i++) original[i] = 65;
    for (int i = 50; i < 100; i++) original[i] = 66;
    for (int i = 100; i < 150; i++) original[i] = 67;
    for (int i = 150; i < 200; i++) original[i] = 68;
    
    // Copy to compressed buffer
    for (int i = 0; i < 200; i++) {
        compressed[i] = original[i];
    }
    
    // Compress
    int compressed_len = simple_compress(compressed, 200);
    EXPECT_GT(compressed_len, 0);
    EXPECT_LT(compressed_len, 200);
    
    // Decompress
    int decompressed_len = simple_decompress(compressed, compressed_len, decompressed, 300);
    EXPECT_EQ(decompressed_len, 200);
    
    // Verify data matches
    for (int i = 0; i < 200; i++) {
        EXPECT_EQ(decompressed[i], original[i]);
    }
}
