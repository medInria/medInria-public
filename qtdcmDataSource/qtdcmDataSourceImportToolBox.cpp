#include "qtdcmDataSourceImportToolBox.h"

#include <QtDcmImportWidget.h>
#include <medPluginManager.h>

class qtdcmDataSourceImportToolBoxPrivate
{
public:
    QWidget * parent;
    QtDcmImportWidget * importWidget;
};

qtdcmDataSourceImportToolBox::qtdcmDataSourceImportToolBox ( QWidget* parent ) : medToolBox ( parent ), d ( new qtdcmDataSourceImportToolBoxPrivate )
{
    d->parent = parent;
    d->importWidget = new QtDcmImportWidget;

    this->setTitle ( "Import in database" );
    this->addWidget ( d->importWidget );

    // Add about plugin
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "qtdcmDataSourcePlugin" );
    setAboutPluginButton ( plugin );
    setAboutPluginVisibility( true );
}

QtDcmImportWidget* qtdcmDataSourceImportToolBox::getImportWidget()
{
    return d->importWidget;
}

