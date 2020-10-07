#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractImageDenoisingProcess.h>
#include <medAbstractSingleFilterOperationProcessPresenter.h>
#include <medProcessPresenterFactory.h>
#include <medWidgetsExport.h>

class MEDWIDGETS_EXPORT medAbstractImageDenoisingProcessPresenter: public medAbstractSingleFilterOperationProcessPresenter
{
    Q_OBJECT
public:
    medAbstractImageDenoisingProcessPresenter(medAbstractImageDenoisingProcess *parent)
        : medAbstractSingleFilterOperationProcessPresenter(parent)
    {}
    virtual medAbstractImageDenoisingProcess* process() const = 0;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractImageDenoisingProcess, MEDWIDGETS_EXPORT)
