#ifndef DCMTKLOGGER_H
#define DCMTKLOGGER_H
#include "BaseLogger.h"


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstring.h"

/**
 * @class dcmtkLogger
 * @author Michael Knopke
 * @brief Main logger class to be used in dcmtk based code
 * Inherits from generic logger and adds OFSting conversion methods
 */
class dcmtkLogger : public BaseLogger
{
public:

    /**
     * Logs a debug message (log level DEBUG)
     * @param message message string
     */
    static void debug(OFString message);

    /**
     * Logs an info message (log level INFO)
     * @param message message string
     */
    static void info(OFString message);

    /**
     * Logs a warning message (log level WARNING)
     * @param message message string
     */
    static void warning(OFString message);

    /**
     * Logs an error message (log level ERROR)
     * @param message message string
     */
    static void error(OFString message);
};

#endif //DCMTKLOGGER_H
