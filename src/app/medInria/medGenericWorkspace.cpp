/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medGenericWorkspace.h>

#include <medTabbedViewContainers.h>

#include <medCore.h>
#include <medWidgets.h>
#include <medProcessPresenterFactory.h>
#include <medAbstractProcess.h>
#include <medAbstractProcessPresenter.h>

#include <medAbstractOpeningImageProcess.h>
#include <medJobManager.h>

#include <medToolBox.h>
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>

#include <QDebug>

class medGenericWorkspacePrivate
{
public:
    medAbstractProcess *process;
    medAbstractProcessPresenter *presenter;

    QComboBox *processTypeComboBox;
    QComboBox *processSelectorComboBox;

    medToolBox *workspaceToolBox;
    QWidget *currentProcessToolBox;
};

medGenericWorkspace::medGenericWorkspace(QWidget *parent): medAbstractWorkspaceLegacy (parent), d(new medGenericWorkspacePrivate)
{
    d->presenter = NULL;
    d->process = NULL;

    QWidget *processTypeWidget = new QWidget;
    QLabel *processTypeLabel = new QLabel("Process Type", processTypeWidget);
    QHBoxLayout *processTypeLayout = new QHBoxLayout;
    processTypeLayout->addWidget(processTypeLabel);
    d->processTypeComboBox = new QComboBox;
    processTypeLayout->addWidget(d->processTypeComboBox);
    d->processTypeComboBox->addItem("None selected");
    d->processTypeComboBox->addItem("Generic processes");
    processTypeWidget->setLayout(processTypeLayout);

    connect(d->processTypeComboBox,SIGNAL(currentIndexChanged(int)),
            this,SLOT(setProcessType(int)));

    QWidget *processWidget = new QWidget;
    QLabel *processLabel = new QLabel("Process", processWidget);
    QHBoxLayout *processLayout = new QHBoxLayout;
    processLayout->addWidget(processLabel);
    d->processSelectorComboBox = new QComboBox;
    processLayout->addWidget(d->processSelectorComboBox);
    processWidget->setLayout(processLayout);

    connect(d->processSelectorComboBox, SIGNAL(currentIndexChanged(int)),
            this,SLOT(setProcessSelection(int)));

//    if(!plugins.isEmpty())
//    {
//        QString key = plugins.first();
//        d->process = medCore::tractography::pluginFactory().create(key);
//        d->presenter = medWidgets::tractography::presenterFactory().create(d->process);

    d->workspaceToolBox = new medToolBox;
    //d->workspaceToolBox->addWidget(d->presenter->buildToolBoxWidget());
    d->workspaceToolBox->setTitle("Process controller");
    d->workspaceToolBox->addWidget(processTypeWidget);
    d->workspaceToolBox->addWidget(processWidget);
    this->addToolBox(d->workspaceToolBox);
}

medGenericWorkspace::~medGenericWorkspace()
{
    delete d;
    d = NULL;
}

void medGenericWorkspace::setProcessType(int index)
{
    if (index == 0)
        return;

    ProcessTypes tProc = (ProcessTypes)index;

    d->processSelectorComboBox->blockSignals(true);
    d->processSelectorComboBox->clear();
    d->processSelectorComboBox->addItem("None selected");

    switch (tProc)
    {
        case Generic:
        default:
        {
            QStringList plugins = medCore::generic::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::generic::pluginFactory().create(pluginKey);
                if (process)
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
            }

            break;
        }
    }

    d->processSelectorComboBox->blockSignals(false);
}

void medGenericWorkspace::setProcessSelection(int index)
{
    if (index == 0)
        return;

    ProcessTypes tProc = (ProcessTypes)d->processTypeComboBox->currentIndex();
    QString pluginKey = d->processSelectorComboBox->itemData(index).toString();

    switch (tProc)
    {
        case Generic:
        {
            medAbstractProcess *process = medCore::generic::pluginFactory().create(pluginKey);
            d->process = process;
            d->presenter = medWidgets::generic::presenterFactory().create(process);

            break;
        }
    }


    d->workspaceToolBox->removeWidget(d->currentProcessToolBox);
    d->currentProcessToolBox = d->presenter->buildToolBoxWidget();
    d->workspaceToolBox->addWidget(d->currentProcessToolBox);
    d->workspaceToolBox->setTitle(d->process->caption());

    if (this->tabbedViewContainers()->currentWidget() == 0)
        this->tabbedViewContainers()->setSplitter(this->tabbedViewContainers()->currentIndex(), d->presenter->buildViewContainerSplitter());
    else
        this->tabbedViewContainers()->setSplitter(this->tabbedViewContainers()->count(), d->presenter->buildViewContainerSplitter());
}

void medGenericWorkspace::setupTabbedViewContainer()
{
}

bool medGenericWorkspace::isUsable()
{
    return true;
}
