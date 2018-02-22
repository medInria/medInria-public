/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QCoreApplication>

#include <medTestExport.h>

/** A basic application which does not require a GUI.
 *  This is so that tests that do not require a GUI can be run on batch servers
 *  without an X-server connection.
 */
class MEDTEST_EXPORT medTestApplication : public QCoreApplication
{
    Q_OBJECT;
public:
    medTestApplication( int & argc, char ** argv );
    virtual ~medTestApplication();
    //! For testing results. Throws on error.
    static void CheckTestResult( bool condition, const char *cond, const char *file, const int line = 0 );
};

#define CHECK_TEST_RESULT(cond) medTestApplication::CheckTestResult((cond), DTK_STRINGIZE(cond), __FILE__, __LINE__)




