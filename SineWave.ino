/*
** Sine Wave Generator for Arduino
**
** Description:
**   This program uses Timer/Counter2 Compare Register OCR2A in
**   Fast-PWM mode which outputs on OC2A/PB3/digital pin 11. The duty
**   cycle is changed at each TOV2 interrupt. This is done by setting
**   the value of OCR2A inside the ISR(TIMER2_OVF_vect) function. From
**   the datasheet, this value is double buffered and any update,
**   while in Fast-PWM mode, affects the compare register at BOTTOM
**
**   All values for the sine wave are calcuated in advance and stored
**   in RAM. Testing of storage of the array in PROGMEM was not done
**   over concerns that the access time may be too slow for an
**   interrupt to read the data during one timer cycle.
**
**   The selection of a 100µF capacitor was aribitrary. The capacitor
**   value and PRESCALER selection affect the smoothness and amplitude
**   of the output wave. The higher that capacitor, the smoother the
**   output wave, but also the lower the amplitude.
**
** Set up:
**   o Select PRESCALER and SAMPLE_COUNT values based on the
**     desired frequency sine wave frequency.
**   o Connect ground from arduino to breadboard ground rail.
**   o Connect digital pin 11 to a breadboard. 
**   o Add a 100µF capacitor between pin 11 on the breadboard and
**     ground.
**   o Connect an oscilloscope probe to the output pin 11 on the
**     positive side of the capacitor.
**   o Connect occilloscope probe ground lead to ground on either the
**     arduino or breadboard.
**   o Compile/Upload the program to the arduino.
**
** Keywords: Fast-PWM, Timer/Counter1, OCR2A, Sine Wave, TOV2
**
** Resultant Sine wave frequency:
**   F_CPU / (<Prescaler Factor> * 2^<Timer Size> * <Sample Count>)
**   Example:
**     16 Mhz / (32 * 2^8 * 512)
**     16 MHz / (32 * 256 * 512)
**     16 MHz / 4194304  = 3.81 Hz
**
** Sine Wave Frequencies with prescaler and sample counts*
**              Frequency at Sample Count  
** Prescaler:    [256]     [512]     [1024]
**      1      244.14 Hz 122.07 Hz  61.03 Hz
**      8       30.52 Hz  15.25 Hz   7.63 Hz
**     32        7.63 Hz   3.81 Hz   1.91 Hz
**     64        3.81 Hz   1.91 Hz   0.95 Hz
**    128        1.91 Hz   0.95 Hz   0.48 Hz
**    256        0.95 Hz   0.48 Hz   0.24 Hz
**   1024        0.24 Hz   0.12 Hz   0.06 Hz
**
** * - Note that this table assumes an 8-bit timer.
**
** Copyright 2023 David Herbert
**
** This program is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as
** published by the Free Software Foundation, either version 3 of the
** License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see
** <http://www.gnu.org/licenses/>.
**
*/

/* Valid values for PRESCALER are: 1, 8, 32, 64, 128, 256, and 1024 */
#define PRESCALER 1

/* SAMPLE_COUNT should be power of 2 for the rollover mask in the interrupt */

/* Sine Wave precalculated data from "python3 sine_array.py"*/
/* Valid values for SAMPLE_COUNT are: 256, 512, or 1024 */

#define SAMPLE_COUNT 256

#if SAMPLE_COUNT == 256

