#include "qtdcmDataSourceSerieInfoToolBox.h"

#include <QtDcmSerieInfoWidget.h>

class qtdcmDataSourceSerieInfoToolBoxPrivate
{
public:
  QWidget * parent;
  QtDcmSerieInfoWidget * serieInfoWidget;
};

qtdcmDataSourceSerieInfoToolBox::qtdcmDataSourceSerieInfoToolBox ( QWidget* parent ) : medToolBox ( parent ), d(new qtdcmDataSourceSerieInfoToolBoxPrivate)
{
  d->parent = parent;
  d->serieInfoWidget = new QtDcmSerieInfoWidget(this);
  d->serieInfoWidget->infosGroupBox->setTitle("");

  this->addWidget(d->serieInfoWidget);
  this->setTitle("Selected serie information");
}

qtdcmDataSourceSerieInfoToolBox::~qtdcmDataSourceSerieInfoToolBox()
{
    if (d)
        delete d;
    
    d = NULL;
}

QtDcmSerieInfoWidget* qtdcmDataSourceSerieInfoToolBox::getSerieInfoWidget()
{
  return d->serieInfoWidget;
}


