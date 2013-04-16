/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medSettingsWidget.h"

#include <QWidget>
#include <QtGui>


class medSettingsWidgetPrivate {

public:
    QWidget* Parent;
    QString section;
    medSettingsWidgetPrivate();
    ~medSettingsWidgetPrivate();
};

medSettingsWidgetPrivate::medSettingsWidgetPrivate()
{
    this->section = QString();
}

medSettingsWidgetPrivate::~medSettingsWidgetPrivate()
{
}

medSettingsWidget::medSettingsWidget(QWidget *parent) : QWidget(parent), d(new medSettingsWidgetPrivate())
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

}


bool medSettingsWidget::validate()
{
    return false;
}

void medSettingsWidget::setTabName(QString section)
{
    d->section = section;
}

QString medSettingsWidget::tabName()const
{
    return d->section;
}

void medSettingsWidget::read()
{
}



