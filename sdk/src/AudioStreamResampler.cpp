/*
 * @Description: AudioStreamResampler.cpp
 * @version: 1.0
 * @Author: Guo Zhc
 * @Date: 2021-01-05 18:28:29
 * @LastEditors: Guo Zhc
 * @LastEditTime: 2021-01-06 18:46:43
 */
#define LOG_TAG "ASResampler"

#include "AudioStreamResampler.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "libavutil/opt.h"
#include "libswresample/swresample.h"
#include "libswresample/version.h"

#ifdef __cplusplus
}
#endif

#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include "ASPLog.h"

#define ALOGD printf
#define ALOGI printf
#define ALOGW printf
#define ALOGE printf

Resampler::Resampler(int rate, AVSampleFormat fmt, int chl, int targetRate, AVSampleFormat targetFmt, int targetChl)
    : mRate(rate),
      mFmt(fmt),
      mChl(chl),
      mTargetRate(targetRate),
      mTargetFmt(targetFmt),
      mTargetChl(targetChl),
      mBytesPerSample(0),
      mTargetBytesPerSample(0),
      mChannelNb(0),
      mTargetChannelNb(0),
      mSwrCtx(nullptr) {
    static bool log_initialized = false;
    if (!log_initialized) {
        ALOGD("av_log_set_callback\n");
        av_log_set_callback(av_log);
        log_initialized = true;
    }

    ALOGI("libswresample verion:%d.%d\n", LIBSWRESAMPLE_VERSION_MAJOR, LIBSWRESAMPLE_VERSION_MINOR);

    const int64_t phase_shift = 3;
    ALOGD("+\n");
    do {
        if (fmt != AV_SAMPLE_FMT_U8 && fmt != AV_SAMPLE_FMT_S16 && fmt != AV_SAMPLE_FMT_S32 &&
            fmt != AV_SAMPLE_FMT_FLT && fmt != AV_SAMPLE_FMT_DBL) {
            ALOGE("not support format\n");
            break;
        }
        if (targetFmt != AV_SAMPLE_FMT_U8 && targetFmt != AV_SAMPLE_FMT_S16 && targetFmt != AV_SAMPLE_FMT_S32 &&
            targetFmt != AV_SAMPLE_FMT_FLT && targetFmt != AV_SAMPLE_FMT_DBL) {
            ALOGE("not support format\n");
            break;
        }
        ALOGD("swr_alloc_set_opts\n");
        mSwrCtx = swr_alloc_set_opts(mSwrCtx, mTargetChl, mTargetFmt, mTargetRate, mChl, mFmt, mRate, 0, nullptr);
        // Speed up initialization.
        av_opt_set_int(mSwrCtx, "phase_shift", phase_shift, 0);
        ALOGI("mSwrCtx phase shift is %ld\n", phase_shift);

        ALOGD("swr_init\n");
        if (mSwrCtx != nullptr) {
            long ts = getSysTimeMs();
            if (swr_init(mSwrCtx) < 0) {
                swr_free(&mSwrCtx);
                ALOGE("swr_init failed\n");
            } else {
                mSwrCtx = nullptr;
                // ALOGD("alloc mSwrCtx ok(0x%x)\n", mSwrCtx);
            }
            ALOGI("time taken by swr_init is %ld ms.\n", getSysTimeMs() - ts);
        } else {
            ALOGE("mSwrCtx alloc failed\n");
        }

        ALOGD("init param\n");
        mBytesPerSample = av_get_bytes_per_sample(mFmt);
        mTargetBytesPerSample = av_get_bytes_per_sample(mTargetFmt);
        mChannelNb = getChannelNumber(mChl);
        mTargetChannelNb = getChannelNumber(mTargetChl);
        ALOGD("ChannelNb[%d] TargetChannelNb[%d]\n", mChannelNb, mTargetChannelNb);
    } while (0);
    ALOGD("-\n");
}

