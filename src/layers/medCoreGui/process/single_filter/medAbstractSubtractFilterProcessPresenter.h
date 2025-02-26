#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSingleFilterOperationDoubleProcessPresenter.h>
#include <medAbstractSubtractFilterProcess.h>
#include <medProcessPresenterFactory.h>
#include <medCoreGuiExport.h>

class MEDCOREGUI_EXPORT medAbstractSubtractFilterProcessPresenter: public medAbstractSingleFilterOperationDoubleProcessPresenter
{
    Q_OBJECT
public:
    medAbstractSubtractFilterProcessPresenter(medAbstractSingleFilterOperationDoubleProcess *parent)
        : medAbstractSingleFilterOperationDoubleProcessPresenter(parent)
    {}
    virtual medAbstractSubtractFilterProcess* process() const = 0;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractSubtractFilterProcess, MEDCOREGUI_EXPORT)
