/*
 * @Author: Zhc Guo
 * @Date: 2021-01-05 11:24:52
 * @LastEditors: Zhc Guo
 * @LastEditTime: 2021-01-06 18:28:33
 * @Description:
 */
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "ASPLogImpl.h"

#define LOG_BUFFER_LEN (1024*5)

ASPLogFuncGroupImpl::ASPLogFuncGroupImpl() : mLogBuffer(nullptr) {
    mLogBuffer = new char[LOG_BUFFER_LEN];
}

ASPLogFuncGroupImpl::~ASPLogFuncGroupImpl() {
    delete [] mLogBuffer;
}

void ASPLogFuncGroupImpl::logD(const char *tag, const char *funcName, const char *format, ...) {
    uint32_t uResult = 0;
    uint32_t len = 0;
    va_list stVaList;
    va_start(stVaList, format);
    memset(mLogBuffer, 0, LOG_BUFFER_LEN);
    uResult = sprintf(mLogBuffer, "[%s|%s|%s]", tag, funcName, "Debug");
    if (uResult < 0) {
        va_end(stVaList);
        return;
    }
    len += uResult;
    uResult = vsprintf(mLogBuffer + len, (const char *)format, stVaList);
    if (uResult < 0) {
        va_end(stVaList);
        return;
    }
    printf("%s\n", mLogBuffer);
    va_end(stVaList);
    return;
}

void ASPLogFuncGroupImpl::logI(const char *tag, const char *funcName, const char *format, ...) {
    uint32_t uResult = 0;
    uint32_t len = 0;
    va_list stVaList;
    va_start(stVaList, format);
    memset(mLogBuffer, 0, LOG_BUFFER_LEN);

    uResult = sprintf(mLogBuffer, "[%s|%s|%s]", tag, funcName, "Info");
    if (uResult < 0) {
        va_end(stVaList);
        return;
    }
    len += uResult;
    uResult = vsprintf(mLogBuffer + len, (const char *)format, stVaList);
    if (uResult < 0) {
        va_end(stVaList);
        return;
    }
    printf("%s\n", mLogBuffer);
    va_end(stVaList);
    return;
}

void ASPLogFuncGroupImpl::logW(const char *tag, const char *funcName, const char *format, ...) {
    uint32_t uResult = 0;
    uint32_t len = 0;
    va_list stVaList;
    va_start(stVaList, format);
    memset(mLogBuffer, 0, LOG_BUFFER_LEN);
    uResult = sprintf(mLogBuffer, "[%s|%s|%s]", tag, funcName, "Warn");
    if (uResult < 0) {
        va_end(stVaList);
        return;
    }
    len += uResult;
    uResult = vsprintf(mLogBuffer + len, (const char *)format, stVaList);
    if (uResult < 0) {
        va_end(stVaList);
        return;
    }
    printf("%s\n", mLogBuffer);
    va_end(stVaList);
    return;
}

void ASPLogFuncGroupImpl::logE(const char *tag, const char *funcName, const char *format, ...) {
    uint32_t uResult = 0;
    uint32_t len = 0;
    va_list stVaList;
    va_start(stVaList, format);
    memset(mLogBuffer, 0, LOG_BUFFER_LEN);
    uResult = sprintf(mLogBuffer, "[%s|%s|%s]", tag, funcName, "Error");
    if (uResult < 0) {
        va_end(stVaList);
        return;
    }
    len += uResult;
    uResult = vsprintf(mLogBuffer + len, (const char *)format, stVaList);
    if (uResult < 0) {
        va_end(stVaList);
        return;
    }
    printf("%s\n", mLogBuffer);
    va_end(stVaList);
    return;
}