Resampler::~Resampler() {
    if (mSwrCtx != nullptr) {
        swr_free(&mSwrCtx);
    }
}

int Resampler::resample(uint8_t* inAddr, int inSize, uint8_t* outAddr, int outSize) {
    if (mSwrCtx == nullptr || inAddr == nullptr || outAddr == nullptr || inSize <= 0 || outSize <= 0) {
        ALOGE("bad param\n");
        return -1;
    }

    int inSampleCntPerChn = inSize / (mChannelNb * mBytesPerSample);
    int inDelaySampleCnt = swr_get_delay(mSwrCtx, mRate);
    ALOGD("delay=%d, buffer=%d\n", inDelaySampleCnt, inSampleCntPerChn);

    // Handling of loss of precision
    // OutSamples / OutSmapleRate = (InSamples + DelaySamples) / InSampleRate.
    int outSampleCntPerChn = av_rescale_rnd(inDelaySampleCnt + inSampleCntPerChn, mTargetRate, mRate, AV_ROUND_UP);

    if (outSize < outSampleCntPerChn * mTargetChannelNb * mTargetBytesPerSample) {
        ALOGD("delay=%d, buffer=%d\n", inDelaySampleCnt, inSampleCntPerChn);
        ALOGE("out buffer(%d/%d) is too small\n", outSize,
              outSampleCntPerChn * mTargetChannelNb * mTargetBytesPerSample);
        return 0;
    }

    int nbOutSamplesPerChn = swr_convert(mSwrCtx, &outAddr, outSampleCntPerChn, (const uint8_t**)&inAddr, inSampleCntPerChn);
    int sizeOut = nbOutSamplesPerChn * mTargetChannelNb * mTargetBytesPerSample;
    return sizeOut;
}

long Resampler::getSysTimeMs() {
    struct timeval tv = {0};
    long long time = 0LL;
    if (gettimeofday(&tv, nullptr) != 0) {
        ALOGW("gettimeofday error!!");
        return -1;
    }

    time = ((long long)tv.tv_sec) * 1000 + tv.tv_usec / 1000;
    return (long)(time);
}

int Resampler::getChannelNumber(int chl) {
    int chs = 0;
    switch (chl) {
        case AV_CH_LAYOUT_MONO:
            chs = 1;
            break;
        case AV_CH_LAYOUT_STEREO:
        case AV_CH_LAYOUT_STEREO_DOWNMIX:
            chs = 2;
            break;
        case AV_CH_LAYOUT_3POINT1:
        case AV_CH_LAYOUT_4POINT0:
        case AV_CH_LAYOUT_QUAD:
        case AV_CH_LAYOUT_2_2:
            chs = 4;
            break;
        case AV_CH_LAYOUT_5POINT1:
        case AV_CH_LAYOUT_5POINT1_BACK:
        case AV_CH_LAYOUT_6POINT0:
        case AV_CH_LAYOUT_6POINT0_FRONT:
        case AV_CH_LAYOUT_HEXAGONAL:
            chs = 6;
            break;
        case AV_CH_LAYOUT_7POINT1:
        case AV_CH_LAYOUT_7POINT1_WIDE:
        case AV_CH_LAYOUT_7POINT1_WIDE_BACK:
        case AV_CH_LAYOUT_OCTAGONAL:
            chs = 8;
            break;
        default:
            chs = -1;
            break;
    }
    return chs;
}

void Resampler::av_log(void* ptr, int level, const char* fmt, va_list vargs) {
    char msg[512];
    memset(msg, 0x00, 512);
    vsnprintf(msg, 512 - 1, fmt, vargs);
    switch (level) {
        case AV_LOG_INFO:
            ALOGI("%s", msg);
            break;
        case AV_LOG_WARNING:
            ALOGW("%s", msg);
            break;
        case AV_LOG_ERROR:
            ALOGE("%s", msg);
            break;
        case AV_LOG_DEBUG:
        default:
            ALOGD("%s", msg);
            break;
    }
    return;
}
