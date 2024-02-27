/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAsyncRequest.h"

#include <QDateTime>

#include <medDataHub.h>

asyncRequest::asyncRequest() { stampTimeout = QDateTime::currentSecsSinceEpoch() + REQUEST_TIME_OUT; needMedAbstractConversion = false; noLongerValid = false; }

asyncRequest::asyncRequest(const asyncRequest & rqst) { *this = rqst; }