const uint8_t SINE_FULL_WAVE[SAMPLE_COUNT] =
  {
    128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 162, 165, 167,
    170, 173, 176, 179, 182, 185, 188, 190, 193, 196, 198, 201, 203, 206,
    208, 211, 213, 215, 218, 220, 222, 224, 226, 228, 230, 232, 234, 235,
    237, 238, 240, 241, 243, 244, 245, 246, 248, 249, 250, 250, 251, 252,
    253, 253, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 254, 254,
    254, 253, 253, 252, 251, 250, 250, 249, 248, 246, 245, 244, 243, 241,
    240, 238, 237, 235, 234, 232, 230, 228, 226, 224, 222, 220, 218, 215,
    213, 211, 208, 206, 203, 201, 198, 196, 193, 190, 188, 185, 182, 179,
    176, 173, 170, 167, 165, 162, 158, 155, 152, 149, 146, 143, 140, 137,
    134, 131, 128, 124, 121, 118, 115, 112, 109, 106, 103, 100,  97,  93,
     90,  88,  85,  82,  79,  76,  73,  70,  67,  65,  62,  59,  57,  54,
     52,  49,  47,  44,  42,  40,  37,  35,  33,  31,  29,  27,  25,  23,
     21,  20,  18,  17,  15,  14,  12,  11,  10,   9,   7,   6,   5,   5,
      4,   3,   2,   2,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,
      1,   1,   1,   2,   2,   3,   4,   5,   5,   6,   7,   9,  10,  11,
     12,  14,  15,  17,  18,  20,  21,  23,  25,  27,  29,  31,  33,  35,
     37,  40,  42,  44,  47,  49,  52,  54,  57,  59,  62,  65,  67,  70,
     73,  76,  79,  82,  85,  88,  90,  93,  97, 100, 103, 106, 109, 112,
    115, 118, 121, 124
  };

#elif SAMPLE_COUNT == 512

const uint8_t SINE_FULL_WAVE[SAMPLE_COUNT] =
  {
    128, 129, 131, 132, 134, 135, 137, 138, 140, 142, 143, 145, 146, 148,
    149, 151, 152, 154, 155, 157, 158, 160, 162, 163, 165, 166, 167, 169,
    170, 172, 173, 175, 176, 178, 179, 181, 182, 183, 185, 186, 188, 189,
    190, 192, 193, 194, 196, 197, 198, 200, 201, 202, 203, 205, 206, 207,
    208, 210, 211, 212, 213, 214, 215, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 234, 235, 236,
    237, 238, 238, 239, 240, 241, 241, 242, 243, 243, 244, 245, 245, 246,
    246, 247, 248, 248, 249, 249, 250, 250, 250, 251, 251, 252, 252, 252,
    253, 253, 253, 253, 254, 254, 254, 254, 254, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 254,
    254, 253, 253, 253, 253, 252, 252, 252, 251, 251, 250, 250, 250, 249,
    249, 248, 248, 247, 246, 246, 245, 245, 244, 243, 243, 242, 241, 241,
    240, 239, 238, 238, 237, 236, 235, 234, 234, 233, 232, 231, 230, 229,
    228, 227, 226, 225, 224, 223, 222, 221, 220, 219, 218, 217, 215, 214,
    213, 212, 211, 210, 208, 207, 206, 205, 203, 202, 201, 200, 198, 197,
    196, 194, 193, 192, 190, 189, 188, 186, 185, 183, 182, 181, 179, 178,
    176, 175, 173, 172, 170, 169, 167, 166, 165, 163, 162, 160, 158, 157,
    155, 154, 152, 151, 149, 148, 146, 145, 143, 142, 140, 138, 137, 135,
    134, 132, 131, 129, 128, 126, 124, 123, 121, 120, 118, 117, 115, 113,
    112, 110, 109, 107, 106, 104, 103, 101, 100,  98,  97,  95,  93,  92,
     90,  89,  88,  86,  85,  83,  82,  80,  79,  77,  76,  74,  73,  72,
     70,  69,  67,  66,  65,  63,  62,  61,  59,  58,  57,  55,  54,  53,
     52,  50,  49,  48,  47,  45,  44,  43,  42,  41,  40,  38,  37,  36,
     35,  34,  33,  32,  31,  30,  29,  28,  27,  26,  25,  24,  23,  22,
     21,  21,  20,  19,  18,  17,  17,  16,  15,  14,  14,  13,  12,  12,
     11,  10,  10,   9,   9,   8,   7,   7,   6,   6,   5,   5,   5,   4,
      4,   3,   3,   3,   2,   2,   2,   2,   1,   1,   1,   1,   1,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      1,   1,   1,   1,   1,   2,   2,   2,   2,   3,   3,   3,   4,   4,
      5,   5,   5,   6,   6,   7,   7,   8,   9,   9,  10,  10,  11,  12,
     12,  13,  14,  14,  15,  16,  17,  17,  18,  19,  20,  21,  21,  22,
     23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
     37,  38,  40,  41,  42,  43,  44,  45,  47,  48,  49,  50,  52,  53,
     54,  55,  57,  58,  59,  61,  62,  63,  65,  66,  67,  69,  70,  72,
     73,  74,  76,  77,  79,  80,  82,  83,  85,  86,  88,  89,  90,  92,
     93,  95,  97,  98, 100, 101, 103, 104, 106, 107, 109, 110, 112, 113,
    115, 117, 118, 120, 121, 123, 124, 126
  };

