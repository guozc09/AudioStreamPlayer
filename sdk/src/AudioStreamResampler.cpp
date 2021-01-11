/*
 * @Description: AudioStreamResampler.cpp
 * @version: 1.0
 * @Author: Guo Zhc
 * @Date: 2021-01-05 18:28:29
 * @LastEditors: Guo Zhc
 * @LastEditTime: 2021-01-11 10:55:55
 */
#define LOG_TAG "ASResampler"

#include "AudioStreamResampler.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "libavutil/channel_layout.h"
#include "libavutil/opt.h"
#include "libavutil/samplefmt.h"
#include "libswresample/swresample.h"
#include "libswresample/version.h"

#ifdef __cplusplus
}
#endif

#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include <map>

#include "Asplog.h"

static LoggerPtr logger(Logger::getLogger(__FILE__));

using namespace std;

namespace audio_coverter {

class AudioStreamResampler::ResamplerPriv {
  public:
    ResamplerPriv();
    ~ResamplerPriv();
    long getSysTimeMs();
    int getChannelNumber(AudioChannel chl);

  public:
    SampleRate mRate;
    SampleFormat mFmt;
    AudioChannel mChl;
    SampleRate mTargetRate;
    SampleFormat mTargetFmt;
    AudioChannel mTargetChl;
    int mBytesPerSample, mTargetBytesPerSample;
    int mChannelNb, mTargetChannelNb;
    SwrContext* mSwrCtx;
    static map<SampleFormat, AVSampleFormat> mMapSampleFormat;
    static map<AudioChannel, long long unsigned> mMapChannel;
};

map<SampleFormat, AVSampleFormat> AudioStreamResampler::ResamplerPriv::mMapSampleFormat;
map<AudioChannel, long long unsigned> AudioStreamResampler::ResamplerPriv::mMapChannel;

AudioStreamResampler::AudioStreamResampler(SampleRate rate, SampleFormat fmt, AudioChannel chl, SampleRate targetRate,
                                           SampleFormat targetFmt, AudioChannel targetChl)
    : m(new ResamplerPriv) {
    m->mRate = rate;
    m->mFmt = fmt;
    m->mChl = chl;
    m->mTargetRate = targetRate;
    m->mTargetFmt = targetFmt;
    m->mTargetChl = targetChl;
    m->mBytesPerSample = 0;
    m->mTargetBytesPerSample = 0;
    m->mChannelNb = 0;
    m->mTargetChannelNb = 0;
    m->mSwrCtx = nullptr;

    LOG_I("libswresample verion:%d.%d\n", LIBSWRESAMPLE_VERSION_MAJOR, LIBSWRESAMPLE_VERSION_MINOR);

    const int64_t phase_shift = 3;
    LOG_D("+\n");
    do {
        LOG_D("swr_alloc_set_opts\n");
        m->mSwrCtx = swr_alloc_set_opts(m->mSwrCtx, m->mMapChannel[m->mTargetChl], m->mMapSampleFormat[m->mTargetFmt],
                                        m->mTargetRate, m->mMapChannel[m->mChl], m->mMapSampleFormat[m->mFmt], m->mRate,
                                        0, nullptr);
        // Speed up initialization.
        av_opt_set_int(m->mSwrCtx, "phase_shift", phase_shift, 0);
        LOG_I("SwrCtx phase shift is %ld\n", phase_shift);

        LOG_D("swr_init\n");
        if (m->mSwrCtx != nullptr) {
            long ts = m->getSysTimeMs();
            if (swr_init(m->mSwrCtx) < 0) {
                swr_free(&m->mSwrCtx);
                m->mSwrCtx = nullptr;
                LOG_E("swr_init failed\n");
            } else {
                LOG_D("alloc SwrCtx ok(%p)\n", m->mSwrCtx);
            }
            LOG_I("time taken by swr_init is %ld ms.\n", m->getSysTimeMs() - ts);
        } else {
            LOG_E("SwrCtx alloc failed\n");
        }

        LOG_D("init param\n");
        m->mBytesPerSample = av_get_bytes_per_sample(m->mMapSampleFormat[m->mFmt]);
        m->mTargetBytesPerSample = av_get_bytes_per_sample(m->mMapSampleFormat[m->mTargetFmt]);
        m->mChannelNb = m->getChannelNumber(m->mChl);
        m->mTargetChannelNb = m->getChannelNumber(m->mTargetChl);
        LOG_D("ChannelNb[%d] TargetChannelNb[%d]\n", m->mChannelNb, m->mTargetChannelNb);
    } while (0);
    LOG_D("-\n");
}

AudioStreamResampler::~AudioStreamResampler() {
    if (m->mSwrCtx != nullptr) {
        swr_free(&m->mSwrCtx);
    }
    delete m;
}

int AudioStreamResampler::resample(uint8_t* inAddr, size_t inSize, uint8_t* outAddr, size_t outSize) {
    if (m->mSwrCtx == nullptr || inAddr == nullptr || outAddr == nullptr || inSize <= 0 || outSize <= 0) {
        LOG_E("bad param\n");
        return -1;
    }

    int inSampleCntPerChn = inSize / (m->mChannelNb * m->mBytesPerSample);
    int inDelaySampleCnt = swr_get_delay(m->mSwrCtx, m->mRate);

    // Handling of loss of precision
    // OutSamples / OutSmapleRate = (InSamples + DelaySamples) / InSampleRate.
    int outSampleCntPerChn =
        av_rescale_rnd(inDelaySampleCnt + inSampleCntPerChn, m->mTargetRate, m->mRate, AV_ROUND_UP);

    size_t expectedOutSize = outSampleCntPerChn * m->mTargetChannelNb * m->mTargetBytesPerSample;
    if (outSize < expectedOutSize) {
        LOG_D("inDelaySampleCnt=%d, inSampleCntPerChn=%d\n", inDelaySampleCnt, inSampleCntPerChn);
        LOG_E("out buffer(%d/%d) is too small\n", outSize, expectedOutSize);
        return 0;
    }

    int nbOutSamplesPerChn =
        swr_convert(m->mSwrCtx, &outAddr, outSampleCntPerChn, (const uint8_t**)&inAddr, inSampleCntPerChn);
    int sizeOut = nbOutSamplesPerChn * m->mTargetChannelNb * m->mTargetBytesPerSample;
    return sizeOut;
}

AudioStreamResampler::ResamplerPriv::ResamplerPriv() {
    static bool isInitializedMap = false;
    if (!isInitializedMap) {
        // Initializes the sample format.
        mMapSampleFormat[SAMPLE_FMT_NONE] = AV_SAMPLE_FMT_NONE;
        mMapSampleFormat[SAMPLE_FMT_U8] = AV_SAMPLE_FMT_U8;
        mMapSampleFormat[SAMPLE_FMT_S16] = AV_SAMPLE_FMT_S16;
        mMapSampleFormat[SAMPLE_FMT_S32] = AV_SAMPLE_FMT_S32;
        mMapSampleFormat[SAMPLE_FMT_FLT] = AV_SAMPLE_FMT_FLT;
        mMapSampleFormat[SAMPLE_FMT_DBL] = AV_SAMPLE_FMT_DBL;

        // Initializes the channel.
        mMapChannel[CH_LAYOUT_MONO] = AV_CH_LAYOUT_MONO;
        mMapChannel[CH_LAYOUT_STEREO] = AV_CH_LAYOUT_STEREO;
        mMapChannel[CH_LAYOUT_2POINT1] = AV_CH_LAYOUT_2POINT1;
        mMapChannel[CH_LAYOUT_2_1] = AV_CH_LAYOUT_2_1;
        mMapChannel[CH_LAYOUT_SURROUND] = AV_CH_LAYOUT_SURROUND;
        mMapChannel[CH_LAYOUT_3POINT1] = AV_CH_LAYOUT_3POINT1;
        mMapChannel[CH_LAYOUT_4POINT0] = AV_CH_LAYOUT_4POINT0;
        mMapChannel[CH_LAYOUT_4POINT1] = AV_CH_LAYOUT_4POINT1;
        mMapChannel[CH_LAYOUT_2_2] = AV_CH_LAYOUT_2_2;
        mMapChannel[CH_LAYOUT_QUAD] = AV_CH_LAYOUT_QUAD;
        mMapChannel[CH_LAYOUT_5POINT0] = AV_CH_LAYOUT_5POINT0;
        mMapChannel[CH_LAYOUT_5POINT1] = AV_CH_LAYOUT_5POINT1;
        mMapChannel[CH_LAYOUT_5POINT0_BACK] = AV_CH_LAYOUT_5POINT0_BACK;
        mMapChannel[CH_LAYOUT_5POINT1_BACK] = AV_CH_LAYOUT_5POINT1_BACK;
        mMapChannel[CH_LAYOUT_6POINT0] = AV_CH_LAYOUT_6POINT0;
        mMapChannel[CH_LAYOUT_6POINT0_FRONT] = AV_CH_LAYOUT_6POINT0_FRONT;
        mMapChannel[CH_LAYOUT_HEXAGONAL] = AV_CH_LAYOUT_HEXAGONAL;
        mMapChannel[CH_LAYOUT_6POINT1] = AV_CH_LAYOUT_6POINT1;
        mMapChannel[CH_LAYOUT_6POINT1_BACK] = AV_CH_LAYOUT_6POINT1_BACK;
        mMapChannel[CH_LAYOUT_6POINT1_FRONT] = AV_CH_LAYOUT_6POINT1_FRONT;
        mMapChannel[CH_LAYOUT_7POINT0] = AV_CH_LAYOUT_7POINT0;
        mMapChannel[CH_LAYOUT_7POINT0_FRONT] = AV_CH_LAYOUT_7POINT0_FRONT;
        mMapChannel[CH_LAYOUT_7POINT1] = AV_CH_LAYOUT_7POINT1;
        mMapChannel[CH_LAYOUT_7POINT1_WIDE] = AV_CH_LAYOUT_7POINT1_WIDE;
        mMapChannel[CH_LAYOUT_7POINT1_WIDE_BACK] = AV_CH_LAYOUT_7POINT1_WIDE_BACK;
        mMapChannel[CH_LAYOUT_OCTAGONAL] = AV_CH_LAYOUT_OCTAGONAL;
        mMapChannel[CH_LAYOUT_HEXADECAGONAL] = AV_CH_LAYOUT_HEXADECAGONAL;
        mMapChannel[CH_LAYOUT_STEREO_DOWNMIX] = AV_CH_LAYOUT_STEREO_DOWNMIX;
        mMapChannel[CH_LAYOUT_22POINT2] = AV_CH_LAYOUT_22POINT2;

        isInitializedMap = true;
    }
}

AudioStreamResampler::ResamplerPriv::~ResamplerPriv() {}

long AudioStreamResampler::ResamplerPriv::getSysTimeMs() {
    struct timeval tv = {0};
    long long time = 0LL;
    if (gettimeofday(&tv, nullptr) != 0) {
        LOG_W("gettimeofday error!!");
        return -1;
    }

    time = ((long long)tv.tv_sec) * 1000 + tv.tv_usec / 1000;
    return (long)(time);
}

int AudioStreamResampler::ResamplerPriv::getChannelNumber(AudioChannel chl) {
    int chs = 0;
    switch (chl) {
        case CH_LAYOUT_MONO:
            chs = 1;
            break;
        case CH_LAYOUT_STEREO:
        case CH_LAYOUT_STEREO_DOWNMIX:
            chs = 2;
            break;
        case CH_LAYOUT_3POINT1:
        case CH_LAYOUT_4POINT0:
        case CH_LAYOUT_QUAD:
        case CH_LAYOUT_2_2:
            chs = 4;
            break;
        case CH_LAYOUT_5POINT1:
        case CH_LAYOUT_5POINT1_BACK:
        case CH_LAYOUT_6POINT0:
        case CH_LAYOUT_6POINT0_FRONT:
        case CH_LAYOUT_HEXAGONAL:
            chs = 6;
            break;
        case CH_LAYOUT_7POINT1:
        case CH_LAYOUT_7POINT1_WIDE:
        case CH_LAYOUT_7POINT1_WIDE_BACK:
        case CH_LAYOUT_OCTAGONAL:
            chs = 8;
            break;
        default:
            chs = -1;
            break;
    }
    return chs;
}

}  // namespace audio_coverter
