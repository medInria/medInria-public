/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractArithmeticOperationProcessPresenter.h>

#include <medAbstractSubstractImageProcess.h>

class medAbstractSubstractImageProcessPresenter: public medAbstractArithmeticOperationProcessPresenter
{
public:
    medAbstractSubstractImageProcessPresenter(QObject *parent = NULL);

    medAbstractSubstractImageProcess* process() const = 0;
};
