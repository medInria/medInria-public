/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTriggerParameter.h>
#include <medTriggerParameterPresenter.h>

#include <QIcon>
#include <QPushButton>

class medTriggerParameterPresenterPrivate
{
public:
    QIcon icon;
    QSize iconSize;

    medTriggerParameter *parameter;
};


medTriggerParameterPresenter::medTriggerParameterPresenter(medTriggerParameter *parent): medAbstractParameterPresenter(parent), d(new medTriggerParameterPresenterPrivate)
{
    d->parameter = parent;
    
    d->icon = QIcon();
    d->iconSize = QSize(15,15);
}

medTriggerParameterPresenter::~medTriggerParameterPresenter()
{
    delete d;
}

void medTriggerParameterPresenter::setIcon(QIcon& icon)
{
    d->icon = icon;
}

void medTriggerParameterPresenter::setIconSize(QSize& size)
{
    d->iconSize = size;
}

QWidget* medTriggerParameterPresenter::buildWidget()
{
    QWidget *poWidgetRes = nullptr;
    switch (d->parameter->defaultRepresentation())
    {
    case 0:
    default:
        poWidgetRes = this->buildPushButton(); break;
    }
    return poWidgetRes;
}

QPushButton* medTriggerParameterPresenter::buildPushButton()
{
    QPushButton *poButtonRes = new QPushButton;

        poButtonRes->setCheckable(false);
        poButtonRes->setText(d->parameter->caption());
        if(!d->icon.isNull())
        {
            poButtonRes->setIcon(d->icon);
            poButtonRes->setIconSize(d->iconSize);
            poButtonRes->setMinimumSize(d->iconSize);
        }


        this->_connectWidget(poButtonRes);
        connect(poButtonRes, &QPushButton::pressed, [=]() {emit d->parameter->pushed(true);  } );
        connect(poButtonRes, &QPushButton::pressed, [=]() {emit d->parameter->pushed(false); } );
        connect(poButtonRes, &QPushButton::clicked, d->parameter, &medTriggerParameter::trigger);

        connect(d->parameter, &medTriggerParameter::pushed, poButtonRes, &QPushButton::setDown);

    return poButtonRes;
}

medAbstractParameter * medTriggerParameterPresenter::parameter() const
{
    return d->parameter;
}
