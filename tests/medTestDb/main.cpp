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

