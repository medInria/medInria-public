/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "qtdcmDataSource.h"

#include <medSql/medDatabaseImporter.h>
#include <medSql/medDatabaseController.h>
#include <medCore/medJobManager.h>
#include <medCore/medAbstractDataSourceFactory.h>
#include <medToolBox.h>

#include <QtDcm.h>
#include <QtDcmLocalDicomSettingsWidget.h>
#include <QtDcmServersDicomSettingsWidget.h>
#include <QtDcmManager.h>
#include <QtDcmPreferences.h>

#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>

#include <qtdcmDataSourceSerieToolBox.h>
#include <qtdcmDataSourceServersSettingsToolBox.h>

// /////////////////////////////////////////////////////////////////
// qtdcmDataSourcePrivate
// /////////////////////////////////////////////////////////////////

class qtdcmDataSourcePrivate
{
public:
    QtDcm * mainWidget;
    QWidget * rightWidget;
    QtDcmLocalDicomSettingsWidget * localDicomSettingsWidget;
    QtDcmServersDicomSettingsWidget * serversDicomSettingsWidget;

    qtdcmDataSourceSerieToolBox * serieToolBox;
    qtdcmDataSourceServersSettingsToolBox * serversSettingsToolBox;

    QList <medToolBox *> additional_toolboxes;
    
    QPushButton * localSettingsButton;

    ~qtdcmDataSourcePrivate();
};

qtdcmDataSourcePrivate::~qtdcmDataSourcePrivate()
{
    if (mainWidget)
        delete mainWidget;
    
    if (rightWidget)
        delete rightWidget;
    
    if (localSettingsButton)
        delete localSettingsButton;
    
    qDeleteAll(additional_toolboxes);
}

// /////////////////////////////////////////////////////////////////
// qtdcmDataSource
// /////////////////////////////////////////////////////////////////

qtdcmDataSource::qtdcmDataSource() : medAbstractDataSource(), d ( new qtdcmDataSourcePrivate )
{
    d->mainWidget = NULL;
    d->rightWidget = NULL;

    // Create gui for the right Widget;
    d->localDicomSettingsWidget = new QtDcmLocalDicomSettingsWidget;

    d->localSettingsButton = new QPushButton;
    d->localSettingsButton->setText ( "Save settings" );
    d->localSettingsButton->setFocusPolicy ( Qt::NoFocus );
    d->localSettingsButton->setToolTip(tr("Save local Dicom settings"));
    d->localSettingsButton->setMaximumWidth ( 100 );
    QObject::connect ( d->localSettingsButton, SIGNAL ( clicked() ), this, SLOT ( onSaveLocalSettings() ) );

    QHBoxLayout * buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(d->localSettingsButton);
    buttonLayout->addStretch();

    QVBoxLayout * layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0); 
    layout->addWidget ( d->localDicomSettingsWidget );
    layout->addLayout(buttonLayout);
    layout->addStretch();

    d->rightWidget = new QWidget;
    d->rightWidget->setLayout ( layout );

    d->serieToolBox = new qtdcmDataSourceSerieToolBox;
    d->serversSettingsToolBox = new qtdcmDataSourceServersSettingsToolBox;
    d->serversSettingsToolBox->switchMinimize();

    d->additional_toolboxes.clear();
    d->additional_toolboxes.push_back ( d->serversSettingsToolBox );
    d->additional_toolboxes.push_back ( d->serieToolBox );
}

qtdcmDataSource::~qtdcmDataSource()
{
    delete d;
    d = NULL;
}

bool qtdcmDataSource::registered()
{
    return medAbstractDataSourceFactory::instance()->registerDataSource ( "qtdcmDataSource", createQtdcmDataSource );
}

QString qtdcmDataSource::description() const
{
    return "Browse DICOM workstation";
}

QWidget *qtdcmDataSource::mainViewWidget()
{
    if ( d->mainWidget )
        return d->mainWidget;

    initWidgets();
    return d->mainWidget;
}

QWidget *qtdcmDataSource::sourceSelectorWidget()
{
    if ( d->rightWidget )
        return d->rightWidget;

    initWidgets();
    return d->rightWidget;
}

QString qtdcmDataSource::tabName()
{
    return QString ( "Dicom" );
}

QList<medToolBox*> qtdcmDataSource::getToolBoxes()
{
    return d->additional_toolboxes;
}

void qtdcmDataSource::initWidgets()
{
    if ( d->mainWidget && d->rightWidget )
        return;

    if ( !d->mainWidget )
    {
        d->mainWidget = new QtDcm();
        QtDcmPreferences::instance()->setIniFile("medInria");
        QtDcmManager::instance()->setQtDcmWidget(d->mainWidget);
        QtDcmManager::instance()->setPreviewWidget ( d->serieToolBox->getPreviewWidget() );
        QtDcmManager::instance()->setImportWidget ( d->serieToolBox->getImportWidget() );
        QtDcmManager::instance()->setSerieInfoWidget ( d->serieToolBox->getSerieInfoWidget() );
        QtDcmManager::instance()->useConverter ( false );
        
        d->mainWidget->layout()->setContentsMargins(0,0,0,0);
        
        QObject::connect ( QtDcmManager::instance(), SIGNAL ( serieMoved ( QString ) ), this, SLOT ( onSerieMoved ( QString ) ) );

        d->localDicomSettingsWidget->setPreferences ( QtDcmPreferences::instance() );
        d->serversSettingsToolBox->getServersDicomSettingsWidget()->setPreferences(QtDcmPreferences::instance());
    }
}

void qtdcmDataSource::onSerieMoved ( QString directory )
{
    emit dataToImportReceived ( directory );
}

void qtdcmDataSource::onSaveLocalSettings()
{
    if ( d->localDicomSettingsWidget )
        d->localDicomSettingsWidget->updatePreferences();
}


// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

medAbstractDataSource *createQtdcmDataSource ( QObject* )
{
    return new qtdcmDataSource;
}
