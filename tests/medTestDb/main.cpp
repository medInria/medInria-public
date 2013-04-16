/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medTestDbApp.h"

#include<dtkLog/dtkLog.h>


int main(int argc, char *argv[])
{
    qRegisterMetaType<medDataIndex>("medDataIndex");
    int ret = DTK_FAILURE;
//    try {
        medTestDbApp theTest(argc,argv);
        theTest.run();
        medTestDbApp::exec();
        ret = theTest.getTestResult();
//    }
//    catch ( std::exception & e ){
//        dtkDebug() << "Exception caught : "<< e.what();
//        ret = DTK_FAILURE;
//    }
//    catch ( ... ){
//        dtkDebug() << "Exception caught : " << "[Unknown exception type]";
//        ret = DTK_FAILURE;
//    }

    return ret;
}

