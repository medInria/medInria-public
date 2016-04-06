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
#include <QFormLayout>

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
    QWidget *tbWidget = new QWidget;
    QVBoxLayout *tbLayout = new QVBoxLayout;
    tbWidget->setLayout(tbLayout);

    QFormLayout *paramsLayout = new QFormLayout;
    paramsLayout->addRow(d->parameterPresenter->parameter()->caption(),d->parameterPresenter->buildWidget());

    tbLayout->addLayout(paramsLayout);
    tbLayout->addWidget(this->buildRunButton());
    tbLayout->addWidget(this->buildCancelButton());
    tbLayout->addWidget(this->progressionPresenter()->buildProgressBar());

    return tbWidget;
}
