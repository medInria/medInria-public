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
#include <medFilteringSelectorToolBox.h>
#include <medFilteringAbstractToolBox.h>
#include <medProgressionStack.h>
#include <medDataManager.h>

#include <QtGui>

#include "boutiquesGUI/medBoutiquesFileHandler.h"

class medBoutiquesToolBoxPrivate
{
public:

    medBoutiquesSearchToolsWidget *searchToolsWidget;
    medBoutiquesInvocationWidget *invocationWidget;
    medBoutiquesExecutionWidget *executionWidget;
    medAbstractData *output;
    QList<QUuid> expectedUuids;
};

medBoutiquesToolBox::medBoutiquesToolBox(QWidget *parent) : medFilteringAbstractToolBox(parent), d(new medBoutiquesToolBoxPrivate)
{
    // Check if the plugin was installed ("installed" is true in the settings)
    // Install if necessary: check that the commands "python" and "docker" work properly
    this->checkBoutiquesInstallation();

    d->searchToolsWidget = new medBoutiquesSearchToolsWidget(parent);
    d->invocationWidget = new medBoutiquesInvocationWidget(parent, d->searchToolsWidget, new medBoutiquesFileHandler(this));
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
     if(!this->parentToolBox())
         return nullptr;

     return this->parentToolBox()->data();
}

// void medBoutiquesToolBox::run()
// {
//     if(!this->parentToolBox())
//         return;

//     d->process = dtkAbstractProcessFactory::instance()->create("medBoutiquesGaussianBlur");

//     if(!this->parentToolBox()->data())
//         return;

//     d->process->setInput(this->parentToolBox()->data());

//     d->process->setParameter(d->variance->text().toDouble(),0);

//     medRunnableProcess *runProcess = new medRunnableProcess;
//     runProcess->setProcess (d->process);

//     d->progression_stack->addJobItem(runProcess, tr("Progress:"));

//     connect (runProcess, SIGNAL (success  (QObject*)),  this, SIGNAL (success ()));
//     connect (runProcess, SIGNAL (failure  (QObject*)),  this, SIGNAL (failure ()));

//     medJobManagerL::instance()->registerJobItem(runProcess);
//     QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));

// }

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
}

void medBoutiquesToolBox::open_waitForImportedSignal(medDataIndex index, QUuid uuid)
{
    if(d->expectedUuids.contains(uuid)) {
        d->expectedUuids.removeAll(uuid);
        disconnect(medDataManager::instance(), SIGNAL(dataImported(medDataIndex,QUuid)), this, SLOT(open_waitForImportedSignal(medDataIndex,QUuid)));
        if (index.isValid()) {
            d->output = medDataManager::instance()->retrieveData(index);
            emit success();
        }
    }
}


void medBoutiquesToolBox::checkBoutiquesInstallation()
{
    // Check the "installed" property in the settings
    QFile file(BoutiquesPaths::Settings());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // If can't open the settings: install boutiques
        file.close();
        this->installBoutiques();
    } else {
        QJsonDocument jsonDocument(QJsonDocument::fromJson(file.readAll()));
        QJsonObject settings = jsonDocument.object();

        // If "installed" is null or false: install boutiques
        if(!settings.contains("installed") || !settings["installed"].toBool())
        {
            file.close();
            this->installBoutiques(&settings);
        }
    }
}

void medBoutiquesToolBox::setBoutiquesInstalled(QJsonObject *settings)
{
    // Write the "installed" property in settings
    QFile file(BoutiquesPaths::Settings());
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        if(settings != nullptr)
        {
            (*settings)["installed"] = true;
            QJsonDocument settingsDocument(*settings);
            file.write(settingsDocument.toJson());
        }
        else
        {
            QJsonObject settings;
            settings["installed"] = true;
            QJsonDocument settingsDocument(settings);
            file.write(settingsDocument.toJson());
        }
    }
}

bool medBoutiquesToolBox::isPythonWorking(const QString &version)
{
    // Check that the python command is working ("python" on Linux and OS X, and "BoutiquesGUI-Data/python/python.exe" on Windows)
    QProcess pythonProcess(this);
    pythonProcess.start(BoutiquesPaths::Python(), {"--version"});
    pythonProcess.waitForFinished();
    QString output = QString::fromUtf8(pythonProcess.readAllStandardOutput());
    QString error = QString::fromUtf8(pythonProcess.readAllStandardError());
    QString pythonVersion = "Python " + version;
    return output.contains(pythonVersion) || error.contains(pythonVersion);
}

bool medBoutiquesToolBox::isDockerWorking()
{
    // Check that the docker command is working
    QProcess dockerProcess(this);
    dockerProcess.start(BoutiquesPaths::Docker(), {"--version"});
    dockerProcess.waitForFinished();
    QString output = QString::fromUtf8(dockerProcess.readAllStandardOutput());
    QString error = QString::fromUtf8(dockerProcess.readAllStandardError());
    QString dockerVersion = "Docker version";
    return output.contains(dockerVersion) || error.contains(dockerVersion);
}

void medBoutiquesToolBox::installBoutiques(QJsonObject *settings)
{
    bool pythonAndDockerAreWorking = true;

    if (QSysInfo::productType() == "winrt" || QSysInfo::productType() == "windows") {

        // On Windows:

        if(!this->isPythonWorking("3"))
        {
            // If "BoutiquesGUI-Data/python/python.exe" does not work:
            // Install visual studio redistributable (required for python3)

            QProcess installVisualStudioRedistributableProcess(this);
            installVisualStudioRedistributableProcess.start(BoutiquesPaths::VCRedis(), {"\\q"});

            if (!installVisualStudioRedistributableProcess.waitForFinished())
            {
                // If the install fails: ask the user to install it manullay
                pythonAndDockerAreWorking = false;
                QMessageBox::critical(this, "Could not install Microsoft Visual C++ Redistributable for Visual Studio", "Error while installing Microsoft Visual C++ Redistributable for Visual Studio.\nThis software is required to run python3 and boutiques under windows.\n\nTry to install it manually.");
            }
            else if(!this->isPythonWorking("3"))
            {
                // If the install succeeds but python still does not work: ask the user to install python
                pythonAndDockerAreWorking = false;
                QMessageBox::critical(this, "Python is not working", "Python.exe (" + BoutiquesPaths::Python() + ") is not working.\n\nYou need a working python3 version at this location to run boutiques tools.");
            }
        }

    } else {
        // On Linux:

        if(!this->isPythonWorking())
        {
            // If python does not work: ask the user to install it
            pythonAndDockerAreWorking = false;
            QMessageBox::critical(this, "Could not run Python", "Error while testing Python.\nInstall python 2.7 or 3 to run boutiques tools.");
        }
    }

    if (!this->isDockerWorking())
    {
        // If docker does not work: ask the user to install it
        pythonAndDockerAreWorking = false;
        QMessageBox::critical(this, "Could not run Docker", "Error while testing Docker.\nInstall Docker or Singularity to run boutiques tools.");
    }

    if(pythonAndDockerAreWorking)
    {
        // If python and docker work: set the "installed" setting to "true"
        this->setBoutiquesInstalled(settings);
    }
}
