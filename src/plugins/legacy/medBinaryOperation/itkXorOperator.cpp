/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkXorOperator.h"

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

bool itkXorOperator::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkXorOperator", createitkXorOperator);
}

QString itkXorOperator::description() const
{
    return "XOR";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkXorOperator()
{
    return new itkXorOperator;
}
