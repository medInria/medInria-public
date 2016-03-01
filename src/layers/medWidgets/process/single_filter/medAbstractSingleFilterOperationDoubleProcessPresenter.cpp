/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSingleFilterOperationDoubleProcessPresenter.h>

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

class medAbstractSingleFilterOperationDoubleProcessPresenterPrivate
{
public:
    medDoubleParameterPresenter *parameterPresenter;
};

medAbstractSingleFilterOperationDoubleProcessPresenter::medAbstractSingleFilterOperationDoubleProcessPresenter(medAbstractSingleFilterOperationDoubleProcess *parent)
    : medAbstractSingleFilterOperationProcessPresenter(parent), d(new medAbstractSingleFilterOperationDoubleProcessPresenterPrivate)
{
    d->parameterPresenter = new medDoubleParameterPresenter(parent->doubleParameter());
}

medAbstractSingleFilterOperationDoubleProcessPresenter::~medAbstractSingleFilterOperationDoubleProcessPresenter()
{
}

QWidget *medAbstractSingleFilterOperationDoubleProcessPresenter::buildToolBoxWidget()
{
    QWidget* tbWidget = medAbstractSingleFilterOperationProcessPresenter::buildToolBoxWidget();

    tbWidget->layout()->addWidget(d->parameterPresenter->buildWidget());

    return tbWidget;
}
