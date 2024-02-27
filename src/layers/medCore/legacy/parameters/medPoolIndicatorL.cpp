/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medPoolIndicatorL.h>

#include <QtGui>
#include <QtWidgets>

class medPoolIndiactorPrivate
{
public:
    QHBoxLayout *layout;
    QHash<QString,QLabel*> colorIndicators;
};

medPoolIndicatorL::medPoolIndicatorL(QWidget *parent):
    QWidget(parent), d(new medPoolIndiactorPrivate)
{
    d->layout = new QHBoxLayout;
    d->layout->setContentsMargins(0,0,0,0);
    this->setLayout(d->layout);

}

medPoolIndicatorL::~medPoolIndicatorL()
{
    delete d;
}


void medPoolIndicatorL::addColorIndicator(QColor color, QString description)
{
    QLabel *colorIndicator = new QLabel;
    QPixmap pix(10,10);
    pix.fill(color);
    colorIndicator->setPixmap(pix);
    colorIndicator->setToolTip(description);

    d->layout->addWidget(colorIndicator);
    d->colorIndicators.insert(color.name(), colorIndicator);
}

void medPoolIndicatorL::removeColorIndicator(QColor color)
{
    delete d->colorIndicators.take(color.name());
}

void medPoolIndicatorL::replaceColorIndicator(QColor oldColor, QColor newColor)
{
    QLabel *colorIndicator =  d->colorIndicators[oldColor.name()];
    if(colorIndicator)
    {
        QPixmap pix(10,10);
        pix.fill(newColor);
        colorIndicator->setPixmap(pix);
    }
}

