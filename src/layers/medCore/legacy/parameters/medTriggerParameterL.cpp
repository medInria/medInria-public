/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTriggerParameterL.h>

#include <QIcon>
#include <QPushButton>

class medTriggerParameterLPrivate
{
public:
    QPushButton *pushButton;
    QIcon icon;
    QSize iconSize;
    QString text;

    ~medTriggerParameterLPrivate() {delete pushButton;}
};


medTriggerParameterL::medTriggerParameterL(QString name, QObject *parent):
    medAbstractTriggerParameterL(name, parent),
    d(new medTriggerParameterLPrivate)
{
     d->pushButton = nullptr;
     d->text = QString();
     d->icon = QIcon();
     d->iconSize = QSize(15,15);
}

medTriggerParameterL::~medTriggerParameterL()
{
    delete d;
}

void medTriggerParameterL::setButtonIcon(QIcon& icon)
{
    d->icon = icon;

    if(d->pushButton)
        d->pushButton->setIcon(d->icon);
}

void medTriggerParameterL::setButtonIconSize(QSize& size)
{
    d->iconSize = size;
    if(d->pushButton)
    {
        d->pushButton->setIconSize(size);
        d->pushButton->setMinimumSize(size);
    }
}

void medTriggerParameterL::setButtonText(QString value)
{
    d->text = value;

    if(d->pushButton)
        d->pushButton->setText(value);
}

QPushButton* medTriggerParameterL::getPushButton()
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

QWidget* medTriggerParameterL::getWidget()
{
    return this->getPushButton();
}

void medTriggerParameterL::removeInternPushButton()
{
    this->removeFromInternWidgets(d->pushButton);
    d->pushButton = nullptr;
}
