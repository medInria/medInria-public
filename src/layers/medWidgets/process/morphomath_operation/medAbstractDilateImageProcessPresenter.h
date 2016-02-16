/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractMorphomathOperationProcessPresenter.h>
#include <medProcessPresenterFactory.h>
#include <medAbstractDilateImageProcess.h>

#include <medWidgetsExport.h>

class MEDWIDGETS_EXPORT medAbstractDilateImageProcessPresenter: public medAbstractMorphomathOperationProcessPresenter
{
    Q_OBJECT
public:
    medAbstractDilateImageProcessPresenter(medAbstractDilateImageProcess *parent)
        : medAbstractMorphomathOperationProcessPresenter(parent)
    {}
    virtual medAbstractDilateImageProcess* process() const = 0;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractDilateImageProcess, MEDWIDGETS_EXPORT)
