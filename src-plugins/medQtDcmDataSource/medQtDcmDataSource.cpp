/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medQtDcmDataSource.h>

#include <medDatabaseImporter.h>
#include <medDatabaseController.h>
#include <medJobManager.h>
#include <medAbstractDataSourceFactory.h>
#include <medToolBox.h>

#include <QtDcm.h>
#include <QtDcmLocalDicomSettingsWidget.h>
#include <QtDcmServersDicomSettingsWidget.h>
#include <QtDcmManager.h>
#include <QtDcmPreferences.h>

#include <dtkCore/dtkAbstractDataReader.h>
#include <medAbstractDataFactory.h>
#include <medAbstractData.h>

#include <medQtDcmDataSourceSerieToolBox.h>
#include <medQtDcmDataSourceServersSettingsToolBox.h>

// /////////////////////////////////////////////////////////////////
// medQtDcmDataSourcePrivate
// /////////////////////////////////////////////////////////////////

class medQtDcmDataSourcePrivate
{
public:
    QtDcm * mainWidget;
    QWidget * rightWidget;
    QtDcmLocalDicomSettingsWidget * localDicomSettingsWidget;
    QtDcmServersDicomSettingsWidget * serversDicomSettingsWidget;

    medQtDcmDataSourceSerieToolBox * serieToolBox;
    medQtDcmDataSourceServersSettingsToolBox * serversSettingsToolBox;

    QList <medToolBox *> additional_toolboxes;

    QPushButton * localSettingsButton;

    ~medQtDcmDataSourcePrivate();
};

medQtDcmDataSourcePrivate::~medQtDcmDataSourcePrivate()
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
// medQtDcmDataSource
// /////////////////////////////////////////////////////////////////

medQtDcmDataSource::medQtDcmDataSource() : medAbstractDataSource(), d ( new medQtDcmDataSourcePrivate )
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

    d->serieToolBox = new medQtDcmDataSourceSerieToolBox;
    d->serversSettingsToolBox = new medQtDcmDataSourceServersSettingsToolBox;
    d->serversSettingsToolBox->switchMinimize();

    d->additional_toolboxes.clear();
    d->additional_toolboxes.push_back ( d->serversSettingsToolBox );
    d->additional_toolboxes.push_back ( d->serieToolBox );
}

medQtDcmDataSource::~medQtDcmDataSource()
{
    delete d;
    d = NULL;
}

bool medQtDcmDataSource::registered()
{
    return medAbstractDataSourceFactory::instance()->registerDataSource ( "medQtDcmDataSource", createmedQtDcmDataSource );
}

QString medQtDcmDataSource::description() const
{
    return "Browse DICOM workstation";
}

QWidget *medQtDcmDataSource::mainViewWidget()
{
    if ( d->mainWidget )
        return d->mainWidget;

    initWidgets();
    return d->mainWidget;
}

QWidget *medQtDcmDataSource::sourceSelectorWidget()
{
    if ( d->rightWidget )
        return d->rightWidget;

    initWidgets();
    return d->rightWidget;
}

QString medQtDcmDataSource::tabName()
{
    return QString ( "PACS/CD" );
}

QList<medToolBox*> medQtDcmDataSource::getToolBoxes()
{
    return d->additional_toolboxes;
}

void medQtDcmDataSource::initWidgets()
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

void medQtDcmDataSource::onSerieMoved ( QString directory )
{
    emit dataToImportReceived ( directory );
}

void medQtDcmDataSource::onSaveLocalSettings()
{
    if ( d->localDicomSettingsWidget )
        d->localDicomSettingsWidget->updatePreferences();
}


// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

medAbstractDataSource *createmedQtDcmDataSource ( QWidget * )
{
    return new medQtDcmDataSource;
}
