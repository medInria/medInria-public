#ifndef LOGGERLOGSTREAM_H
#define LOGGERLOGSTREAM_H

#include <sstream>

#include "LoggerLogLevel.h"

/**
 * @class LoggerLogStream
 * @author Michael Knopke
 * @brief Class for logging in a stream fashion
 */
class LoggerLogStream
{

  public:

    /**
     * Constructor initializes members.
     * @param logLevel level that logs this stream to
     */
    LoggerLogStream(LoggerLogLevel::LogLevel logLevel = LoggerLogLevel::INFOLOG);

    /**
     * Destructor frees the member m_Buffer and flushes the buffer to the logger
     */
    ~LoggerLogStream();

    /**
     * Writes a message to the buffer
     * @param value the message to write
     * @return reference to the modified log stream
     */
    template<typename T>
    LoggerLogStream& operator<<(const T& value)
    {
      (*m_Buffer) << value;
      return *this;
    }

    /**
     * flushes the buffer (sends all contents of m_Buffer to Logger)
     */
    void flushBuffer();

  private:

    LoggerLogLevel::LogLevel m_LogLevel;
    std::ostringstream* m_Buffer;
};

#endif // LOGGERLOGSTREAM_H

