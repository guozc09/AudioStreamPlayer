/*
 * @Description: AudioStreamResampler.h
 * @version: 1.0
 * @Author: Guo Zhc
 * @Date: 2021-01-05 18:28:33
 * @LastEditors: Guo Zhc
 * @LastEditTime: 2021-01-06 18:47:27
 */

#ifndef AUDIO_STREAM_RESAMPLER_H
#define AUDIO_STREAM_RESAMPLER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "libavutil/samplefmt.h"
#include "libavutil/channel_layout.h"
#include "libswresample/swresample.h"

#ifdef __cplusplus
}
#endif

class Resampler {
  public:
    Resampler(int rate, AVSampleFormat fmt, int chl, int targetRate, AVSampleFormat targetFmt, int targetChl);
    ~Resampler();
    int resample(uint8_t* inAddr, int inSize, uint8_t* outAddr, int outSize);

  private:
    long getSysTimeMs();
    int getChannelNumber(int chl);
    static void av_log(void* ptr, int level, const char* fmt, va_list vargs);

  private:
    const int mRate;
    const AVSampleFormat mFmt;
    const int mChl;
    const int mTargetRate;
    const AVSampleFormat mTargetFmt;
    const int mTargetChl;
    int mBytesPerSample, mTargetBytesPerSample;
    int mChannelNb, mTargetChannelNb;
    SwrContext* mSwrCtx;
};

#endif  // AUDIO_STREAM_RESAMPLER_H
