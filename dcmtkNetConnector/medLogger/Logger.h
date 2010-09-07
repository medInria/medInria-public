#ifndef LOGGER_H
#define LOGGER_H


#include <string>
#include <vector>
#include <sstream>
#include "LoggerLogStream.h"
#include "LoggerLogLevel.h"

// forward declarations
class LoggerOutput;

    /**
     * @class Logger
     * @author Michael Knopke
     * @brief Logging class. Logs to LoggerOutput objects.
     *
     * This Logger logs to LoggerOutput objects. These output objects define what action has to be done on loggin (e.g.
     * writing the log messages to std::cout or to a logfile etc.).
     * To use the logger, call Logger::startUp() first. Then attach some LoggerOutput objects using Logger::addOutput(). These
     * objects will automatically be deleted when the logger gets destroyed.
     * To destroy the logger and all attached logger outputs call Logger::shutDown().
     */
class Logger
{

  public:

    /**
     * The destructor deletes all attached logger outputs.
     */
    virtual ~Logger();

    /**
     * Logs a debug message (log level DEBUG)
     * @param message message string
     */
    static void debug(std::string message);

    /**
     * Logs an info message (log level INFO)
     * @param message message string
     */
    static void info(std::string message);

    /**
     * Logs a warning message (log level WARNING)
     * @param message message string
     */
    static void warning(std::string message);

    /**
     * Logs an error message (log level ERROR)
     * @param message message string
     */
    static void error(std::string message);

    /**
     * Logs a message with the given log level to all connected LoggerOutput objects.
     * @param message message to log
     * @param logLevel log level
     */
    static void logMessage(std::string message, LoggerLogLevel::LogLevel logLevel);

    /**
     * Add a logger output. All messages will be send to all added outputs.
     * @param output Pointer to an output object. Must be valid through the lifetime of the logger
     * or detached via removeOutput() before being destroyed!
     */
    static void addOutput(LoggerOutput* output);

    /**
     * Removes a logger output. The connected logger output are searched for the given, if it is found,
     * it will be removed from the list.
     * @param output pointer to the output object that has to be removed.
     */
    static void removeOutput(LoggerOutput* output);

    /**
     * @return logging stream that logs info messages
     */
    static LoggerLogStream debugStream();

    /**
     * @return logging stream that logs info messages
     */
    static LoggerLogStream infoStream();

    /**
     * @return logging stream that logs warning messages
     */
    static LoggerLogStream warningStream();

    /**
     * @return logging stream that logs error messages
     */
    static LoggerLogStream errorStream();

    /**
     * Provided for convenience. Returns the current time in a string.
     * @return string with the actual time in the format HH:MM:SS
     */
    static std::string getTimeString();

    /**
     * Provided for convenience. Returns the current date in a string.
     * @return string with the current date in the format DD.MM.YYYY
     */
    static std::string getDateString();

    /**
     * Creates a new Logger instance if no instance exists.
     */
    static void startUp();

    /**
     * Shuts down the logger and destroys all attached logger outputs. The single instance of the logger will be destroyed.
     */
    static void shutDown();

  protected:

    /**
     * Method to retrieve an instance of the logger singleton. If there still is no instance, a new instance
     * will be created. Else, the existing instance will be returned.
     * @return Instance of Logger
     */
    static Logger* getInstance();

    /**
     * Destroys the instance.
     */
    static void destroy();

  private:

    /**
     * Singleton (-> private constructor),
     * use getInstance() to get an instance of the logger.
     * The constructor sets the default members.
     */
    Logger();

    /**
     * Protect the copy constructor to avoid copies.
     */
    Logger(const Logger& logger);

    /**
     * Flushes all streams.
     */
    void flushStreams();

    /**
     * Vector of all outputs.
     */
    std::vector<LoggerOutput*> m_Outputs;

    /**
     * The single instance
     */
    static Logger* m_Instance;

};

#endif // Logger_H

