/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medBoolGroupParameter.h"

#include <QIcon>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>

#include <medBoolParameter.h>

class medBoolGroupParameterPrivate
{
public:
    QWidget* pushButtonGroupWidget;
    QWidget* checkBoxGroupWidget;
    QWidget* radioButtonGroupWidget;

    QButtonGroup* pushButtonGroup;
    QButtonGroup* checkBoxGroup;
    QButtonGroup* radioButtonGroup;

    QList <medBoolParameter*> parameters;

    QBoxLayout* pushButtonLayout;
    QBoxLayout* checkBoxLayout;
    QBoxLayout* radioButtonLayout;

    ~medBoolGroupParameterPrivate()
    {
        delete pushButtonGroupWidget;
        delete pushButtonGroup;

        delete radioButtonGroupWidget;
        delete radioButtonGroup;

        delete checkBoxGroupWidget;
        delete checkBoxGroup;
    }
};

medBoolGroupParameter::medBoolGroupParameter(QString name):
    medAbstractGroupParameter(name),
    d(new medBoolGroupParameterPrivate)
{
    d->pushButtonGroupWidget = NULL;
    d->checkBoxGroupWidget = NULL;
    d->radioButtonGroupWidget = NULL;

    d->pushButtonGroup = NULL;
    d->checkBoxGroup = NULL;
    d->radioButtonGroup = NULL;

    d->pushButtonLayout = NULL;
    d->radioButtonLayout = NULL;
    d->checkBoxLayout = NULL;
}

medBoolGroupParameter::~medBoolGroupParameter()
{
    delete d;
}

void medBoolGroupParameter::addBoolParameter(medBoolParameter *parameter)
{
    d->parameters <<  parameter;

    if(d->radioButtonGroupWidget)
        d->radioButtonGroup->addButton(parameter->getRadioButton());

    if(d->checkBoxGroupWidget)
        d->checkBoxGroup->addButton(parameter->getCheckBox());

    if(d->pushButtonGroupWidget)
        d->pushButtonGroup->addButton(parameter->getPushButton());
}

void medBoolGroupParameter::removeBoolParameter(medBoolParameter *parameter)
{
    d->parameters.removeOne(parameter);

    if(d->radioButtonGroupWidget)
    {
        d->radioButtonGroup->removeButton(parameter->getRadioButton());
        d->radioButtonLayout->removeWidget(parameter->getRadioButton());
    }

    if(d->checkBoxGroupWidget)
    {
        d->checkBoxGroup->removeButton(parameter->getCheckBox());
        d->checkBoxLayout->removeWidget(parameter->getCheckBox());
    }

    if(d->pushButtonGroupWidget)
    {
        d->pushButtonGroup->removeButton(parameter->getPushButton());
        d->pushButtonLayout->removeWidget(parameter->getPushButton());
    }

}

QList <medAbstractParameter*> medBoolGroupParameter::parametersCandidateToPool() const
{
    QList <medAbstractParameter*> parameterCandidateToPool;
    QList <medBoolParameter*>::Iterator it;
    for(it = d->parameters.begin(); it != d->parameters.end(); ++it)
        parameterCandidateToPool << *it;

    return parameterCandidateToPool;
}

void medBoolGroupParameter::setPushButtonDirection(QBoxLayout::Direction direction)
{
    if(!d->pushButtonGroupWidget)
        return;
    d->pushButtonLayout->setDirection(direction);
}

void medBoolGroupParameter::setRadioButtonDirection(QBoxLayout::Direction direction)
{
    if(!d->radioButtonGroupWidget)
        return;
    d->radioButtonLayout->setDirection(direction);
}

void medBoolGroupParameter::setCheckBoxDirection(QBoxLayout::Direction direction)
{
    if(!d->checkBoxGroupWidget)
        return;
    d->checkBoxLayout->setDirection(direction);
}

QWidget* medBoolGroupParameter::getPushButtonGroup()
{
    if(!d->pushButtonGroupWidget)
    {
        d->pushButtonGroupWidget = new QWidget;
        d->pushButtonLayout = new QBoxLayout(QBoxLayout::TopToBottom, d->pushButtonGroupWidget);
        d->pushButtonGroup = new QButtonGroup;
        d->pushButtonGroup->setExclusive(true);

        foreach (medBoolParameter* parameter, d->parameters)
        {
            d->pushButtonGroup->addButton(parameter->getPushButton());
            d->pushButtonLayout->addWidget(parameter->getPushButton());
        }

        this->addToInternWidgets(d->pushButtonGroupWidget);
        connect(d->pushButtonGroupWidget, SIGNAL(destroyed()), this, SLOT(removeInternPushButtonGroup()));
    }
    return d->pushButtonGroupWidget;
}

QWidget* medBoolGroupParameter::getCheckBoxGroup()
{
    if(!d->checkBoxGroupWidget)
    {
        d->checkBoxGroupWidget = new QWidget;
        d->checkBoxLayout = new QBoxLayout(QBoxLayout::TopToBottom, d->checkBoxGroupWidget);
        d->checkBoxGroup = new QButtonGroup;
        d->checkBoxGroup->setExclusive(true);

        foreach (medBoolParameter* parameter, d->parameters)
        {
            d->checkBoxGroup->addButton(parameter->getCheckBox());
            d->checkBoxLayout->addWidget(parameter->getCheckBox());
        }

        this->addToInternWidgets(d->checkBoxGroupWidget);
        connect(d->checkBoxGroupWidget, SIGNAL(destroyed()), this, SLOT(removeInternCheckBoxGroup()));
    }
    return d->checkBoxGroupWidget;
}

QWidget* medBoolGroupParameter::getRadioButtonGroup()
{
    if(!d->radioButtonGroupWidget)
    {
        d->radioButtonGroupWidget = new QWidget;
        d->radioButtonLayout = new QBoxLayout(QBoxLayout::TopToBottom, d->radioButtonGroupWidget);
        d->radioButtonGroup = new QButtonGroup;
        d->radioButtonGroup->setExclusive(true);

        foreach (medBoolParameter* parameter, d->parameters)
        {
            d->radioButtonGroup->addButton(parameter->getRadioButton());
            d->radioButtonLayout->addWidget(parameter->getRadioButton());
        }

        this->addToInternWidgets(d->radioButtonGroupWidget);
        connect(d->radioButtonGroupWidget, SIGNAL(destroyed()), this, SLOT(removeInternRadioButtonGroup()));
    }
    return d->radioButtonGroupWidget;
}

QWidget* medBoolGroupParameter::getWidget()
{
    return this->getRadioButtonGroup();
}


void medBoolGroupParameter::removeInternPushButtonGroup()
{
    this->removeFromInternWidgets(d->pushButtonGroupWidget);
    d->pushButtonGroupWidget = NULL;
}

void medBoolGroupParameter::removeInternCheckBoxGroup()
{
    this->removeFromInternWidgets(d->checkBoxGroupWidget);
    d->checkBoxGroupWidget = NULL;
}

void medBoolGroupParameter::removeInternRadioButtonGroup()
{
    this->removeFromInternWidgets(d->radioButtonGroupWidget);
    d->radioButtonGroupWidget = NULL;
}

