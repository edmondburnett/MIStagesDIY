// Copyright 2017 Olivier Gillet.
//
// Author: Olivier Gillet (ol.gillet@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// Settings storage.

#include "stages/settings.h"

#include <algorithm>

#include "stmlib/system/storage.h"

namespace stages {

using namespace std;

#define FIX_OUTLIER(destination, expected_value)                               \
  if (fabsf(destination / expected_value - 1.0f) > 0.1f) {                     \
    destination = expected_value;                                              \
  }
#define FIX_OUTLIER_ABSOLUTE(destination, expected_value)                      \
  if (fabsf(destination - expected_value) > 0.1f) {                            \
    destination = expected_value;                                              \
  }

bool Settings::Init() {
  ChannelCalibrationData default_calibration_data;

  default_calibration_data.dac_offset = 32768.0f;
  default_calibration_data.dac_scale = -32263.0f;
  default_calibration_data.adc_offset = 0.0f;
  default_calibration_data.adc_scale = -1.0f;

  fill(&persistent_data_.channel_calibration_data[0],
       &persistent_data_.channel_calibration_data[kNumChannels],
       default_calibration_data);

  fill(&state_.segment_configuration[0],
       &state_.segment_configuration[kNumChannels], 0);

#if 1
  // Edmond's settings for the second build (purple PCB)
  persistent_data_.channel_calibration_data[0].dac_offset = 32688.0324252f;
  persistent_data_.channel_calibration_data[0].dac_scale = -32208.2337488f;
  persistent_data_.channel_calibration_data[1].dac_offset = 32726.6554891f;
  persistent_data_.channel_calibration_data[1].dac_scale = -32375.8494517f;
  persistent_data_.channel_calibration_data[2].dac_offset = 32660.6127607f;
  persistent_data_.channel_calibration_data[2].dac_scale = -32182.3988848f;
  persistent_data_.channel_calibration_data[3].dac_offset = 32286.7643120f;
  persistent_data_.channel_calibration_data[3].dac_scale = -28031.4175080f;
  persistent_data_.channel_calibration_data[4].dac_offset = 32738.4726037f;
  persistent_data_.channel_calibration_data[4].dac_scale = -32141.3443058f;
  persistent_data_.channel_calibration_data[5].dac_offset = 32708.1772108f;
  persistent_data_.channel_calibration_data[5].dac_scale = -32205.39910904f;

  persistent_data_.channel_calibration_data[0].adc_offset =
      0.04197841f /* FIXME */;
  persistent_data_.channel_calibration_data[0].adc_scale =
      -0.99318016f /* FIXME */;
  persistent_data_.channel_calibration_data[1].adc_offset =
      0.04287711f /* FIXME */;
  persistent_data_.channel_calibration_data[1].adc_scale =
      -0.99262151f /* FIXME */;
  persistent_data_.channel_calibration_data[2].adc_offset =
      0.04866396f /* FIXME */;
  persistent_data_.channel_calibration_data[2].adc_scale =
      -0.99272005f /* FIXME */;
  persistent_data_.channel_calibration_data[3].adc_offset =
      0.04803432f /* FIXME */;
  persistent_data_.channel_calibration_data[3].adc_scale =
      -0.98005906f /* FIXME */;
  persistent_data_.channel_calibration_data[4].adc_offset =
      0.04541499f /* FIXME */;
  persistent_data_.channel_calibration_data[4].adc_scale =
      -0.99439822f /* FIXME */;
  persistent_data_.channel_calibration_data[5].adc_offset =
      0.04106699f /* FIXME */;
  persistent_data_.channel_calibration_data[5].adc_scale =
      -0.99255583f /* FIXME */;
#endif

  state_.color_blind = 0;

  bool success = chunk_storage_.Init(&persistent_data_, &state_);

  // Sanitize settings read from flash.
  if (success) {
    for (size_t i = 0; i < kNumChannels; ++i) {
      ChannelCalibrationData *c = &persistent_data_.channel_calibration_data[i];
      FIX_OUTLIER_ABSOLUTE(c->adc_offset, 0.0f);

      FIX_OUTLIER(c->dac_offset, 32768.0f);
      FIX_OUTLIER(c->dac_scale, -32263.0f);
      FIX_OUTLIER(c->adc_scale, -1.0f);

      uint8_t type_bits = state_.segment_configuration[i] & 0x3;
      uint8_t loop_bit = state_.segment_configuration[i] & 0x4;
      CONSTRAIN(type_bits, 0, 3);
      state_.segment_configuration[i] = type_bits | loop_bit;
    }
  }

  return success;
}

void Settings::SavePersistentData() { chunk_storage_.SavePersistentData(); }

void Settings::SaveState() { chunk_storage_.SaveState(); }

} // namespace stages
