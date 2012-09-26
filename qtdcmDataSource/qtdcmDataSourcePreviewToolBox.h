#ifndef QTDCMDATASOURCEPREVIEWTOOLBOX_H
#define QTDCMDATASOURCEPREVIEWTOOLBOX_H

#include <medToolBox.h>

class QtDcmPreviewWidget;
class qtdcmDataSourcePreviewToolBoxPrivate;

class qtdcmDataSourcePreviewToolBox : public medToolBox
{
    Q_OBJECT
public:
    qtdcmDataSourcePreviewToolBox ( QWidget* parent = 0 );

    QtDcmPreviewWidget * getPreviewWidget();

private:
    qtdcmDataSourcePreviewToolBoxPrivate * d;
};

#endif // QTDCMDATASOURCEPREVIEWTOOLBOX_H
