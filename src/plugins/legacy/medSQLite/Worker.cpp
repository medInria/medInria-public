/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include "Worker.h"
#include <thread>

void Worker::sendSignalWithDelay(const int iRequestId)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    emit sendProgress(iRequestId, (int)medAbstractSource::eRequestStatus::pending);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    emit sendProgress(iRequestId, (int)medAbstractSource::eRequestStatus::finish);
}
