#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSingleFilterOperationProcessPresenter.h>
#include <medAbstractWindowingFilterProcess.h>
#include <medProcessPresenterFactory.h>
#include <medCoreGuiExport.h>

class medAbstractWindowingFilterProcessPresenterPrivate;

class MEDCOREGUI_EXPORT medAbstractWindowingFilterProcessPresenter: public medAbstractSingleFilterOperationProcessPresenter
{
    Q_OBJECT
public:
    medAbstractWindowingFilterProcessPresenter(medAbstractWindowingFilterProcess *parent);
    virtual ~medAbstractWindowingFilterProcessPresenter();
    virtual QWidget* buildToolBoxWidget();
    virtual medAbstractWindowingFilterProcess* process() const = 0;

    const QScopedPointer<medAbstractWindowingFilterProcessPresenterPrivate> d;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractWindowingFilterProcess, MEDCOREGUI_EXPORT)
