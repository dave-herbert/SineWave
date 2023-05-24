#!/usr/bin/env python3
#
# Program to generate one period of Sine Wave in the range of 0-255.
#
#
# There are three options: FULL_WAVE, HALF_WAVE, and QUARTER_WAVE
# PI Character: 𝜋 = (u+1D70B) 
#
# FULL_WAVE:
#   One complete wave from [0, 2𝜋)
# HALF_WAVE:
#   One complete wave from [0, 𝜋)
# QUARTER_WAVE:
#   One complete wave from [0, 𝜋/2)

import math

SAMPLE_COUNT_DEFAULT = 512

FULL_WAVE    = 0
HALF_WAVE    = 1
QUARTER_WAVE = 2

def sample_array(count=SAMPLE_COUNT_DEFAULT, wave_type=FULL_WAVE, col_count=8):
  if wave_type == FULL_WAVE:
    wave_name = 'FULL_WAVE'
    factor = 2 * math.pi / count
    # Need to map from [-1, 1] -> [0, 255]
  elif wave_type == HALF_WAVE:
    wave_name = 'HALF_WAVE'
    factor = math.pi / count
  else:  # QUARTER_WAVE
    wave_name = 'QUARTER_WAVE'
    factor = math.pi / 2 / count
  # Need to map from [-1, 1] -> [0, 255]
  # sine + 1 -> [0, 2] -> [0, 255]
  dc_offset = 1
  scale = 255 / 2

  print(f'const uint8_t SINE_{wave_name}[SAMPLE_COUNT] =\n  {{')

  for s in range(0, count, col_count):
    index = []
    x_array = []
    s_array = []
    a_array = []  # Scaled from 0-255
    output = []
    for c in range(col_count):
      i = s + c  # Index
      if i >= count:
        break
      index.append(i)  # Calculate sine here
      x = i * factor
      x_array.append(x)
      sine_of_x = math.sin(x)
      s_array.append(sine_of_x)
      a_scaled = (sine_of_x + dc_offset) * scale
      a_array.append(round(a_scaled))
      
    output = [f'{a:3}' for a in a_array]
    print(f'    {", ".join(str(i) for i in output)}', end='')
    if i < count:
      print(',')  # End line
    else:
      print('')  # End line
  print('  };\n')


def main():
  print('/* Sine Wave precalculated data from "python3 sine_array.py"*/')
  print('/* Valid values for SAMPLE_COUNT are: 256, 512, or 1024 */\n')
  print(f'#define SAMPLE_COUNT {SAMPLE_COUNT_DEFAULT}\n')
  if_str = '#if'
  for s in (256, 512, 1024):
    print(f'{if_str} SAMPLE_COUNT == {s}\n')
    sample_array(s, wave_type=FULL_WAVE, col_count=14)
    if_str = '#elif'
  print('#else')
  print('#error "Valid values for SAMPLE_COUNT are: 256, 512, 1024"')
  print('#endif')


if '__name__':
  exit(main())
