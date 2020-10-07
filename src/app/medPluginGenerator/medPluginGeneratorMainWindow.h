/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtWidgets>

class medPluginGeneratorMainWindowPrivate;

class medPluginGeneratorMainWindow : public QMainWindow
{
    Q_OBJECT

public:
     medPluginGeneratorMainWindow(QWidget *parent = 0);
    ~medPluginGeneratorMainWindow();

public slots:
    void generate();

public slots:
    void about();
    void onOutputPathClicked();
    void onOutputPathChanged();
    void onPluginTypeChanged(int);
    void onNameChanged();
    void onDescriptionChanged();
    void onPluginLicenseChanged();
    void onGenerateOptionsChanged(bool);
    void update();
    void onSetDefaultPath();
private:
    medPluginGeneratorMainWindowPrivate *d;
};


