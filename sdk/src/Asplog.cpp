/*
 * @Description:
 * @Author: Guo Zhc
 * @Date: 1970-01-01 08:00:00
 * @LastEditors: Guo Zhc
 * @LastEditTime: 2021-01-09 23:18:41
 */
#include "Asplog.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// include log4cxx header files.
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/helpers/exception.h"
#include "log4cxx/logger.h"

using namespace log4cxx;
using namespace log4cxx::helpers;

#define LOG_BUFFER_LEN (1024 * 5)

static char logBuffer[LOG_BUFFER_LEN] = {0};

void aspLog(LoggerPtr &logger, LogLevel level, const char *format, ...) {
    uint32_t uResult = 0;
    uint32_t len = 0;
    va_list stVaList;
    va_start(stVaList, format);
    memset(logBuffer, 0, LOG_BUFFER_LEN);

    uResult = vsprintf(logBuffer + len, (const char *)format, stVaList);
    if (uResult < 0) {
        va_end(stVaList);
        return;
    }
    switch (level) {
        case LOG_DEBUG:
            LOG4CXX_DEBUG(logger, logBuffer);
            break;
        case LOG_INFO:
            LOG4CXX_INFO(logger, logBuffer);
            break;
        case LOG_WARN:
            LOG4CXX_WARN(logger, logBuffer);
            break;
        case LOG_ERROR:
            LOG4CXX_ERROR(logger, logBuffer);
            break;
        default:
            LOG4CXX_DEBUG(logger, logBuffer);
            break;
    }
    va_end(stVaList);
    return;
}
