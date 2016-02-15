/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSingleFilterOperationDoubleProcessPresenter.h>
#include <medProcessPresenterFactory.h>
#include <medAbstractShrinkFilterProcess.h>

#include <medWidgetsExport.h>

class medAbstractShrinkFilterProcessPresenterPrivate;
class MEDWIDGETS_EXPORT medAbstractShrinkFilterProcessPresenter: public medAbstractSingleFilterOperationProcessPresenter
{
    Q_OBJECT
public:
    medAbstractShrinkFilterProcessPresenter(medAbstractShrinkFilterProcess *parent);
    virtual ~medAbstractShrinkFilterProcessPresenter();
    virtual QWidget* buildToolBoxWidget();
    virtual medAbstractShrinkFilterProcess* process() const = 0;

        const QScopedPointer<medAbstractShrinkFilterProcessPresenterPrivate> d;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractShrinkFilterProcess, MEDWIDGETS_EXPORT)
