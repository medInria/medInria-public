#include "dcmtkBaseScp.h"


void dcmtkBaseScp::setConnectionParams(const char* ourTitle, const char* ourIP, unsigned short ourPort)
{
     opt_respondingAETitle = ourTitle;
     opt_port = ourPort;
}