#elif SAMPLE_COUNT == 1024

const uint8_t SINE_FULL_WAVE[SAMPLE_COUNT] =
  {
    128, 128, 129, 130, 131, 131, 132, 133, 134, 135, 135, 136, 137, 138,
    138, 139, 140, 141, 142, 142, 143, 144, 145, 145, 146, 147, 148, 149,
    149, 150, 151, 152, 152, 153, 154, 155, 155, 156, 157, 158, 158, 159,
    160, 161, 162, 162, 163, 164, 165, 165, 166, 167, 167, 168, 169, 170,
    170, 171, 172, 173, 173, 174, 175, 176, 176, 177, 178, 178, 179, 180,
    181, 181, 182, 183, 183, 184, 185, 186, 186, 187, 188, 188, 189, 190,
    190, 191, 192, 192, 193, 194, 194, 195, 196, 196, 197, 198, 198, 199,
    200, 200, 201, 202, 202, 203, 203, 204, 205, 205, 206, 207, 207, 208,
    208, 209, 210, 210, 211, 211, 212, 213, 213, 214, 214, 215, 215, 216,
    217, 217, 218, 218, 219, 219, 220, 220, 221, 221, 222, 222, 223, 224,
    224, 225, 225, 226, 226, 227, 227, 228, 228, 228, 229, 229, 230, 230,
    231, 231, 232, 232, 233, 233, 234, 234, 234, 235, 235, 236, 236, 236,
    237, 237, 238, 238, 238, 239, 239, 240, 240, 240, 241, 241, 241, 242,
    242, 242, 243, 243, 243, 244, 244, 244, 245, 245, 245, 246, 246, 246,
    246, 247, 247, 247, 248, 248, 248, 248, 249, 249, 249, 249, 250, 250,
    250, 250, 250, 251, 251, 251, 251, 251, 252, 252, 252, 252, 252, 252,
    253, 253, 253, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254, 254,
    254, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 254, 254, 254, 254, 254, 254, 254, 254, 254,
    254, 254, 253, 253, 253, 253, 253, 253, 253, 252, 252, 252, 252, 252,
    252, 251, 251, 251, 251, 251, 250, 250, 250, 250, 250, 249, 249, 249,
    249, 248, 248, 248, 248, 247, 247, 247, 246, 246, 246, 246, 245, 245,
    245, 244, 244, 244, 243, 243, 243, 242, 242, 242, 241, 241, 241, 240,
    240, 240, 239, 239, 238, 238, 238, 237, 237, 236, 236, 236, 235, 235,
    234, 234, 234, 233, 233, 232, 232, 231, 231, 230, 230, 229, 229, 228,
    228, 228, 227, 227, 226, 226, 225, 225, 224, 224, 223, 222, 222, 221,
    221, 220, 220, 219, 219, 218, 218, 217, 217, 216, 215, 215, 214, 214,
    213, 213, 212, 211, 211, 210, 210, 209, 208, 208, 207, 207, 206, 205,
    205, 204, 203, 203, 202, 202, 201, 200, 200, 199, 198, 198, 197, 196,
    196, 195, 194, 194, 193, 192, 192, 191, 190, 190, 189, 188, 188, 187,
    186, 186, 185, 184, 183, 183, 182, 181, 181, 180, 179, 178, 178, 177,
    176, 176, 175, 174, 173, 173, 172, 171, 170, 170, 169, 168, 167, 167,
    166, 165, 165, 164, 163, 162, 162, 161, 160, 159, 158, 158, 157, 156,
    155, 155, 154, 153, 152, 152, 151, 150, 149, 149, 148, 147, 146, 145,
    145, 144, 143, 142, 142, 141, 140, 139, 138, 138, 137, 136, 135, 135,
    134, 133, 132, 131, 131, 130, 129, 128, 128, 127, 126, 125, 124, 124,
    123, 122, 121, 120, 120, 119, 118, 117, 117, 116, 115, 114, 113, 113,
    112, 111, 110, 110, 109, 108, 107, 106, 106, 105, 104, 103, 103, 102,
    101, 100, 100,  99,  98,  97,  97,  96,  95,  94,  93,  93,  92,  91,
     90,  90,  89,  88,  88,  87,  86,  85,  85,  84,  83,  82,  82,  81,
     80,  79,  79,  78,  77,  77,  76,  75,  74,  74,  73,  72,  72,  71,
     70,  69,  69,  68,  67,  67,  66,  65,  65,  64,  63,  63,  62,  61,
     61,  60,  59,  59,  58,  57,  57,  56,  55,  55,  54,  53,  53,  52,
     52,  51,  50,  50,  49,  48,  48,  47,  47,  46,  45,  45,  44,  44,
     43,  42,  42,  41,  41,  40,  40,  39,  38,  38,  37,  37,  36,  36,
     35,  35,  34,  34,  33,  33,  32,  31,  31,  30,  30,  29,  29,  28,
     28,  27,  27,  27,  26,  26,  25,  25,  24,  24,  23,  23,  22,  22,
     21,  21,  21,  20,  20,  19,  19,  19,  18,  18,  17,  17,  17,  16,
     16,  15,  15,  15,  14,  14,  14,  13,  13,  13,  12,  12,  12,  11,
     11,  11,  10,  10,  10,   9,   9,   9,   9,   8,   8,   8,   7,   7,
      7,   7,   6,   6,   6,   6,   5,   5,   5,   5,   5,   4,   4,   4,
      4,   4,   3,   3,   3,   3,   3,   3,   2,   2,   2,   2,   2,   2,
      2,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,
      1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,
      2,   2,   2,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,
      5,   5,   5,   5,   5,   6,   6,   6,   6,   7,   7,   7,   7,   8,
      8,   8,   9,   9,   9,   9,  10,  10,  10,  11,  11,  11,  12,  12,
     12,  13,  13,  13,  14,  14,  14,  15,  15,  15,  16,  16,  17,  17,
     17,  18,  18,  19,  19,  19,  20,  20,  21,  21,  21,  22,  22,  23,
     23,  24,  24,  25,  25,  26,  26,  27,  27,  27,  28,  28,  29,  29,
     30,  30,  31,  31,  32,  33,  33,  34,  34,  35,  35,  36,  36,  37,
     37,  38,  38,  39,  40,  40,  41,  41,  42,  42,  43,  44,  44,  45,
     45,  46,  47,  47,  48,  48,  49,  50,  50,  51,  52,  52,  53,  53,
     54,  55,  55,  56,  57,  57,  58,  59,  59,  60,  61,  61,  62,  63,
     63,  64,  65,  65,  66,  67,  67,  68,  69,  69,  70,  71,  72,  72,
     73,  74,  74,  75,  76,  77,  77,  78,  79,  79,  80,  81,  82,  82,
     83,  84,  85,  85,  86,  87,  88,  88,  89,  90,  90,  91,  92,  93,
     93,  94,  95,  96,  97,  97,  98,  99, 100, 100, 101, 102, 103, 103,
    104, 105, 106, 106, 107, 108, 109, 110, 110, 111, 112, 113, 113, 114,
    115, 116, 117, 117, 118, 119, 120, 120, 121, 122, 123, 124, 124, 125,
    126, 127
  };

