/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractArithmeticOperationProcessPresenter.h>
#include <medProcessPresenterFactory.h>
#include <medAbstractDivideImageProcess.h>

#include <medWidgetsExport.h>

class MEDWIDGETS_EXPORT medAbstractDivideImageProcessPresenter: public medAbstractArithmeticOperationProcessPresenter
{
    Q_OBJECT
public:
    medAbstractDivideImageProcessPresenter(medAbstractDivideImageProcess *parent)
        : medAbstractArithmeticOperationProcessPresenter(parent)
    {}
    virtual medAbstractDivideImageProcess* process() const = 0;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractDivideImageProcess, MEDWIDGETS_EXPORT)
