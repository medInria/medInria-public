/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medStringListParameter.h>
#include <medStringListParameterPresenter.h>

#include <QComboBox>


class medStringListParameterPresenterPrivate
{
public:
    medStringListParameter* parameter;
};

medStringListParameterPresenter::medStringListParameterPresenter(medStringListParameter* parent) :medAbstractParameterPresenter(parent), d(new medStringListParameterPresenterPrivate)
{
    d->parameter = parent;
}

medStringListParameterPresenter::~medStringListParameterPresenter()
{
}

medStringListParameter * medStringListParameterPresenter::parameter() const
{
    return d->parameter;
}

QWidget * medStringListParameterPresenter::buildWidget()
{
    QWidget *poWidgetRes = nullptr;
    switch (d->parameter->defaultRepresentation())
    {
    case 0:
    default:
        poWidgetRes = this->buildComboBox(); break;
    }
    return poWidgetRes;
}

QComboBox * medStringListParameterPresenter::buildComboBox()
{
    QComboBox *poComboBoxRes = new QComboBox;

    poComboBoxRes->setToolTip(d->parameter->description());
    poComboBoxRes->addItems(d->parameter->items());
    
    connect(poComboBoxRes, SIGNAL(currentIndexChanged(int)),                         d->parameter,  SLOT(setIndex(int)));
    connect(d->parameter,  SIGNAL(medStringListParameter::valueChanged(int const&)), poComboBoxRes, SLOT(setCurrentIndex(int)));
    connect(d->parameter, &medStringListParameter::compositionChanged, [=]() {poComboBoxRes->clear();  poComboBoxRes->addItems(d->parameter->items()); poComboBoxRes->setCurrentIndex(d->parameter->getIndex()); });
    this->_connectWidget(poComboBoxRes);

    return poComboBoxRes;
}
