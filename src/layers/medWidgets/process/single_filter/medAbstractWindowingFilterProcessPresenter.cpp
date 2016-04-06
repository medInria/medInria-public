/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractWindowingFilterProcessPresenter.h>

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>

#include <medIntParameter.h>
#include <medIntParameterPresenter.h>
#include <medDoubleParameter.h>
#include <medDoubleParameterPresenter.h>
#include <medViewContainerSplitter.h>
#include <medViewContainer.h>
#include <medDataManager.h>
#include <medAbstractImageView.h>
#include <medAbstractImageData.h>

class medAbstractWindowingFilterProcessPresenterPrivate
{
public:
    medDoubleParameterPresenter* minimumIntensityValue;
    medDoubleParameterPresenter* maximumIntensityValue;
    medDoubleParameterPresenter* minimumOutputIntensityValue;
    medDoubleParameterPresenter* maximumOutputIntensityValue;
};

medAbstractWindowingFilterProcessPresenter::medAbstractWindowingFilterProcessPresenter(medAbstractWindowingFilterProcess *parent)
    : medAbstractSingleFilterOperationProcessPresenter(parent), d(new medAbstractWindowingFilterProcessPresenterPrivate)
{
    d->minimumIntensityValue = new medDoubleParameterPresenter(parent->minimumIntensityValue());
    d->maximumIntensityValue = new medDoubleParameterPresenter(parent->maximumIntensityValue());
    d->minimumOutputIntensityValue = new medDoubleParameterPresenter(parent->minimumOutputIntensityValue());
    d->maximumOutputIntensityValue = new medDoubleParameterPresenter(parent->maximumOutputIntensityValue());
}

medAbstractWindowingFilterProcessPresenter::~medAbstractWindowingFilterProcessPresenter()
{
}

QWidget *medAbstractWindowingFilterProcessPresenter::buildToolBoxWidget()
{
    QWidget* tbWidget = medAbstractSingleFilterOperationProcessPresenter::buildToolBoxWidget();

    tbWidget->layout()->addWidget(d->minimumIntensityValue->buildWidget());
    tbWidget->layout()->addWidget(d->maximumIntensityValue->buildWidget());
    tbWidget->layout()->addWidget(d->minimumOutputIntensityValue->buildWidget());
    tbWidget->layout()->addWidget(d->maximumOutputIntensityValue->buildWidget());


    return tbWidget;
}
