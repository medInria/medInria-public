/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medPoolIndicator.h>

#include <QComboBox>

#include <medStringListParameter.h>

class medPoolIndiactorPrivate
{
public:
    medStringListParameter *linkParameter;
};

medPoolIndicator::medPoolIndicator(QWidget *parent):
    QLabel(parent), d(new medPoolIndiactorPrivate)
{
    d->linkParameter = NULL;
}

medPoolIndicator::~medPoolIndicator()
{
}

void medPoolIndicator::setLinkParameter(medStringListParameter *linkParameter)
{
    d->linkParameter = linkParameter;
    this->update();

    if(d->linkParameter)
    {
        connect(d->linkParameter, SIGNAL(valueChanged(QString)), this, SLOT(update()));
        connect(d->linkParameter, SIGNAL(destroyed()), this, SLOT(removeInternLinkParamater()));
    }
}

void medPoolIndicator::update()
{
    if(!d->linkParameter)
    {
        this->clear();
    }
    else
    {
        QComboBox *cb = d->linkParameter->getComboBox();
        QPixmap pixmap = cb->itemIcon(cb->currentIndex()).pixmap(15,15);
        this->setPixmap(pixmap);
    }
}

void medPoolIndicator::removeInternLinkParamater()
{
    d->linkParameter = NULL;
    this->update();
}
