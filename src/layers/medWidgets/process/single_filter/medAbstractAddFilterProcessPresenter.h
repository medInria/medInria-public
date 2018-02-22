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
#include <medAbstractAddFilterProcess.h>

#include <medWidgetsExport.h>

class MEDWIDGETS_EXPORT medAbstractAddFilterProcessPresenter: public medAbstractSingleFilterOperationDoubleProcessPresenter
{
    Q_OBJECT
public:
    medAbstractAddFilterProcessPresenter(medAbstractAddFilterProcess *parent)
        : medAbstractSingleFilterOperationDoubleProcessPresenter(parent)
    {}
    virtual medAbstractAddFilterProcess* process() const = 0;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractAddFilterProcess, MEDWIDGETS_EXPORT)
