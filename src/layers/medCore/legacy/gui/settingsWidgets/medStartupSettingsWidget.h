#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include <medSettingsWidget.h>

#include <QCheckBox>
#include <QDialog>
#include <QComboBox>
#include <QWidget>

class medStartupSettingsWidgetPrivate;

class MEDCORE_EXPORT medStartupSettingsWidget : public medSettingsWidget
{
    Q_OBJECT

public :
    medStartupSettingsWidget(QWidget *parent = nullptr);

public slots:
    void read();
    void write();

private:
    QCheckBox *startInFullScreen;
    QComboBox *defaultStartingArea;
};
