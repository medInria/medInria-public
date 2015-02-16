/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>

class QtDcmServersDicomSettingsWidget;
class medQtDcmDataSourceServersSettingsToolBoxPrivate;

class medQtDcmDataSourceServersSettingsToolBox : public medToolBox
{
Q_OBJECT
public:
    medQtDcmDataSourceServersSettingsToolBox ( QWidget* parent = 0 );
    ~medQtDcmDataSourceServersSettingsToolBox();

    QtDcmServersDicomSettingsWidget * getServersDicomSettingsWidget();
    
public slots:
  void onSaveServersSettings();
    
private:
  medQtDcmDataSourceServersSettingsToolBoxPrivate * d;
};


