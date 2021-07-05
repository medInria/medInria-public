/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medSQLite.h"

#include <cmath>

bool medSQlite::isWriteable()
{
    return true;
}

bool medSQlite::isLocal()
{
    return true;
}

bool medSQlite::isCached()
{
    return false;
}

bool medSQlite::isOnline()
{
    return false;
}

void medSQlite::abort(int pi_iRequest)
{

}

medSQlite::~medSQlite()
{

}
