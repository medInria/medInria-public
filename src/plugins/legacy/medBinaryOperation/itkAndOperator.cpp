/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkAndOperator.h"

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

bool itkAndOperator::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkAndOperator", createitkAndOperator);
}

QString itkAndOperator::description() const
{
    return "AND";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkAndOperator()
{
    return new itkAndOperator;
}
