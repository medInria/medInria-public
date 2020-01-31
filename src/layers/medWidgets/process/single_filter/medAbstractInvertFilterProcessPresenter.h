#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractInvertFilterProcess.h>
#include <medAbstractSingleFilterOperationDoubleProcessPresenter.h>
#include <medProcessPresenterFactory.h>
#include <medWidgetsExport.h>

class MEDWIDGETS_EXPORT medAbstractInvertFilterProcessPresenter: public medAbstractSingleFilterOperationProcessPresenter
{
    Q_OBJECT
public:
    medAbstractInvertFilterProcessPresenter(medAbstractInvertFilterProcess *parent)
        : medAbstractSingleFilterOperationProcessPresenter(parent)
    {}
    virtual medAbstractInvertFilterProcess* process() const = 0;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractInvertFilterProcess, MEDWIDGETS_EXPORT)
