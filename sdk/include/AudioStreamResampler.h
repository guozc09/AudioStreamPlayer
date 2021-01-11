/*
 * @Description: AudioStreamResampler.h
 * @version: 1.0
 * @Author: Guo Zhc
 * @Date: 2021-01-05 18:28:33
 * @LastEditors: Guo Zhc
 * @LastEditTime: 2021-01-11 15:24:20
 */

#ifndef AUDIO_STREAM_RESAMPLER_H
#define AUDIO_STREAM_RESAMPLER_H

#include <stdint.h>
#include <cstddef>

namespace audio_coverter {

enum SampleRate {
    SAMPLE_RATE_8K = 8000u,
    SAMPLE_RATE_11K = 11025u,
    SAMPLE_RATE_12K = 12000u,
    SAMPLE_RATE_16K = 16000u,
    SAMPLE_RATE_22K = 22050u,
    SAMPLE_RATE_24K = 24000u,
    SAMPLE_RATE_32K = 32000u,
    SAMPLE_RATE_44_1K = 44100u,
    SAMPLE_RATE_48K = 48000u,
    SAMPLE_RATE_96000 = 96000u,
    SAMPLE_RATE_192000 = 192000u
};

enum SampleFormat {
    SAMPLE_FMT_U8 = 0,      ///< unsigned 8 bits
    SAMPLE_FMT_S16,         ///< signed 16 bits
    SAMPLE_FMT_S32,         ///< signed 32 bits
    SAMPLE_FMT_FLT,         ///< float
    SAMPLE_FMT_DBL,         ///< double
};

enum AudioChannel {
    CH_LAYOUT_MONO = 0,
    CH_LAYOUT_STEREO,
    CH_LAYOUT_2POINT1,
    CH_LAYOUT_2_1,
    CH_LAYOUT_SURROUND,
    CH_LAYOUT_3POINT1,
    CH_LAYOUT_4POINT0,
    CH_LAYOUT_4POINT1,
    CH_LAYOUT_2_2,
    CH_LAYOUT_QUAD,
    CH_LAYOUT_5POINT0,
    CH_LAYOUT_5POINT1,
    CH_LAYOUT_5POINT0_BACK,
    CH_LAYOUT_5POINT1_BACK,
    CH_LAYOUT_6POINT0,
    CH_LAYOUT_6POINT0_FRONT,
    CH_LAYOUT_HEXAGONAL,
    CH_LAYOUT_6POINT1,
    CH_LAYOUT_6POINT1_BACK,
    CH_LAYOUT_6POINT1_FRONT,
    CH_LAYOUT_7POINT0,
    CH_LAYOUT_7POINT0_FRONT,
    CH_LAYOUT_7POINT1,
    CH_LAYOUT_7POINT1_WIDE,
    CH_LAYOUT_7POINT1_WIDE_BACK,
    CH_LAYOUT_OCTAGONAL,
    CH_LAYOUT_HEXADECAGONAL,
    CH_LAYOUT_STEREO_DOWNMIX,
    CH_LAYOUT_22POINT2,
};

class AudioStreamResampler {
  public:
    AudioStreamResampler(SampleRate rate, SampleFormat fmt, AudioChannel chl, SampleRate targetRate, SampleFormat targetFmt, AudioChannel targetChl);
    ~AudioStreamResampler();
    int resample(uint8_t* inAddr, size_t inSize, uint8_t* outAddr, size_t outSize);

  private:
    class ResamplerPriv;
    ResamplerPriv* m;
};

}  // namespace audio_coverter

#endif  // AUDIO_STREAM_RESAMPLER_H
