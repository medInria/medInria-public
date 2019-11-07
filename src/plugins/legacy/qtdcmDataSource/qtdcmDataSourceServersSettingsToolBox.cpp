/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <qtdcmDataSourceServersSettingsToolBox.h>

#include <QtDcmServersDicomSettingsWidget.h>
#include <QtDcmPreferences.h>

class qtdcmDataSourceServersSettingsToolBoxPrivate
{
public:
    QWidget * parent;
    QtDcmServersDicomSettingsWidget * serversSettingsWidget;
};

qtdcmDataSourceServersSettingsToolBox::qtdcmDataSourceServersSettingsToolBox ( QWidget* parent ) : medToolBox ( parent ), d(new qtdcmDataSourceServersSettingsToolBoxPrivate)
{
    d->parent = parent;
    d->serversSettingsWidget = new QtDcmServersDicomSettingsWidget ( this );

    d->serversSettingsWidget->treeWidget->setColumnWidth ( 0, 70 );
    d->serversSettingsWidget->treeWidget->setColumnWidth ( 1, 100 );
    d->serversSettingsWidget->treeWidget->setColumnWidth ( 2, 150 );

    d->serversSettingsWidget->treeWidget->setMinimumHeight(100);

    QPushButton * settingsButton = new QPushButton ( this );
    settingsButton->setText ( "Save settings" );
    settingsButton->setFocusPolicy ( Qt::NoFocus );
    settingsButton->setMaximumWidth ( 100 );
    QObject::connect ( settingsButton, SIGNAL ( clicked() ), this, SLOT ( onSaveServersSettings() ) );

    QHBoxLayout * buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget ( settingsButton );
    buttonLayout->addStretch();

    QVBoxLayout * layout = new QVBoxLayout;
    layout->addWidget ( d->serversSettingsWidget );
    layout->addLayout ( buttonLayout );
    layout->addStretch();

    QWidget * widget = new QWidget ( this );
    widget->setLayout ( layout );

    this->addWidget ( widget );
    this->setTitle ( "Dicom servers settings" );
}

qtdcmDataSourceServersSettingsToolBox::~qtdcmDataSourceServersSettingsToolBox()
{
    if (d)
        delete d;
    
    d = NULL;
}

QtDcmServersDicomSettingsWidget* qtdcmDataSourceServersSettingsToolBox::getServersDicomSettingsWidget()
{
    return d->serversSettingsWidget;
}

void qtdcmDataSourceServersSettingsToolBox::onSaveServersSettings()
{
    d->serversSettingsWidget->updatePreferences();
    QtDcmPreferences::instance()->writeSettings();
}
