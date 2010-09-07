#ifndef LOGGERWIDGETOUTPUT_H
#define LOGGERWIDGETOUTPUT_H

#include <string>
#include <fstream>

#include <qwidget.h>

#include "LoggerOutput.h"

// forward declarations
class QTextEdit;

/**
 * @class LoggerWidgetOutput
 * @author Michael Knopke
 * @brief Logs to a Qt Widget (into a QTextEdit)
 *
 */
class LoggerWidgetOutput : public QWidget, public LoggerOutput
{

  Q_OBJECT

  public:

    /**
     * Constructor that sets loglevel.
     * @param logLevel level to log on Widget
     * @param parent the parent widget
     */
    LoggerWidgetOutput(LoggerLogLevel::LogLevel logLevel = LoggerLogLevel::INFOLOG, QWidget* parent = 0);

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

    /**
     * Holds a pointer to the text edit object that displays the log text
     */
    QTextEdit* m_TextEdit;

};

#endif // LOGGERWIDGETOUTPUT_H

