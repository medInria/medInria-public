/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTriggerParameter.h>

#include <QIcon>
#include <QPushButton>

class medTriggerParameterPrivate
{
public:
    QPushButton *pushButton;
    QIcon icon;
    QSize iconSize;
    QString text;

    ~medTriggerParameterPrivate() {delete pushButton;}
};


medTriggerParameter::   medTriggerParameter(QString name, QObject *parent):
    medAbstractTriggerParameter(name, parent),
    d(new medTriggerParameterPrivate)
{
     d->pushButton = NULL;
     d->text = QString();
     d->icon = QIcon();
     d->iconSize = QSize(15,15);
}

medTriggerParameter::~medTriggerParameter()
{
    delete d;
}

void medTriggerParameter::setButtonIcon(QIcon& icon)
{
    d->icon = icon;

    if(d->pushButton)
        d->pushButton->setIcon(d->icon);
}

void medTriggerParameter::setButtonIconSize(QSize& size)
{
    d->iconSize = size;
    if(d->pushButton)
    {
        d->pushButton->setIconSize(size);
        d->pushButton->setMinimumSize(size);
    }
}

void medTriggerParameter::setButtonText(QString value)
{
    d->text = value;

    if(d->pushButton)
        d->pushButton->setText(value);
}

QPushButton* medTriggerParameter::getPushButton()
{
    if(!d->pushButton)
    {
        d->pushButton = new QPushButton;
        d->pushButton->setCheckable(false);
        d->pushButton->setText(d->text);
        if(!d->icon.isNull())
        {
            d->pushButton->setIcon(d->icon);
            d->pushButton->setIconSize(d->iconSize);
            d->pushButton->setMinimumSize(d->iconSize);
        }

        this->addToInternWidgets(d->pushButton);
        connect(d->pushButton, SIGNAL(destroyed()), this, SLOT(removeInternPushButton()));
        connect(d->pushButton, SIGNAL(clicked()), this, SLOT(trigger()));
    }
    return d->pushButton;
}

QWidget* medTriggerParameter::getWidget()
{
    return this->getPushButton();
}

void medTriggerParameter::removeInternPushButton()
{
    this->removeFromInternWidgets(d->pushButton);
    d->pushButton = NULL;
}
