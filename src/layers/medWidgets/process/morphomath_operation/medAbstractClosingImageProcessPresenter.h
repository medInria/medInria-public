/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractMorphomathOperationProcessPresenter.h>
#include <medProcessPresenterFactory.h>
#include <medAbstractClosingImageProcess.h>

#include <medWidgetsExport.h>

class MEDWIDGETS_EXPORT medAbstractClosingImageProcessPresenter: public medAbstractMorphomathOperationProcessPresenter
{
    Q_OBJECT
public:
    medAbstractClosingImageProcessPresenter(medAbstractClosingImageProcess *parent)
        : medAbstractMorphomathOperationProcessPresenter(parent)
    {}
    virtual medAbstractClosingImageProcess* process() const = 0;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractClosingImageProcess, MEDWIDGETS_EXPORT)
