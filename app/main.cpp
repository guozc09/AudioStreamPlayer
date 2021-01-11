/*
 * @Description: 应用程序入口
 * @Author: Guo Zhc
 * @Date: 2021-01-05 13:33:47
 * @LastEditors: Guo Zhc
 * @LastEditTime: 2021-01-11 10:14:27
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "AudioStreamResampler.h"
#include "Asplog.h"

using namespace std;
using namespace audio_coverter;

int main(int argc, char *argv[]) {
    // BasicConfigurator replaced with PropertyConfigurator.
    PropertyConfigurator::configure("/home/zhcguo/linuxMount/AudioStreamPlayer/doc/config/log4cxx.properties");

    FILE *ifs = fopen("/home/zhcguo/linuxMount/AudioStreamPlayer/doc/那女孩对我说.pcm", "rb");
    FILE *ofs = fopen("/home/zhcguo/linuxMount/AudioStreamPlayer/doc/那女孩对我说2.pcm", "wb");
    uint8_t tmpinbuf[1024];
    uint8_t tmpoutbuf[1024];
    int ret = 0;
    AudioStreamResampler resample(SAMPLE_RATE_44_1K, SAMPLE_FMT_FLT, CH_LAYOUT_STEREO, SAMPLE_RATE_16K, SAMPLE_FMT_FLT, CH_LAYOUT_STEREO);
    while (true) {
        ret = fread(tmpinbuf, 1, sizeof(tmpinbuf), ifs);
        if (ret <= 0) {
            break;
        }
        ret = resample.resample(tmpinbuf, ret, tmpoutbuf, sizeof(tmpoutbuf));
        if (ret > 0) {
            fwrite(tmpoutbuf, 1, ret, ofs);
        }
    }

    fclose(ifs);
    fclose(ofs);
    return 0;
}