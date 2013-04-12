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


