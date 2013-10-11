/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "qtdcmDataSourceSerieToolBox.h"

#include <QtDcmPreviewWidget.h>
#include <QtDcmSerieInfoWidget.h>
#include <QtDcmImportWidget.h>

#include <medPluginManager.h>

class qtdcmDataSourceSerieToolBoxPrivate
{
public:
    QWidget *parent;
    
    QWidget *main;
    QtDcmPreviewWidget * preview;
    QtDcmSerieInfoWidget * serieInfoWidget;
    QtDcmImportWidget * importWidget;
};

qtdcmDataSourceSerieToolBox::qtdcmDataSourceSerieToolBox ( QWidget* parent ) : medToolBox ( parent ), d ( new qtdcmDataSourceSerieToolBoxPrivate )
{
    d->parent = parent;
    
    d->main = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    d->main->setLayout(mainLayout);
    
    d->preview = new QtDcmPreviewWidget(d->main);
    d->serieInfoWidget = new QtDcmSerieInfoWidget(d->main);
    d->importWidget = new QtDcmImportWidget(d->main);
    
    mainLayout->addWidget(d->preview);
    mainLayout->addWidget(d->serieInfoWidget);
    mainLayout->addWidget(d->importWidget);
    
    this->addWidget(d->main);
    this->setTitle("Series preview and import");

    // Add about plugin
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "qtdcmDataSourcePlugin" );
    setAboutPluginButton ( plugin );
    setAboutPluginVisibility( true );
}

qtdcmDataSourceSerieToolBox::~qtdcmDataSourceSerieToolBox()
{
    if (d)
        delete d;
    
    d = NULL;
}

QtDcmPreviewWidget* qtdcmDataSourceSerieToolBox::getPreviewWidget()
{
  return d->preview;
}

QtDcmSerieInfoWidget* qtdcmDataSourceSerieToolBox::getSerieInfoWidget()
{
    return d->serieInfoWidget;
}

QtDcmImportWidget* qtdcmDataSourceSerieToolBox::getImportWidget()
{
    return d->importWidget;
}
