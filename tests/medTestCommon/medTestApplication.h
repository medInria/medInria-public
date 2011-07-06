/*
 * medTestApplication.h
 *
 *  Created on: 4 juil. 2011 17:48:22
 *      Author: John Stark
 */

#ifndef MEDTESTAPPLICATION_H_
#define MEDTESTAPPLICATION_H_

#include <QCoreApplication>

#include "medTestCommonExport.h"

/** A basic application which does not require a GUI.
 *  This is so that tests that do not require a GUI can be run on batch servers
 *  without an X-server connection.
 */
class MEDTESTCOMMON_EXPORT medTestApplication : public QCoreApplication
{
    Q_OBJECT;
public:
    medTestApplication( int & argc, char ** argv );
    virtual ~medTestApplication();
    //! For testing results. Throws on error.
    static void CheckTestResult( bool condition, const char *cond, const char *file, const int line = 0 );
};

#define CHECK_TEST_RESULT(cond) medTestApplication::CheckTestResult((cond), DTK_STRINGIZE(cond), __FILE__, __LINE__)


#endif /* MEDTESTAPPLICATION_H_ */

