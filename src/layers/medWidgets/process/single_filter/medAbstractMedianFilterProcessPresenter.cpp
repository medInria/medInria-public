/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractMedianFilterProcessPresenter.h>

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QFormLayout>

#include <medIntParameter.h>
#include <medIntParameterPresenter.h>

class medAbstractMedianFilterProcessPresenterPrivate
{
public:
    medIntParameterPresenter* radius;
};

medAbstractMedianFilterProcessPresenter::medAbstractMedianFilterProcessPresenter(medAbstractMedianFilterProcess *parent)
    : medAbstractSingleFilterOperationProcessPresenter(parent), d(new medAbstractMedianFilterProcessPresenterPrivate)
{
    d->radius = new medIntParameterPresenter(parent->radius());
}

medAbstractMedianFilterProcessPresenter::~medAbstractMedianFilterProcessPresenter()
{
}

QWidget *medAbstractMedianFilterProcessPresenter::buildToolBoxWidget()
{
    QWidget *tbWidget = new QWidget;
    QVBoxLayout *tbLayout = new QVBoxLayout;
    tbWidget->setLayout(tbLayout);

    QFormLayout *paramsLayout = new QFormLayout;
    paramsLayout->addRow(d->radius->parameter()->caption(),d->radius->buildWidget());
    tbLayout->addLayout(paramsLayout);

    tbLayout->addWidget(this->buildRunButton());
    tbLayout->addWidget(this->buildCancelButton());
    tbLayout->addWidget(this->progressionPresenter()->buildProgressBar());

    return tbWidget;
}
