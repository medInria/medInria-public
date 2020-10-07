#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractAddImageProcess.h>
#include <medAbstractArithmeticOperationProcessPresenter.h>
#include <medProcessPresenterFactory.h>
#include <medWidgetsExport.h>

class MEDWIDGETS_EXPORT medAbstractAddImageProcessPresenter: public medAbstractArithmeticOperationProcessPresenter
{
    Q_OBJECT
public:
    medAbstractAddImageProcessPresenter(medAbstractAddImageProcess *parent)
        : medAbstractArithmeticOperationProcessPresenter(parent)
    {}
    virtual medAbstractAddImageProcess* process() const = 0;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractAddImageProcess, MEDWIDGETS_EXPORT)
