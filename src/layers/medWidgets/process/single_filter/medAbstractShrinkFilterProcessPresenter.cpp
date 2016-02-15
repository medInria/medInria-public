/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractShrinkFilterProcessPresenter.h>

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

class medAbstractShrinkFilterProcessPresenterPrivate
{
public:
    medIntParameterPresenter* shrinkFactor1;
    medIntParameterPresenter* shrinkFactor2;
    medIntParameterPresenter* shrinkFactor3;
};


medAbstractShrinkFilterProcessPresenter::medAbstractShrinkFilterProcessPresenter(medAbstractShrinkFilterProcess *parent)
    : medAbstractSingleFilterOperationProcessPresenter(parent), d(new medAbstractShrinkFilterProcessPresenterPrivate)
{
    d->shrinkFactor1 = new medIntParameterPresenter(parent->shrinkFactor1());
    d->shrinkFactor2 = new medIntParameterPresenter(parent->shrinkFactor2());
    d->shrinkFactor3 = new medIntParameterPresenter(parent->shrinkFactor3());
}

medAbstractShrinkFilterProcessPresenter::~medAbstractShrinkFilterProcessPresenter()
{
}

QWidget *medAbstractShrinkFilterProcessPresenter::buildToolBoxWidget()
{
    QWidget* tbWidget = medAbstractSingleFilterOperationProcessPresenter::buildToolBoxWidget();

    tbWidget->layout()->addWidget(d->shrinkFactor1->buildWidget());
    tbWidget->layout()->addWidget(d->shrinkFactor2->buildWidget());
    tbWidget->layout()->addWidget(d->shrinkFactor3->buildWidget());


    return tbWidget;
}
