/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSingleFilterOperationProcessPresenter.h>
#include <medProcessPresenterFactory.h>
#include <medAbstractSymmetryPlaneAlignmentProcess.h>

#include <medWidgetsExport.h>

class MEDWIDGETS_EXPORT medAbstractSymmetryPlaneAlignmentProcessPresenter: public medAbstractSingleFilterOperationProcessPresenter
{
    Q_OBJECT
public:
    medAbstractSymmetryPlaneAlignmentProcessPresenter(medAbstractSymmetryPlaneAlignmentProcess *parent)
        : medAbstractSingleFilterOperationProcessPresenter(parent)
    {}
    virtual medAbstractSymmetryPlaneAlignmentProcess* process() const = 0;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractSymmetryPlaneAlignmentProcess, MEDWIDGETS_EXPORT)
