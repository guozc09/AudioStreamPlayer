/*
 * @Description: ASPLogImpl.h
 * @version: 1.0
 * @Author: Zhc Guo
 * @Date: 2021-01-06 17:06:50
 * @LastEditors: Zhc Guo
 * @LastEditTime: 2021-01-06 17:07:44
 */
#pragma once

#include "ASPLog.h"

class ASPLogFuncGroupImpl : public ASPLogFuncGroup {
  public:
    ASPLogFuncGroupImpl();
    ~ASPLogFuncGroupImpl();

    void logD(const char *tag, const char *funcName, const char *format, ...);
    void logI(const char *tag, const char *funcName, const char *format, ...);
    void logW(const char *tag, const char *funcName, const char *format, ...);
    void logE(const char *tag, const char *funcName, const char *format, ...);

  private:
    char *mLogBuffer;
};
