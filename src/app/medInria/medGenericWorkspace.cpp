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

    std::vector<HackySolution> *vectorOfHacks;
};




medGenericWorkspace::medGenericWorkspace(QWidget *parent): medAbstractWorkspaceLegacy (parent), d(new medGenericWorkspacePrivate)
{
    d->presenter = NULL;
    d->process = NULL;
    d->vectorOfHacks = new std::vector<HackySolution>();

    QWidget *processTypeWidget = new QWidget;
    QLabel *processTypeLabel = new QLabel("Process Type", processTypeWidget);
    QHBoxLayout *processTypeLayout = new QHBoxLayout;
    processTypeLayout->addWidget(processTypeLabel);
    d->processTypeComboBox = new QComboBox;
    processTypeLayout->addWidget(d->processTypeComboBox);
    d->processTypeComboBox->addItem("None selected");
    d->processTypeComboBox->addItem("Mathematical morphology");
    d->processTypeComboBox->addItem("Image masking");
    d->processTypeComboBox->addItem("DWI mask computation");
    d->processTypeComboBox->addItem("Diffusion model estimation");
    d->processTypeComboBox->addItem("Diffusion scalar maps");
    d->processTypeComboBox->addItem("Tractography");
    d->processTypeComboBox->addItem("Single Filter");
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
        case MorphoMath:
        {
            QStringList plugins = medCore::morphomathOperation::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::morphomathOperation::pluginFactory().create(pluginKey);
                if (process)
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
            }
;
            break;
        }

        case MaskImage:
        {
            QStringList plugins = medCore::maskImage::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::maskImage::pluginFactory().create(pluginKey);
                if (process)
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
            }

            break;
        }

        case DWIMasking:
        {
            QStringList plugins = medCore::dwiMasking::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::dwiMasking::pluginFactory().create(pluginKey);
                if (process)
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
            }

            break;
        }

        case DiffusionModelEstimation:
        {
            QStringList plugins = medCore::diffusionModelEstimation::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::diffusionModelEstimation::pluginFactory().create(pluginKey);
                if (process)
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
            }

            break;
        }

        case DiffusionScalarMaps:
        {
            QStringList plugins = medCore::diffusionScalarMaps::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::diffusionScalarMaps::pluginFactory().create(pluginKey);
                if (process)
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
            }

            break;
        }

        case Tractography:
        {
            QStringList plugins = medCore::tractography::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::tractography::pluginFactory().create(pluginKey);
                if (process)
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
            }

            break;
        }

        case SingleFilter:
        default:
        {
            QStringList plugins = medCore::singleFilterOperation::gaussianFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::gaussianFilter::pluginFactory().create(pluginKey);
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
        case MorphoMath:
        {

            medAbstractOpeningImageProcess *process = medCore::morphomathOperation::openingImage::pluginFactory().create(pluginKey);
            d->process = process;
            d->presenter = medWidgets::morphomathOperation::openingImage::presenterFactory().create(process);

            break;
        }

        case MaskImage:
        {
            medAbstractMaskImageProcess *process = medCore::maskImage::pluginFactory().create(pluginKey);
            d->process = process;
            d->presenter = medWidgets::maskImage::presenterFactory().create(process);

            break;
        }

        case DWIMasking:
        {
            medAbstractDWIMaskingProcess *process = medCore::dwiMasking::pluginFactory().create(pluginKey);
            d->process = process;
            d->presenter = medWidgets::dwiMasking::presenterFactory().create(process);

            break;
        }

        case DiffusionModelEstimation:
        {
            medAbstractDiffusionModelEstimationProcess *process = medCore::diffusionModelEstimation::pluginFactory().create(pluginKey);
            d->process = process;
            d->presenter = medWidgets::diffusionModelEstimation::presenterFactory().create(process);

            break;
        }

        case DiffusionScalarMaps:
        {
            medAbstractDiffusionScalarMapsProcess *process = medCore::diffusionScalarMaps::pluginFactory().create(pluginKey);
            d->process = process;
            d->presenter = medWidgets::diffusionScalarMaps::presenterFactory().create(process);

            break;
        }

        case Tractography:
        {
            medAbstractTractographyProcess *process = medCore::tractography::pluginFactory().create(pluginKey);
            d->process = process;
            d->presenter = medWidgets::tractography::presenterFactory().create(process);

            break;
        }

        case SingleFilter:
        default:
        {
            medAbstractGaussianFilterProcess *process = medCore::singleFilterOperation::gaussianFilter::pluginFactory().create(pluginKey);
            d->process = process;
            d->presenter = medWidgets::singleFilterOperation::gaussianFilter::presenterFactory().create(process);

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
