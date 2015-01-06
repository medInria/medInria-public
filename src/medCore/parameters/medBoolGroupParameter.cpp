/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medBoolGroupParameter.h>

#include <QIcon>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>

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

    QBoxLayout::Direction checkBoxDirection;
    QBoxLayout::Direction radioButtonDirection;
    QBoxLayout::Direction pushButtonDirection;

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

medBoolGroupParameter::medBoolGroupParameter(QString name, QObject* parent):
    medAbstractGroupParameter(name, parent),
    d(new medBoolGroupParameterPrivate)
{
    d->pushButtonGroupWidget = NULL;
    d->checkBoxGroupWidget = NULL;
    d->radioButtonGroupWidget = NULL;

    d->pushButtonGroup = NULL;
    d->checkBoxGroup = NULL;
    d->radioButtonGroup = NULL;

    d->pushButtonLayout = NULL;
    d->checkBoxLayout = NULL;
    d->radioButtonLayout = NULL;

    d->checkBoxDirection = QBoxLayout::TopToBottom;
    d->radioButtonDirection = QBoxLayout::TopToBottom;
    d->pushButtonDirection = QBoxLayout::TopToBottom;
}

medBoolGroupParameter::~medBoolGroupParameter()
{
    delete d;
}

void medBoolGroupParameter::addParameter(medBoolParameter *parameter)
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
    d->pushButtonDirection = direction;

    if(d->pushButtonLayout)
        d->pushButtonLayout->setDirection(direction);
}

void medBoolGroupParameter::setRadioButtonDirection(QBoxLayout::Direction direction)
{
    d->radioButtonDirection = direction;

    if(d->radioButtonLayout)
        d->radioButtonLayout->setDirection(direction);
}

void medBoolGroupParameter::setCheckBoxDirection(QBoxLayout::Direction direction)
{
    d->checkBoxDirection = direction;

    if(d->checkBoxLayout)
        d->checkBoxLayout->setDirection(direction);
}

QWidget* medBoolGroupParameter::getPushButtonGroup()
{
    if(!d->pushButtonGroupWidget)
    {
        d->pushButtonGroupWidget = new QWidget;
        d->pushButtonLayout = new QBoxLayout(d->pushButtonDirection, d->pushButtonGroupWidget);
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
        d->checkBoxLayout = new QBoxLayout(d->checkBoxDirection, d->checkBoxGroupWidget);
        d->checkBoxGroup = new QButtonGroup;
        d->checkBoxGroup->setExclusive(true);

        foreach (medBoolParameter* parameter, d->parameters)
        {
            QHBoxLayout *pLayout = new QHBoxLayout;
            pLayout->addWidget(parameter->getLabel());
            pLayout->addWidget(parameter->getCheckBox());
            d->checkBoxGroup->addButton(parameter->getCheckBox());
            d->checkBoxLayout->addLayout(pLayout);
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
        d->radioButtonLayout = new QBoxLayout(d->radioButtonDirection, d->radioButtonGroupWidget);
        d->radioButtonGroup = new QButtonGroup;
        d->radioButtonGroup->setExclusive(true);

        foreach (medBoolParameter* parameter, d->parameters)
        {
            QHBoxLayout *pLayout = new QHBoxLayout;
            pLayout->addWidget(parameter->getLabel());
            pLayout->addWidget(parameter->getRadioButton());
            d->radioButtonGroup->addButton(parameter->getRadioButton());
            d->radioButtonLayout->addLayout(pLayout);
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
    d->pushButtonLayout = NULL;
}

void medBoolGroupParameter::removeInternCheckBoxGroup()
{
    this->removeFromInternWidgets(d->checkBoxGroupWidget);
    d->checkBoxGroupWidget = NULL;
    d->checkBoxLayout = NULL;
}

void medBoolGroupParameter::removeInternRadioButtonGroup()
{
    this->removeFromInternWidgets(d->radioButtonGroupWidget);
    d->radioButtonGroupWidget = NULL;
    d->radioButtonLayout = NULL;
}

void medBoolGroupParameter::trigger()
{
    //TODO: to check
    if(!d->parameters.isEmpty())
        d->parameters[0]->trigger();

}
