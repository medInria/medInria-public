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
#include <medProcessPresenterFactory.h>
#include <medAbstractSubtractImageProcess.h>

#include <medWidgetsExport.h>

class MEDWIDGETS_EXPORT medAbstractSubtractImageProcessPresenter: public medAbstractArithmeticOperationProcessPresenter
{
    Q_OBJECT
public:
    medAbstractSubtractImageProcessPresenter(medAbstractSubtractImageProcess *parent)
        : medAbstractArithmeticOperationProcessPresenter(parent)
    {}
    virtual medAbstractSubtractImageProcess* process() const = 0;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractSubtractImageProcess, MEDWIDGETS_EXPORT)
