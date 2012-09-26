#ifndef QTDCMDATASOURCESERVERSSETTINGSTOOLBOX_H
#define QTDCMDATASOURCESERVERSSETTINGSTOOLBOX_H

#include <medToolBox.h>

class QtDcmServersDicomSettingsWidget;
class qtdcmDataSourceServersSettingsToolBoxPrivate;

class qtdcmDataSourceServersSettingsToolBox : public medToolBox
{
Q_OBJECT
public:
    qtdcmDataSourceServersSettingsToolBox ( QWidget* parent = 0 );

    QtDcmServersDicomSettingsWidget * getServersDicomSettingsWidget();
    
public slots:
  void onSaveServersSettings();
    
private:
  qtdcmDataSourceServersSettingsToolBoxPrivate * d;
};

#endif // QTDCMDATASOURCESERVERSSETTINGSTOOLBOX_H
