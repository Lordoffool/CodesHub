#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BLOCKS 4
#define BITS_PER_BLOCK 4
#define TOTAL_BLOCKS 5

typedef struct {
    int data[TOTAL_BLOCKS][BITS_PER_BLOCK];
    int checksum[BITS_PER_BLOCK];
} DataPacket;

// ==================== UTILITY FUNCTIONS ====================

void print_packet(DataPacket *packet, int is_with_checksum) {
    printf("\nData Blocks:\n");
    for (int i = 0; i < (is_with_checksum ? TOTAL_BLOCKS : BLOCKS); i++) {
        printf("  Block %d: ", i + 1);
        for (int j = 0; j < BITS_PER_BLOCK; j++) {
            printf("%d", packet->data[i][j]);
        }
        printf("\n");
    }
}

// One's Complement Addition (Binary addition with carry wraparound)
void ones_complement_add(int *block, int *result, int *carry) {
    *carry = 0;
    
    for (int j = BITS_PER_BLOCK - 1; j >= 0; j--) {
        int sum = block[j] + result[j] + *carry;
        result[j] = sum % 2;
        *carry = sum / 2;
    }
    
    // Add carry to least significant bit (One's complement)
    if (*carry) {
        result[BITS_PER_BLOCK - 1] += 1;
        if (result[BITS_PER_BLOCK - 1] > 1) {
            result[BITS_PER_BLOCK - 1] = 0;
            *carry = 1;
            for (int j = BITS_PER_BLOCK - 2; j >= 0; j--) {
                result[j] += *carry;
                if (result[j] > 1) {
                    result[j] = 0;
                    *carry = 1;
                } else {
                    *carry = 0;
                    break;
                }
            }
        }
    }
}

// Calculate checksum from blocks
void calculate_checksum(int blocks[][BITS_PER_BLOCK], int *checksum) {
    memset(checksum, 0, BITS_PER_BLOCK * sizeof(int));
    int carry = 0;
    
    for (int i = 0; i < BLOCKS; i++) {
        ones_complement_add(blocks[i], checksum, &carry);
    }
    
    // Invert bits for checksum (One's complement)
    for (int i = 0; i < BITS_PER_BLOCK; i++) {
        checksum[i] = (checksum[i] == 0) ? 1 : 0;
    }
}

// ==================== SENDER FUNCTION ====================

void sender() {
    DataPacket packet;
    memset(&packet, 0, sizeof(DataPacket));
    
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║         SENDER MODE (Checksum)         ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    printf("Enter a 16-bit binary data in blocks of 4 bits:\n");
    printf("(Each bit should be 0 or 1)\n\n");
    
    for (int i = 0; i < BLOCKS; i++) {
        int valid = 0;
        while (!valid) {
            printf("Enter Block %d (4 bits): ", i + 1);
            valid = 1;
            
            for (int j = 0; j < BITS_PER_BLOCK; j++) {
                int bit;
                if (scanf("%1d", &bit) != 1 || (bit != 0 && bit != 1)) {
                    printf("❌ Invalid input! Please enter binary digits (0 or 1)\n");
                    while (getchar() != '\n');
                    valid = 0;
                    break;
                }
                packet.data[i][j] = bit;
            }
            while (getchar() != '\n');
        }
    }
    
    // Calculate and display checksum
    calculate_checksum(packet.data, packet.checksum);
    
    printf("\n✓ Data received successfully!\n");
    print_packet(&packet, 0);
    
    printf("\nCalculated Checksum: ");
    for (int i = 0; i < BITS_PER_BLOCK; i++) {
        printf("%d", packet.checksum[i]);
    }
    printf("\n");
    
    printf("\n═══════════════════════════════════════\n");
    printf("📦 Complete Data Block to Send:\n");
    printf("═══════════════════════════════════════\n");
    for (int i = 0; i < BLOCKS; i++) {
        for (int j = 0; j < BITS_PER_BLOCK; j++) {
            printf("%d", packet.data[i][j]);
        }
        printf(" ");
    }
    for (int i = 0; i < BITS_PER_BLOCK; i++) {
        printf("%d", packet.checksum[i]);
    }
    printf("\n═══════════════════════════════════════\n\n");
}

// ==================== RECEIVER FUNCTION ====================

void receiver() {
    DataPacket packet;
    memset(&packet, 0, sizeof(DataPacket));
    
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║       RECEIVER MODE (Validation)       ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    printf("Enter the complete data block (5 blocks of 4 bits each):\n");
    printf("(Include the checksum block received from sender)\n\n");
    
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        int valid = 0;
        while (!valid) {
            printf("Enter Block %d (4 bits): ", i + 1);
            valid = 1;
            
            for (int j = 0; j < BITS_PER_BLOCK; j++) {
                int bit;
                if (scanf("%1d", &bit) != 1 || (bit != 0 && bit != 1)) {
                    printf("❌ Invalid input! Please enter binary digits (0 or 1)\n");
                    while (getchar() != '\n');
                    valid = 0;
                    break;
                }
                packet.data[i][j] = bit;
            }
            while (getchar() != '\n');
        }
    }
    
    printf("\n✓ Data received successfully!\n");
    print_packet(&packet, 1);
    
    // Verify checksum
    int temp_data[BLOCKS][BITS_PER_BLOCK];
    for (int i = 0; i < BLOCKS; i++) {
        memcpy(temp_data[i], packet.data[i], BITS_PER_BLOCK * sizeof(int));
    }
    
    int calculated_checksum[BITS_PER_BLOCK];
    calculate_checksum(temp_data, calculated_checksum);
    
    printf("\nCalculated Checksum: ");
    for (int i = 0; i < BITS_PER_BLOCK; i++) {
        printf("%d", calculated_checksum[i]);
    }
    printf("\n");
    
    printf("Received Checksum:   ");
    for (int i = 0; i < BITS_PER_BLOCK; i++) {
        printf("%d", packet.data[BLOCKS][i]);
    }
    printf("\n");
    
    // Compare checksums
    int is_valid = 1;
    for (int i = 0; i < BITS_PER_BLOCK; i++) {
        if (calculated_checksum[i] != packet.data[BLOCKS][i]) {
            is_valid = 0;
            break;
        }
    }
    
    printf("\n═══════════════════════════════════════\n");
    if (is_valid) {
        printf("✅ Data Block VALID - No errors detected!\n");
    } else {
        printf("❌ Data Block INVALID - Error detected!\n");
        printf("⚠️  Data may have been corrupted during transmission.\n");
    }
    printf("═══════════════════════════════════════\n\n");
}

// ==================== MAIN MENU ====================

int main() {
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║    🔐 NETWORK CHECKSUM VALIDATOR 🔐    ║\n");
    printf("║   (One's Complement Error Detection)   ║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    printf("\nSelect Mode:\n");
    printf("  1. Sender (Generate Checksum)\n");
    printf("  2. Receiver (Validate Data)\n");
    printf("  3. Exit\n\n");
    
    int choice;
    printf("Enter your choice (1-3): ");
    
    if (scanf("%d", &choice) != 1) {
        printf("❌ Invalid input!\n");
        return 1;
    }
    
    switch (choice) {
        case 1:
            sender();
            break;
        case 2:
            receiver();
            break;
        case 3:
            printf("\nGoodbye! 👋\n\n");
            exit(0);
        default:
            printf("❌ Invalid choice! Please enter 1, 2, or 3.\n\n");
            return 1;
    }
    
    return 0;
}
