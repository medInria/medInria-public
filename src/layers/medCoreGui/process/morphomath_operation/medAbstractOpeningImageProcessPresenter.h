#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractOpeningImageProcess.h>
#include <medAbstractMorphomathOperationProcessPresenter.h>
#include <medProcessPresenterFactory.h>
#include <medCoreGuiExport.h>

class MEDCOREGUI_EXPORT medAbstractOpeningImageProcessPresenter: public medAbstractMorphomathOperationProcessPresenter
{
    Q_OBJECT
public:
    medAbstractOpeningImageProcessPresenter(medAbstractOpeningImageProcess *parent)
        : medAbstractMorphomathOperationProcessPresenter(parent)
    {}
    virtual medAbstractOpeningImageProcess* process() const = 0;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractOpeningImageProcess, MEDCOREGUI_EXPORT)
