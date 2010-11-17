// includes

#include "dcmtkNode.h"
#include "SpeedTest.h" 
#include "dcmtkLogger.h"
#include "LoggerConsoleOutput.h"

#include <sstream>


int main( int argc, char** argv )
{
    std::string patientname = "new*";
    
    if ( argc > 1)
    {
        patientname = argv[1];
    }

    //initialize
    dcmtkLogger::startUp();
    LoggerConsoleOutput *shellOut = new LoggerConsoleOutput();
    shellOut->setLogLevel(LoggerLogLevel::ERRORLOG);
    dcmtkLogger::addOutput(shellOut);
    
    dcmtkNode host;
    host.setTitle("DCMTKTOOL");
    host.setIp("localhost");
    host.setPort(100);

    dcmtkNode peer;
    peer.setTitle("CONQUESTSRV1");
    peer.setIp("localhost");
    peer.setPort(5678);
    
    SpeedTest stest(host,peer);
    int time1 = 0;
    int time2 = 0;
    int time3 = 0;
    int time4 = 0;
    //time1 = stest.moveStudyLevel(patientname.c_str());
    //time2 = stest.moveSeriesLevel(patientname.c_str());
    time3 = stest.moveImageLevel(patientname.c_str(), true);
    //time4 = stest.moveImageLevel(patientname.c_str(), false);
    
    std::cout << "Time: " << time1 <<" | "<< time2 << " | " << time3 << " | " << time4 <<std::endl;
    std::cout << "Image move (batch) takes: " << (float) time3/time2 << " times longer than series and... " <<
        std::endl << (float) time3/time1 << " times longer than study move" << std::endl;
    std::cout << "Image move (iterative) takes: " << (float) time4/time2 << " times longer than series and... " <<
        std::endl << (float) time4/time1 << " times longer than study move" << std::endl;

    // clean up
    dcmtkLogger::shutDown();
    delete shellOut;
    std::string test;
    std::cin >> test;
    return EXIT_SUCCESS;
}
