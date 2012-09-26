#ifndef QTDCMDATASOURCESERIEINFOTOOLBOX_H
#define QTDCMDATASOURCESERIEINFOTOOLBOX_H

#include <medToolBox.h>

class QtDcmSerieInfoWidget;
class qtdcmDataSourceSerieInfoToolBoxPrivate;

class qtdcmDataSourceSerieInfoToolBox : public medToolBox
{
    Q_OBJECT
public:
    qtdcmDataSourceSerieInfoToolBox ( QWidget* parent = 0 );

    QtDcmSerieInfoWidget * getSerieInfoWidget();

private:
    qtdcmDataSourceSerieInfoToolBoxPrivate * d;
};

#endif // QTDCMDATASOURCESERIEINFOTOOLBOX_H
