/*
 * medTestApplication.cpp
 *
 *  Created on: 4 juil. 2011 17:48:22
 *      Author: John Stark
 */

#include "medTestApplication.h"
#include <stdexcept>

medTestApplication::medTestApplication ( int & argc, char ** argv )
   : QCoreApplication(argc, argv)
{

}

medTestApplication::~medTestApplication()
{

}

//static
void medTestApplication::CheckTestResult( bool condition, const char *cond, const char *file, const int line /*= 0*/ )
{
    if (!condition) {
        QString msg = QString("%1(%2): Test failed (%3)").arg(file).arg(line).arg(cond);
        throw std::runtime_error(msg.toStdString());
    }
}


