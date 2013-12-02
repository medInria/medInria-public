/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>

class QtDcmServersDicomSettingsWidget;
class qtdcmDataSourceServersSettingsToolBoxPrivate;

class qtdcmDataSourceServersSettingsToolBox : public medToolBox
{
Q_OBJECT
public:
    qtdcmDataSourceServersSettingsToolBox ( QWidget* parent = 0 );
    ~qtdcmDataSourceServersSettingsToolBox();

    QtDcmServersDicomSettingsWidget * getServersDicomSettingsWidget();
    
public slots:
  void onSaveServersSettings();
    
private:
  qtdcmDataSourceServersSettingsToolBoxPrivate * d;
};


