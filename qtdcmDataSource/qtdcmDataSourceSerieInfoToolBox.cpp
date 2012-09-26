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
  d->serieInfoWidget = new QtDcmSerieInfoWidget;
  d->serieInfoWidget->infosGroupBox->setTitle("");

  this->addWidget(d->serieInfoWidget);
  this->setTitle("Selected serie information");
}

QtDcmSerieInfoWidget* qtdcmDataSourceSerieInfoToolBox::getSerieInfoWidget()
{
  return d->serieInfoWidget;
}


