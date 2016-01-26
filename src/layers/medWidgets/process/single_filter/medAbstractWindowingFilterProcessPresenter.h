/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSingleFilterOperationProcessPresenter.h>
#include <medProcessPresenterFactory.h>
#include <medAbstractWindowingFilterProcess.h>

#include <medWidgetsExport.h>

class medAbstractWindowingFilterProcessPresenterPrivate;

class MEDWIDGETS_EXPORT medAbstractWindowingFilterProcessPresenter: public medAbstractSingleFilterOperationProcessPresenter
{
    Q_OBJECT
public:
    medAbstractWindowingFilterProcessPresenter(medAbstractWindowingFilterProcess *parent);
    virtual ~medAbstractWindowingFilterProcessPresenter();
    virtual QWidget* buildToolBoxWidget();
    virtual medAbstractWindowingFilterProcess* process() const = 0;

    const QScopedPointer<medAbstractWindowingFilterProcessPresenterPrivate> d;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractWindowingFilterProcess, MEDWIDGETS_EXPORT)
