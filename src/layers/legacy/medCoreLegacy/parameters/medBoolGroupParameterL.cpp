/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include <medBoolGroupParameterL.h>

#include <QIcon>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>

#include <medBoolParameterL.h>

class medBoolGroupParameterLPrivate
{
public:
    QWidget* pushButtonGroupWidget;
    QWidget* checkBoxGroupWidget;
    QWidget* radioButtonGroupWidget;

    QButtonGroup* pushButtonGroup;
    QButtonGroup* checkBoxGroup;
    QButtonGroup* radioButtonGroup;

    QList <medBoolParameterL*> parameters;

    QBoxLayout::Direction checkBoxDirection;
    QBoxLayout::Direction radioButtonDirection;
    QBoxLayout::Direction pushButtonDirection;

    QBoxLayout* pushButtonLayout;
    QBoxLayout* checkBoxLayout;
    QBoxLayout* radioButtonLayout;

    ~medBoolGroupParameterLPrivate()
    {
        delete pushButtonGroupWidget;
        delete pushButtonGroup;

        delete radioButtonGroupWidget;
        delete radioButtonGroup;

        delete checkBoxGroupWidget;
        delete checkBoxGroup;
    }
};

medBoolGroupParameterL::medBoolGroupParameterL(QString name, QObject* parent):
    medAbstractGroupParameterL(name, parent),
    d(new medBoolGroupParameterLPrivate)
{
    d->pushButtonGroupWidget = nullptr;
    d->checkBoxGroupWidget = nullptr;
    d->radioButtonGroupWidget = nullptr;

    d->pushButtonGroup = nullptr;
    d->checkBoxGroup = nullptr;
    d->radioButtonGroup = nullptr;

    d->pushButtonLayout = nullptr;
    d->checkBoxLayout = nullptr;
    d->radioButtonLayout = nullptr;

    d->checkBoxDirection = QBoxLayout::TopToBottom;
    d->radioButtonDirection = QBoxLayout::TopToBottom;
    d->pushButtonDirection = QBoxLayout::TopToBottom;
}

medBoolGroupParameterL::~medBoolGroupParameterL()
{
    delete d;
}

void medBoolGroupParameterL::addParameter(medBoolParameterL *parameter)
{
    d->parameters <<  parameter;

    if(d->radioButtonGroupWidget)
        d->radioButtonGroup->addButton(parameter->getRadioButton());

    if(d->checkBoxGroupWidget)
        d->checkBoxGroup->addButton(parameter->getCheckBox());

    if(d->pushButtonGroupWidget)
        d->pushButtonGroup->addButton(parameter->getPushButton());
}

void medBoolGroupParameterL::removeBoolParameter(medBoolParameterL *parameter)
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

QList <medAbstractParameterL*> medBoolGroupParameterL::parametersCandidateToPool() const
{
    QList <medAbstractParameterL*> parameterCandidateToPool;
    QList <medBoolParameterL*>::Iterator it;
    for(it = d->parameters.begin(); it != d->parameters.end(); ++it)
        parameterCandidateToPool << *it;

    return parameterCandidateToPool;
}

void medBoolGroupParameterL::setPushButtonDirection(QBoxLayout::Direction direction)
{
    d->pushButtonDirection = direction;

    if(d->pushButtonLayout)
        d->pushButtonLayout->setDirection(direction);
}

void medBoolGroupParameterL::setRadioButtonDirection(QBoxLayout::Direction direction)
{
    d->radioButtonDirection = direction;

    if(d->radioButtonLayout)
        d->radioButtonLayout->setDirection(direction);
}

void medBoolGroupParameterL::setCheckBoxDirection(QBoxLayout::Direction direction)
{
    d->checkBoxDirection = direction;

    if(d->checkBoxLayout)
        d->checkBoxLayout->setDirection(direction);
}

QWidget* medBoolGroupParameterL::getPushButtonGroup()
{
    if(!d->pushButtonGroupWidget)
    {
        d->pushButtonGroupWidget = new QWidget;
        d->pushButtonLayout = new QBoxLayout(d->pushButtonDirection, d->pushButtonGroupWidget);
        d->pushButtonGroup = new QButtonGroup;
        d->pushButtonGroup->setExclusive(true);

        for(medBoolParameterL* parameter : d->parameters)
        {
            d->pushButtonGroup->addButton(parameter->getPushButton());
            d->pushButtonLayout->addWidget(parameter->getPushButton());
        }

        this->addToInternWidgets(d->pushButtonGroupWidget);
        connect(d->pushButtonGroupWidget, SIGNAL(destroyed()), this, SLOT(removeInternPushButtonGroup()));
    }
    return d->pushButtonGroupWidget;
}

QWidget* medBoolGroupParameterL::getCheckBoxGroup()
{
    if(!d->checkBoxGroupWidget)
    {
        d->checkBoxGroupWidget = new QWidget;
        d->checkBoxLayout = new QBoxLayout(d->checkBoxDirection, d->checkBoxGroupWidget);
        d->checkBoxGroup = new QButtonGroup;
        d->checkBoxGroup->setExclusive(true);

        for(medBoolParameterL* parameter : d->parameters)
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

QWidget* medBoolGroupParameterL::getRadioButtonGroup()
{
    if(!d->radioButtonGroupWidget)
    {
        d->radioButtonGroupWidget = new QWidget;
        d->radioButtonLayout = new QBoxLayout(d->radioButtonDirection, d->radioButtonGroupWidget);
        d->radioButtonGroup = new QButtonGroup;
        d->radioButtonGroup->setExclusive(true);

        for(medBoolParameterL* parameter : d->parameters)
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

QWidget* medBoolGroupParameterL::getWidget()
{
    return this->getRadioButtonGroup();
}


void medBoolGroupParameterL::removeInternPushButtonGroup()
{
    this->removeFromInternWidgets(d->pushButtonGroupWidget);
    d->pushButtonGroupWidget = nullptr;
    d->pushButtonLayout = nullptr;
}

void medBoolGroupParameterL::removeInternCheckBoxGroup()
{
    this->removeFromInternWidgets(d->checkBoxGroupWidget);
    d->checkBoxGroupWidget = nullptr;
    d->checkBoxLayout = nullptr;
}

void medBoolGroupParameterL::removeInternRadioButtonGroup()
{
    this->removeFromInternWidgets(d->radioButtonGroupWidget);
    d->radioButtonGroupWidget = nullptr;
    d->radioButtonLayout = nullptr;
}

void medBoolGroupParameterL::trigger()
{
    //TODO: to check
    if(!d->parameters.isEmpty())
        d->parameters[0]->trigger();

}

void medBoolGroupParameterL::toXMLNode(QDomDocument *doc, QDomElement *currentNode)
{
    medAbstractParameterL::toXMLNode(doc, currentNode);
    QDomElement groupNode = doc->createElement("group");
    QDomElement sizeNode = doc->createElement("size");
    sizeNode.appendChild(doc->createTextNode(QString::number(d->parameters.size())));
    
    for(int i=0; i<d->parameters.size(); i++)
    {
        QDomElement paramNode = doc->createElement("parameter");
        paramNode.setAttribute("n", i);
        d->parameters[i]->toXMLNode(doc, &paramNode);
        groupNode.appendChild(paramNode);
    }
    currentNode->appendChild(groupNode);
}
