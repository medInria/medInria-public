/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medGuiExport.h"
#include "medSettingsWidget.h"
#include <QWidget>

class medInteractionSettingsWidgetPrivate;

class MEDGUI_EXPORT medInteractionSettingsWidget : public medSettingsWidget
{
    Q_OBJECT
public:
    medInteractionSettingsWidget(QWidget *parent = 0);

    QString identifier()  const;
    QString description() const;

signals:

public slots:
    void read();

protected:
    /**
     * @brief Performs the validation of each control inside the widget.
     *
     * @param void
     * @return true is the validation is successful, false otherwise.
    */
    bool validate();

    bool write();
private:
    medInteractionSettingsWidgetPrivate *d;

};
MEDGUI_EXPORT medSettingsWidget* createInteractionSettingsWidget(
        QWidget *parent);

