/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSettingsWidget.h>

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

/**
 * @brief Should not be used directly, the factory will instanciate all the settingsWidgets.
 *
 * @param parent Parent widget. Should be the medSettingsEditor
*/
medSettingsWidget::medSettingsWidget(QWidget *parent) : QWidget(parent), d(new medSettingsWidgetPrivate())
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

}

/**
 * @brief Validates the form before saving the data.
 *
 * @param void
 * @return bool true is succeeded, false otherwise.
*/
bool medSettingsWidget::validate()
{
    return false;
}

/**
 * @brief Sets the name of the tab appearing in the settingsEditor.
 * MUST be used by inherited classes.
 *
 * @param section
*/
void medSettingsWidget::setTabName(QString section)
{
    d->section = section;
}

/**
 * @brief Gets the name as it should appear in the settingsEditor.
 * @return QString
*/
QString medSettingsWidget::tabName()const
{
    return d->section;
}

void medSettingsWidget::read()
{
}



