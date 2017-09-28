/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2017. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSingleFilterOperationProcessPresenter.h>
#include <medProcessPresenterFactory.h>
#include <medAbstractBiasCorrectionProcess.h>

#include <medWidgetsExport.h>

class MEDWIDGETS_EXPORT medAbstractBiasCorrectionProcessPresenter: public medAbstractSingleFilterOperationProcessPresenter
{
    Q_OBJECT
private:

public:
    medAbstractBiasCorrectionProcessPresenter(medAbstractBiasCorrectionProcess *parent);
    virtual QWidget* buildToolBoxWidget() = 0;
    virtual medAbstractBiasCorrectionProcess* process() const = 0;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractBiasCorrectionProcess, MEDWIDGETS_EXPORT)
