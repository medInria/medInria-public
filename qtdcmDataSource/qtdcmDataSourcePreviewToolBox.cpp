#include "qtdcmDataSourcePreviewToolBox.h"
#include <QtDcmPreviewWidget.h>

class qtdcmDataSourcePreviewToolBoxPrivate
{
public:
    QWidget *parent;
    QtDcmPreviewWidget * preview;
};

qtdcmDataSourcePreviewToolBox::qtdcmDataSourcePreviewToolBox ( QWidget* parent ) : medToolBox ( parent ), d ( new qtdcmDataSourcePreviewToolBoxPrivate )
{
    d->parent = parent;
    d->preview = new QtDcmPreviewWidget(this);
    d->preview->previewGroupBox->setTitle("");
    
    this->addWidget(d->preview);
    this->setTitle("Serie preview");
}

QtDcmPreviewWidget* qtdcmDataSourcePreviewToolBox::getPreviewWidget()
{
  return d->preview;
}
