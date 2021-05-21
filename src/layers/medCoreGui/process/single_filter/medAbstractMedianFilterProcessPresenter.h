#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractMedianFilterProcess.h>
#include <medAbstractSingleFilterOperationProcessPresenter.h>
#include <medProcessPresenterFactory.h>
#include <medWidgetsExport.h>

class medAbstractMedianFilterProcessPresenterPrivate;
class MEDWIDGETS_EXPORT medAbstractMedianFilterProcessPresenter: public medAbstractSingleFilterOperationProcessPresenter
{
    Q_OBJECT
public:
    medAbstractMedianFilterProcessPresenter(medAbstractMedianFilterProcess *parent);
    virtual ~medAbstractMedianFilterProcessPresenter();
    virtual QWidget* buildToolBoxWidget();
    virtual medAbstractMedianFilterProcess* process() const = 0;

private:
    const QScopedPointer<medAbstractMedianFilterProcessPresenterPrivate> d;
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractMedianFilterProcess, MEDWIDGETS_EXPORT)
