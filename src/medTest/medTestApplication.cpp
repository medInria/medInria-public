/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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


