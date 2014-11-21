/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medQtDcmDataSourceServersSettingsToolBox.h>

#include <QtDcmServersDicomSettingsWidget.h>

class medQtDcmDataSourceServersSettingsToolBoxPrivate
{
public:
    QWidget * parent;
    QtDcmServersDicomSettingsWidget * serversSettingsWidget;
};

medQtDcmDataSourceServersSettingsToolBox::medQtDcmDataSourceServersSettingsToolBox ( QWidget* parent ) : medToolBox ( parent ), d(new medQtDcmDataSourceServersSettingsToolBoxPrivate)
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

medQtDcmDataSourceServersSettingsToolBox::~medQtDcmDataSourceServersSettingsToolBox()
{
    if (d)
        delete d;
    
    d = NULL;
}

QtDcmServersDicomSettingsWidget* medQtDcmDataSourceServersSettingsToolBox::getServersDicomSettingsWidget()
{
  return d->serversSettingsWidget;
}

void medQtDcmDataSourceServersSettingsToolBox::onSaveServersSettings()
{
  d->serversSettingsWidget->updatePreferences();
}
