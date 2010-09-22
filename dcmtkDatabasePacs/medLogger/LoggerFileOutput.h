#ifndef LOGGERFILEOUTPUT_H
#define LOGGERFILEOUTPUT_H

#include <string>
#include <fstream>

#include "LoggerOutput.h"

/**
 * @class LoggerFileOutput
 * @author Michael Knopke
 * @brief Logs to a file.
 *
 */
class LoggerFileOutput : public LoggerOutput
{

  public:

    /**
     * Constructor that sets loglevel and filename.
     * The file is opened as an ostream and the reference to the stream is stored in a member variable.
     * @param logLevel level to log in file
     * @param filename name of the file to log to.
     */
    LoggerFileOutput(LoggerLogLevel::LogLevel logLevel = LoggerLogLevel::INFOLOG, const char* filename = "medLogOutput.txt");

    /**
     * The destructor closes the file stream
     */
    virtual ~LoggerFileOutput();

    /**
     * Logs a message to the filestream if logLevel is smaller or equal than m_LogLevel
     * @param message message string
     * @param logLevel the LogLevel @see Logger
     */
    void logMessage(std::string message, LoggerLogLevel::LogLevel logLevel);

  protected:

  private:

    /**
     * Stores the output stream.
     */
    std::ofstream m_OutputStream;

    /**
     * Stores a sequence number.
     */
    unsigned int m_SequenceNumber;

};

#endif // LOGGERFILEOUTPUT_H

