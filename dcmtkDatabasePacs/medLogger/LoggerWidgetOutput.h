#ifndef LOGGERWIDGETOUTPUT_H
#define LOGGERWIDGETOUTPUT_H

#include <string>
#include <fstream>

#include <QWidget>

#include "LoggerOutput.h"


/**
 * @class LoggerWidgetOutput
 * @author Michael Knopke
 * @brief Logs to a Qt Widget (into a QTextEdit)
 */
class LoggerWidgetOutput : public LoggerOutput
{

  public:

    /**
     * Constructor that sets loglevel.
     * @param logLevel level to log on Widget
     * @param parent the parent widget
     */
    LoggerWidgetOutput(QWidget* parent,LoggerLogLevel::LogLevel logLevel = LoggerLogLevel::INFOLOG);

    /**
     * The destructor does nothing
     */
    ~LoggerWidgetOutput();

    /**
     * Logs a message to the Widget if logLevel is smaller or equal than m_LogLevel
     * @param message message string
     * @param logLevel the LogLevel @see Logger
     */
    void logMessage(std::string message, LoggerLogLevel::LogLevel logLevel);

  private:

      QWidget*  m_parent;

};

#endif // LOGGERWIDGETOUTPUT_H

