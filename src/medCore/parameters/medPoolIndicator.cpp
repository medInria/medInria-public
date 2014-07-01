/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medPoolIndicator.h>

#include <QtGui>

class medPoolIndiactorPrivate
{
public:
    QHBoxLayout *layout;
    QHash<QString,QLabel*> colorIndicators;
};

medPoolIndicator::medPoolIndicator(QWidget *parent):
    QWidget(parent), d(new medPoolIndiactorPrivate)
{
    d->layout = new QHBoxLayout;
    d->layout->setContentsMargins(0,0,0,0);
    this->setLayout(d->layout);

}

medPoolIndicator::~medPoolIndicator()
{
}


void medPoolIndicator::addColorIndicator(QColor color, QString description)
{
    QLabel *colorIndicator = new QLabel;
    QPixmap pix(10,10);
    pix.fill(color);
    colorIndicator->setPixmap(pix);
    colorIndicator->setToolTip(description);

    d->layout->addWidget(colorIndicator);
    d->colorIndicators.insert(color.name(), colorIndicator);
}

void medPoolIndicator::removeColorIndicator(QColor color)
{
    delete d->colorIndicators.take(color.name());
}

