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
    
    QtDcmPreviewWidget * preview;
    QtDcmSerieInfoWidget * serieInfoWidget;
    QtDcmImportWidget * importWidget;
};

qtdcmDataSourceSerieToolBox::qtdcmDataSourceSerieToolBox ( QWidget* parent ) : medToolBox ( parent ), d ( new qtdcmDataSourceSerieToolBoxPrivate )
{
    d->parent = parent;
    d->preview = new QtDcmPreviewWidget(this);

    d->serieInfoWidget = new QtDcmSerieInfoWidget(this);
    d->importWidget = new QtDcmImportWidget(this);
    
    this->addWidget(d->preview);
    this->addWidget(d->serieInfoWidget);
    this->addWidget(d->importWidget);
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
