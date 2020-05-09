#ifndef LC7213X_REGS_H
#define LC7213X_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

// IF = 10.7MHz and step = 50kHz
#define LC72131_IF          1070
#define LC72131_RF          5 // 25kHz * 2 (for FMIN)

// I/O mode address
#define LC72131_IO_IN1      0x28
#define LC72131_IO_IN2      0x29
#define LC72131_IO_OUT      0x2A

// IN1 mode control word bytes
#define LC72131_IN1_SNS     (1<<0)  // 1 - 2..40MHz, 0 - 0.5..10MHz (when AM input DVS)
#define LC72131_IN1_DVS     (1<<1)  // 1 - FM input, 0 - AM input
#define LC72131_IN1_CTE     (1<<2)  // 1 - Counter start, 0 - Counter reset
#define LC72131_IN1_XS      (1<<3)  // 1 - 7.2MHz crystal, 0 - 4.5MHz crystal
#define LC72131_IN1_R0      (1<<4)  // Reference freq selection
#define LC72131_IN1_R1      (1<<5)  // Reference freq selection
#define LC72131_IN1_R2      (1<<6)  // Reference freq selection
#define LC72131_IN1_R3      (1<<7)  // Reference freq selection

// IN2 mode control word1 bytes
#define LC72131_IN2_IOC1    (1<<0)  // 0 - Input mode, 1 - output mode on IO1 (H-MS1110: 0 - input from AGC)
#define LC72131_IN2_IOC2    (1<<1)  // 0 - Input mode, 1 - output mode on IO2 (H-MS1110: 0 - stereo indicator)
#define LC72131_IN2_IO1     (1<<2)  // IO1 output value (H-MS1110: not used due to IOC1=0)
#define LC72131_IN2_IO2     (1<<3)  // IO2 output value (H-MS1110: not used due to IOC2=0)
#define LC72131_IN2_BO1     (1<<4)  // BO1 output value (H-MS1110: LA1823 FM/AM: 1 - FM mode, 0 - AM mode)
#define LC72131_IN2_BO2     (1<<5)  // BO2 output value (H-MS1110: LA1823 IF BUFF: 1 - mute, 0 - unmute)
#define LC72131_IN2_BO3     (1<<6)  // BO3 output value (H-MS1110: not connected)
#define LC72131_IN2_BO4     (1<<7)  // BO4 output value (H-MS1110: not connected)

// IN2 mode control word2 bytes
#define LC72131_IN2_BO5     (1<<0)  // BO5 output value (H-MS1110: LA1823 forced mono: 1- stereo, 2 - forced mono)
#define LC72131_IN2_DOC0    (1<<1)  // DO putput    001 - low when ulock state, 010 - endUC
#define LC72131_IN2_DOC1    (1<<2)  // DO output    101 - IO1 pin state, 110 - IO2 pin state
#define LC72131_IN2_DOC2    (1<<3)  // DO output    other - Open
#define LC72131_IN2_UL0     (1<<4)  // Unlock detection data
#define LC72131_IN2_UL1     (1<<5)  // Unlock detection data
#define LC72131_IN2_DZ0     (1<<6)  // Phase comparator dead zone
#define LC72131_IN2_DZ1     (1<<7)  // Phase comparator dead one

// IN2 mode control word3 bytes
#define LC72131_IN2_GT0     (1<<0)  // Counter measurement period   00 - 4/3..4ms, 01 - 8/3..4ms
#define LC72131_IN2_GT1     (1<<1)  // Counter measurement period   10 - 32/7..8ms, 11 - 64/7..8ms
#define LC72131_IN2_TBC     (1<<2)  // Clock time base - 0 - off, 1 - 8Hz on BO1
#define LC72131_IN2_DLC     (1<<3)  // Charge pump output - 0 - normal, 1 - forced low
#define LC72131_IN2_IFS     (1<<4)  // If counter control: 1 - normal mode, 0 - degradation mode
#define LC72131_IN2_TEST0   (1<<5)  // Should be 0
#define LC72131_IN2_TEST1   (1<<6)  // Should be 0
#define LC72131_IN2_TEST2   (1<<7)  // Should be 0

#ifdef __cplusplus
}
#endif

#endif // LC7213X_REGS_H
