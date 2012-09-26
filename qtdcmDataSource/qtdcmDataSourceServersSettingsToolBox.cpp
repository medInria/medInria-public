#include "qtdcmDataSourceServersSettingsToolBox.h"

#include <QtDcmServersDicomSettingsWidget.h>

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

    d->serversSettingsWidget->treeWidget->setStyleSheet ( "alternate-background-color: #505050;\
                                                          border-top-width: 0px;\
                                                          border-left-width: 0px;\
                                                          border-right-width: 0px;\
                                                          border-bottom-width: 0px;\
                                                          padding-top: 0px;\
                                                          padding-left: 0px;\
                                                          padding-right: 0px;\
                                                          padding-bottom: 0px;\
                                                          font-size: 10px;\
                                                          color: #b2b8b2;\
                                                          background: #313131;" );

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

QtDcmServersDicomSettingsWidget* qtdcmDataSourceServersSettingsToolBox::getServersDicomSettingsWidget()
{
  return d->serversSettingsWidget;
}

void qtdcmDataSourceServersSettingsToolBox::onSaveServersSettings()
{
  d->serversSettingsWidget->updatePreferences();
}
