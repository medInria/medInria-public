/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkOrOperator.h"

#include <dtkCore/dtkAbstractProcessFactory.h>


bool itkOrOperator::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkOrOperator", createitkOrOperator);
}

QString itkOrOperator::description() const
{
    return "OR";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkOrOperator()
{
    return new itkOrOperator;
}
