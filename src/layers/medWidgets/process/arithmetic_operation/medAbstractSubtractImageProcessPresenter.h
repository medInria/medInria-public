#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractArithmeticOperationProcessPresenter.h>
#include <medAbstractSubtractImageProcess.h>
#include <medProcessPresenterFactory.h>
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
