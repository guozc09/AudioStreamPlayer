/*
 * @Author: Zhc Guo
 * @Date: 2021-01-05 11:16:25
 * @LastEditors: Zhc Guo
 * @LastEditTime: 2021-01-05 11:23:51
 * @Description:
 */

#pragma once

class ASPLogFuncGroup {
  public:
    ASPLogFuncGroup() = default;
    virtual ~ASPLogFuncGroup() = default;
    virtual void logD(const char *tag, const char *funcName, const char *format, ...) = 0;
    virtual void logI(const char *tag, const char *funcName, const char *format, ...) = 0;
    virtual void logW(const char *tag, const char *funcName, const char *format, ...) = 0;
    virtual void logE(const char *tag, const char *funcName, const char *format, ...) = 0;
};
