// includes

#include "dcmtkNode.h"
#include "SpeedTest.h" 
#include "dcmtkLogger.h"
#include "LoggerConsoleOutput.h"

#include <sstream>



void move(int group, int elem, const char* query)
{
 
    
}

int main( int argc, char** argv )
{
	//initialize
	dcmtkLogger::startUp();	
	LoggerConsoleOutput *shellOut = new LoggerConsoleOutput();
	dcmtkLogger::addOutput(shellOut);
	
	dcmtkNode host;
	host.setTitle("SPEEDTEST");
	host.setIp("localhost");
	host.setPort(9000);

	dcmtkNode peer;
	peer.setTitle("CONQEUESTSVR1");
	peer.setIp("localhost");
	peer.setPort(5678);

	SpeedTest stest(host,peer);
	stest.moveStudyLevel("new*");
	
	// clean up
	dcmtkLogger::shutDown();
	delete shellOut;
	return EXIT_SUCCESS;
}
