/*
 * @Description: Asplog.h
 * @Author: Guo Zhc
 * @Date: 1970-01-01 08:00:00
 * @LastEditors: Guo Zhc
 * @LastEditTime: 2021-01-10 00:51:17
 */
#pragma once

// include log4cxx header files.
#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/propertyconfigurator.h"
#include "log4cxx/helpers/exception.h"
 
using namespace log4cxx;
using namespace log4cxx::helpers;

#define LOG_D(...) aspLog(logger, LOG_DEBUG, ##__VA_ARGS__)
#define LOG_I(...) aspLog(logger, LOG_INFO, ##__VA_ARGS__)
#define LOG_W(...) aspLog(logger, LOG_WARN, ##__VA_ARGS__)
#define LOG_E(...) aspLog(logger, LOG_ERROR, ##__VA_ARGS__)

enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
};

void aspLog(LoggerPtr &logger, LogLevel level, const char *format, ...);
