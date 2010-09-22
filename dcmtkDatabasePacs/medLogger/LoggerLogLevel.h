#ifndef LOGGERLOGLEVEL_H
#define LOGGERLOGLEVEL_H

/**
 * @namespace Namespace for log levels used in Logger
 */
namespace LoggerLogLevel
{
  /**
   * Defines the log levels
   */
  enum LogLevel {
    NOLOG = 0,
    ERRORLOG = 1,
    WARNINGLOG = 2,
    INFOLOG = 3,
    DEBUGLOG = 4
  };
};

#endif
