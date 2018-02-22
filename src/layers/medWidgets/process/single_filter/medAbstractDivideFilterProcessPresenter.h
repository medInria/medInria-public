/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSingleFilterOperationDoubleProcessPresenter.h>
#include <medProcessPresenterFactory.h>
#include <medAbstractDivideFilterProcess.h>

#include <medWidgetsExport.h>

class MEDWIDGETS_EXPORT medAbstractDivideFilterProcessPresenter: public medAbstractSingleFilterOperationDoubleProcessPresenter
{
    Q_OBJECT
public:
    medAbstractDivideFilterProcessPresenter(medAbstractDivideFilterProcess *parent)
        : medAbstractSingleFilterOperationDoubleProcessPresenter(parent)
    {}
    virtual medAbstractDivideFilterProcess* process() const = 0;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractDivideFilterProcess, MEDWIDGETS_EXPORT)
