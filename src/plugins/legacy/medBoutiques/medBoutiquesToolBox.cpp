/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medBoutiques.h>
#include <medBoutiquesToolBox.h>

#include <dtkCoreSupport/dtkAbstractDataFactory.h>
#include <dtkCoreSupport/dtkAbstractData.h>

#include <medAbstractImageData.h>

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <dtkCoreSupport/dtkAbstractProcess.h>
#include <dtkCoreSupport/dtkAbstractViewFactory.h>
#include <dtkCoreSupport/dtkAbstractView.h>
#include <dtkCoreSupport/dtkAbstractViewInteractor.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medRunnableProcess.h>
#include <medJobManagerL.h>
#include <medPluginManager.h>

#include <medToolBoxFactory.h>
#include <medAbstractSelectableToolBox.h>
#include <medSelectorToolBox.h>
#include <medProgressionStack.h>
#include <medDataManager.h>

#include <QtGui>

#include "boutiquesGUI/medBoutiquesFileHandler.h"
#include "boutiquesGUI/medBoutiquesInstaller.h"

class medBoutiquesToolBoxPrivate
{
public:

    medBoutiquesFileHandler *fileHandler;
    medBoutiquesSearchToolsWidget *searchToolsWidget;
    medBoutiquesInvocationWidget *invocationWidget;
    medBoutiquesExecutionWidget *executionWidget;

    medAbstractData *output;
    QList<QUuid> expectedUuids;
    bool importOutputSignalConnected = false;
};

medBoutiquesToolBox::medBoutiquesToolBox(QWidget *parent) : medAbstractSelectableToolBox(parent), d(new medBoutiquesToolBoxPrivate)
{
    // Check if the plugin was installed ("installed" is true in the settings)
    // Install if necessary: check that the commands "python" and "docker" work properly
    medBoutiquesInstaller::checkBoutiquesInstallation(this);

    d->fileHandler = new medBoutiquesFileHandler(this);
    d->searchToolsWidget = new medBoutiquesSearchToolsWidget(parent);
    d->invocationWidget = new medBoutiquesInvocationWidget(parent, d->searchToolsWidget, d->fileHandler);
    d->executionWidget = new medBoutiquesExecutionWidget(parent, d->searchToolsWidget, d->invocationWidget);
    d->output = nullptr;

    d->invocationWidget->hide();
    d->executionWidget->hide();

    connect(d->searchToolsWidget, &medBoutiquesSearchToolsWidget::toolSelected, d->invocationWidget, &medBoutiquesInvocationWidget::toolSelected);
    connect(d->searchToolsWidget, &medBoutiquesSearchToolsWidget::toolSelected, d->executionWidget, &medBoutiquesExecutionWidget::toolSelected);
    connect(d->searchToolsWidget, &medBoutiquesSearchToolsWidget::toolDeselected, d->invocationWidget, &medBoutiquesInvocationWidget::toolDeselected);
    connect(d->searchToolsWidget, &medBoutiquesSearchToolsWidget::toolDeselected, d->executionWidget, &medBoutiquesExecutionWidget::toolDeselected);
    connect(d->executionWidget, &medBoutiquesExecutionWidget::success, this, &medBoutiquesToolBox::executionSuccess);

    QWidget *centralWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(d->searchToolsWidget);
    layout->addWidget(d->invocationWidget);
    layout->addWidget(d->executionWidget);
    centralWidget->setLayout(layout);

    // Main toolbox:
    this->setTitle("Boutiques");
    this->addWidget(centralWidget);

    //enable about plugin. Construtor called after the plugin has been registered, go ahead call it.
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin(
                "medBoutiquesPlugin");
    setAboutPluginButton(plugin);
    setAboutPluginVisibility(true);

}

medBoutiquesToolBox::~medBoutiquesToolBox()
{
    delete d->fileHandler;
    delete d;
    d = nullptr;
}

bool medBoutiquesToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medBoutiquesToolBox>();
}


medAbstractData* medBoutiquesToolBox::processOutput()
{
    return d == nullptr ? nullptr : d->output;
}

medAbstractData* medBoutiquesToolBox::getInput()
{
     if(!this->selectorToolBox())
         return nullptr;

     return this->selectorToolBox()->data();
}

dtkPlugin* medBoutiquesToolBox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    return pm->plugin("medBoutiquesPlugin");
}

void medBoutiquesToolBox::update(medAbstractData *data)
{
    Q_UNUSED(data)
}

void medBoutiquesToolBox::executionSuccess(const QString &outputFileName)
{
    QUuid uuid = medDataManager::instance()->importPath(outputFileName, false);
    d->expectedUuids.append(uuid);
    connect(medDataManager::instance(), SIGNAL(dataImported(medDataIndex, QUuid)), this, SLOT(open_waitForImportedSignal(medDataIndex, QUuid)));

    if(!d->importOutputSignalConnected) {
        d->importOutputSignalConnected = true;
        connect(this, SIGNAL(success()), this->getWorkspace(), SLOT(importProcessOutput()), Qt::UniqueConnection);
    }
}

void medBoutiquesToolBox::open_waitForImportedSignal(medDataIndex index, QUuid uuid)
{
    if(d->expectedUuids.contains(uuid))
    {
        d->expectedUuids.removeAll(uuid);
        disconnect(medDataManager::instance(), SIGNAL(dataImported(medDataIndex,QUuid)), this, SLOT(open_waitForImportedSignal(medDataIndex,QUuid)));
        if (index.isValid())
        {
            d->output = medDataManager::instance()->retrieveData(index);
            emit success();
        }
    }
}


