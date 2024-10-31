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

#include <medCoreExport.h>
#include <medSettingsManager.h>

#include <QDialog>
#include <QLabel>
#include <QWidget>

class medStartupSettingsWidgetPrivate;

class MEDCORE_EXPORT medStartupSettingsWidget : public QDialog
{
    Q_OBJECT

public :
    medStartupSettingsWidget(QWidget *);

public slots:
    void read();
    void write();

protected:
    void readFullscreenSettings(medSettingsManager *);
    void readDefaultStartingArea(medSettingsManager *);
    void readDefaultSegmentationSpeciality(medSettingsManager *);
    void readDefaultTheme(medSettingsManager *);

private:
    medStartupSettingsWidgetPrivate *d;
};
