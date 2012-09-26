#ifndef QTDCMDATASOURCEIMPORTTOOLBOX_H
#define QTDCMDATASOURCEIMPORTTOOLBOX_H

#include <medToolBox.h>

class QtDcmImportWidget;
class qtdcmDataSourceImportToolBoxPrivate;

class qtdcmDataSourceImportToolBox : public medToolBox
{
Q_OBJECT
public:
    qtdcmDataSourceImportToolBox ( QWidget* parent = 0 );

    QtDcmImportWidget * getImportWidget();

private:
  qtdcmDataSourceImportToolBoxPrivate * d;
};

#endif // QTDCMDATASOURCEIMPORTTOOLBOX_H