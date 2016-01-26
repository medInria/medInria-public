/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSingleFilterOperationDoubleProcessPresenter.h>
#include <medProcessPresenterFactory.h>
#include <medAbstractSubtractFilterProcess.h>

#include <medWidgetsExport.h>

class MEDWIDGETS_EXPORT medAbstractSubtractFilterProcessPresenter: public medAbstractSingleFilterOperationDoubleProcessPresenter
{
    Q_OBJECT
public:
    medAbstractSubtractFilterProcessPresenter(medAbstractSingleFilterOperationDoubleProcess *parent)
        : medAbstractSingleFilterOperationDoubleProcessPresenter(parent)
    {}
    virtual medAbstractSubtractFilterProcess* process() const = 0;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractSubtractFilterProcess, MEDWIDGETS_EXPORT)
