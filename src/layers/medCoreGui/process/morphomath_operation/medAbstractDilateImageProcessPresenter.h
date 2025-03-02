#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractMorphomathOperationProcessPresenter.h>
#include <medProcessPresenterFactory.h>
#include <medAbstractDilateImageProcess.h>
#include <medCoreGuiExport.h>

class MEDCOREGUI_EXPORT medAbstractDilateImageProcessPresenter: public medAbstractMorphomathOperationProcessPresenter
{
    Q_OBJECT
public:
    medAbstractDilateImageProcessPresenter(medAbstractDilateImageProcess *parent)
        : medAbstractMorphomathOperationProcessPresenter(parent)
    {}
    virtual medAbstractDilateImageProcess* process() const = 0;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractDilateImageProcess, MEDCOREGUI_EXPORT)
