#include "decoder.h"

#include <string.h>

#define OFT_NUM 5
static const uint16_t offset_word[OFT_NUM]     = {0x0FC, 0x198, 0x168, 0x1B4, 0x350};

static RdsDecoder decoder;

uint16_t syndrome_fn(uint32_t y)
{
    return ((y >> 25) & 1) * 0b1000000000 ^
           ((y >> 24) & 1) * 0b0100000000 ^
           ((y >> 23) & 1) * 0b0010000000 ^
           ((y >> 22) & 1) * 0b0001000000 ^
           ((y >> 21) & 1) * 0b0000100000 ^
           ((y >> 20) & 1) * 0b0000010000 ^
           ((y >> 19) & 1) * 0b0000001000 ^
           ((y >> 18) & 1) * 0b0000000100 ^
           ((y >> 17) & 1) * 0b0000000010 ^
           ((y >> 16) & 1) * 0b0000000001 ^
           ((y >> 15) & 1) * 0b1011011100 ^
           ((y >> 14) & 1) * 0b0101101110 ^
           ((y >> 13) & 1) * 0b0010110111 ^
           ((y >> 12) & 1) * 0b1010000111 ^
           ((y >> 11) & 1) * 0b1110011111 ^
           ((y >> 10) & 1) * 0b1100010011 ^
           ((y >>  9) & 1) * 0b1101010101 ^
           ((y >>  8) & 1) * 0b1101110110 ^
           ((y >>  7) & 1) * 0b0110111011 ^
           ((y >>  6) & 1) * 0b1000000001 ^
           ((y >>  5) & 1) * 0b1111011100 ^
           ((y >>  4) & 1) * 0b0111101110 ^
           ((y >>  3) & 1) * 0b0011110111 ^
           ((y >>  2) & 1) * 0b1010100111 ^
           ((y >>  1) & 1) * 0b1110001111 ^
           ((y >>  0) & 1) * 0b1100011011;
}

uint16_t correct(uint32_t y)
{
    // TODO: correction
    return (y >> 10);
}

bool isGoodSyndrome(uint16_t syndrome)
{
    return syndrome == 0;
}

static void handleGroup(RdsBlock *block)
{
    rdsParserDecode(block);
}

void rdsDecoderPushBit(bool data)
{
    decoder.raw <<= 1;
    decoder.raw |= (data ? 0x01 : 0x00);
    decoder.raw &= 0x03FFFFFF;

    if (!decoder.synced) {
        decoder.currIdx = 0; // Searching for block A
        decoder.syndrome = syndrome_fn(decoder.raw ^ offset_word[decoder.currIdx]);

        if (isGoodSyndrome(decoder.syndrome)) {
            // Found block A
            decoder.synced = true;
            decoder.block.blk[decoder.currIdx] = correct(decoder.raw);
            decoder.goodMask = (1 << decoder.currIdx);

            decoder.currCnt = 0;
            decoder.currIdx = 1; // Block B will be next
        } else {
            decoder.synced = false;
        }
    } else {
        if (++decoder.currCnt >= 26) {
            decoder.syndrome = syndrome_fn(decoder.raw ^ offset_word[decoder.currIdx]);

            if (isGoodSyndrome(decoder.syndrome)) {
                decoder.block.blk[decoder.currIdx] = correct(decoder.raw);
                decoder.goodMask |= (1 << decoder.currIdx);
            } else {
                decoder.synced = false;
            }

            if (decoder.goodMask == 0x0F) {
                handleGroup(&decoder.block);
            }

            decoder.currCnt = 0;
            decoder.currIdx = (decoder.currIdx + 1) % 4;
            if (decoder.currIdx == 0) {
                decoder.goodMask = 0x00;
            }
        }
    }
}

RdsDecoder *rdsDecoderGet(void)
{
    return &decoder;
}
