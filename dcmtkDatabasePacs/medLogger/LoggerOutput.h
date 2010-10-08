#ifndef LOGGEROUTPUT_H
#define LOGGEROUTPUT_H

#include <string>
#include <sstream>

#include "BaseLogger.h"
#include "LoggerLogLevel.h"

/**
 * @class LoggerOutput
 * @author Michael Knopke
 * @brief Base class for logger outputs.
 *
 */
class LoggerOutput
{

  public:

    /**
     * Constructor sets the log level
     * @param logLevel log level for this output
     * @param name name for this output
     */
    LoggerOutput(LoggerLogLevel::LogLevel logLevel = LoggerLogLevel::INFOLOG, std::string name = "unnamed");

    /**
     * Empty virtual destructor
     */
    virtual ~LoggerOutput();

    /**
     * Interface function. Implement this in subclasses.
     * @param message message string
     * @param logLevel the LogLevel @see Logger
     */
    virtual void logMessage(std::string message, LoggerLogLevel::LogLevel logLevel) = 0;

    /**
     * Sets the log level
     * @param level new log level
     */
    void setLogLevel(LoggerLogLevel::LogLevel level);

    /**
     * Gets the log level
     * @return log level
     */
    LoggerLogLevel::LogLevel getLogLevel() const;

    /**
     * @return the name of the logger
     */
    std::string getName() const;

  protected:

  private:

    /**
     * Stores the log level
     */
    LoggerLogLevel::LogLevel m_LogLevel;

    /**
     * Stores the name of the logger.
     */
    std::string m_Name;

};

#endif // LOGGEROUTPUT_H

