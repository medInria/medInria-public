#ifndef LOGGERCONSOLEOUTPUT_H
#define LOGGERCONSOLEOUTPUT_H

#include <string>
#include <fstream>

#include "LoggerOutput.h"

/**
 * @class LoggerConsoleOutput
 * @author Michael Knopke
 * @brief Logs to the console (stdout)
 *
 */
class LoggerConsoleOutput : public LoggerOutput
{

  public:

    /**
     * Constructor that sets loglevel.
     * @param logLevel level to log on console
     */
    LoggerConsoleOutput(LoggerLogLevel::LogLevel logLevel = LoggerLogLevel::INFOLOG);

    /**
     * The destructor does nothing
     */
    virtual ~LoggerConsoleOutput();

    /**
     * Logs a message to the console if logLevel is smaller or equal than m_LogLevel
     * @param message message string
     * @param logLevel the LogLevel @see Logger
     */
    void logMessage(std::string message, LoggerLogLevel::LogLevel logLevel);

  protected:

  private:

};

#endif // LOGGERCONSOLEOUTPUT_H