#else
#error "Valid values for SAMPLE_COUNT are: 256, 512, 1024"
#endif


void setup() {

  // Reset Timer/Counter2 Compare Register A for clarity
  OCR2A = 0;
  
  // Reset the two Timer/Counter2 control registers
  TCCR2A = 0;
  TCCR2B = 0;

  // Set Timer/Counter2 OCR2A to non-inverting mode
  bitSet(TCCR2A, COM2A1);

  // Set Timer/Counter2 into Fast-PWM mode WGM2[2:0] 0x3
  bitSet(TCCR2A, WGM21);
  bitSet(TCCR2A, WGM20);

  // Pre-scaler settings for Timer/Counter2
  // CS22 CS21 CS20
  //   0    0    0    No clock source
  //   0    0    1    /1 Pre-scaler (No Prescaling)
  //   0    1    0    /8 Prescaler
  //   0    1    1    /32 Prescaler
  //   1    0    0    /64 Prescaler
  //   1    0    1    /128 Prescaler
  //   1    1    0    /256 Prescaler
  //   1    1    1    /1024 Prescaler

  // No need to clear the prescaler bits, but adding for clarity and
  // completeness
  
  // sbi(REGISTER, BIT) is the same as bitSet(REGISTER, BIT) 
  // cbi(REGISTER, BIT) is the same as bitClear(REGISTER, BIT)
  
  // Note that these DO NOT call the sbi and cbi assembly
  // instructions. Those assembly instructions are reserved for the
  // I/O registers (PORTA/B/C). However, these are macros available in
  // the Arduino IDE and can be used for any register.
  
#if PRESCALER == 1
  bitClear(TCCR2B, CS22);  bitClear(TCCR2B, CS21);    bitSet(TCCR2B, CS20);
#elif PRESCALER == 8
  bitClear(TCCR2B, CS22);    bitSet(TCCR2B, CS21);  bitClear(TCCR2B, CS20);
#elif PRESCALER == 32
  bitClear(TCCR2B, CS22);    bitSet(TCCR2B, CS21);    bitSet(TCCR2B, CS20);
#elif PRESCALER == 64
    bitSet(TCCR2B, CS22);  bitClear(TCCR2B, CS21);  bitClear(TCCR2B, CS20);
#elif PRESCALER == 128
    bitSet(TCCR2B, CS22);  bitClear(TCCR2B, CS21);    bitSet(TCCR2B, CS20);
#elif PRESCALER == 256
    bitSet(TCCR2B, CS22);    bitSet(TCCR2B, CS21);  bitClear(TCCR2B, CS20);
#elif PRESCALER == 1024
    bitSet(TCCR2B, CS22);    bitSet(TCCR2B, CS21);    bitSet(TCCR2B, CS20);
#else
#error "Valid PRESCALER values for TIMER2 are: 1, 8, 32, 64, 128, 256, 1024"
#endif

  // Set Data Direction Enabled for OC2A Pin, a.k.a., pinMode(11, OUTPUT)
  bitSet(DDRB, PORTB3);

  // Enable the overflow interrupt for Timer/Counter2
  cli(); // Disable interrupts
  TIMSK2 = 0;
  bitSet(TIMSK2, TOIE2);
  sei(); // Re-enable interrupts
}


void loop() {
}


ISR(TIMER2_OVF_vect) {
  static uint16_t i = 0;

  OCR2A = SINE_FULL_WAVE[i++];
  i &= (SAMPLE_COUNT - 1); // Rollover the index if it overflows SAMPLE_COUNT
}