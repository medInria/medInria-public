/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medQtDcmDataSourceSerieToolBox.h>

#include <QtDcmPreviewWidget.h>
#include <QtDcmSerieInfoWidget.h>
#include <QtDcmImportWidget.h>

#include <medPluginManager.h>

class medQtDcmDataSourceSerieToolBoxPrivate
{
public:
    QWidget *parent;
    
    QWidget *main;
    QtDcmPreviewWidget * preview;
    QtDcmSerieInfoWidget * serieInfoWidget;
    QtDcmImportWidget * importWidget;
};

medQtDcmDataSourceSerieToolBox::medQtDcmDataSourceSerieToolBox ( QWidget* parent ) : medToolBox ( parent ), d ( new medQtDcmDataSourceSerieToolBoxPrivate )
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
    dtkPlugin* plugin = pm->plugin ( "medQtDcmDataSourcePlugin" );
    setAboutPluginButton ( plugin );
    setAboutPluginVisibility( true );
}

medQtDcmDataSourceSerieToolBox::~medQtDcmDataSourceSerieToolBox()
{
    if (d)
        delete d;
    
    d = NULL;
}

QtDcmPreviewWidget* medQtDcmDataSourceSerieToolBox::getPreviewWidget()
{
  return d->preview;
}

QtDcmSerieInfoWidget* medQtDcmDataSourceSerieToolBox::getSerieInfoWidget()
{
    return d->serieInfoWidget;
}

QtDcmImportWidget* medQtDcmDataSourceSerieToolBox::getImportWidget()
{
    return d->importWidget;
}
