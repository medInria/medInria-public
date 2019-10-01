/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <variationalSegmentation.h>

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

// /////////////////////////////////////////////////////////////////
// variationalSegmentation
// /////////////////////////////////////////////////////////////////

variationalSegmentation::variationalSegmentation() : dtkAbstractProcess()
{    
}

bool variationalSegmentation::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("variationalSegmentation", createVariationalSegmentation);
}

QString variationalSegmentation::description() const
{
    return "variationalSegmentation";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createVariationalSegmentation()
{
    return new variationalSegmentation;
}
