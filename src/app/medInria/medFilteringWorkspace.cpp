/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medFilteringWorkspace.h>


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

struct HackySolution
{

    // fun times
    medAbstractAddFilterProcessPluginFactory* addFactory;
    medAbstractDivideFilterProcessPluginFactory* divideFactory;
    medAbstractGaussianFilterProcessPluginFactory* gaussianFactory;
    medAbstractInvertFilterProcessPluginFactory* invertFactory;
    medAbstractMedianFilterProcessPluginFactory* medianFactory;
    medAbstractMultiplyFilterProcessPluginFactory* multiplyFactory;
    medAbstractNormalizeFilterProcessPluginFactory* normalizeFactory;
    medAbstractShrinkFilterProcessPluginFactory* shrinkFactory;
    medAbstractSubtractFilterProcessPluginFactory* subtractFactory;
    medAbstractWindowingFilterProcessPluginFactory* windowingFactory;


    medAbstractAddFilterProcessPresenterFactory* addFactoryPresenter;
    medAbstractDivideFilterProcessPresenterFactory* divideFactoryPresenter;
    medAbstractGaussianFilterProcessPresenterFactory* gaussianFactoryPresenter;
    medAbstractInvertFilterProcessPresenterFactory* invertFactoryPresenter;
    medAbstractMedianFilterProcessPresenterFactory* medianFactoryPresenter;
    medAbstractMultiplyFilterProcessPresenterFactory* multiplyFactoryPresenter;
    medAbstractNormalizeFilterProcessPresenterFactory* normalizeFactoryPresenter;
    medAbstractShrinkFilterProcessPresenterFactory* shrinkFactoryPresenter;
    medAbstractSubtractFilterProcessPresenterFactory* subtractFactoryPresenter;
    medAbstractWindowingFilterProcessPresenterFactory* windowingFactoryPresenter;

    QString pluginKey;
    medAbstractProcess* myProcess;

    HackySolution()
    {
        // yeah yeah I know
        memset(this, 0, 20*sizeof(void*));
        myProcess = 0;
    }

    medAbstractProcess* getProcess()
    {
        if (addFactory)
            myProcess = addFactory->create(pluginKey);

        else if(divideFactory)
            myProcess = divideFactory->create(pluginKey);

        else if(gaussianFactory)
            myProcess = gaussianFactory->create(pluginKey);

        else if(invertFactory)
            myProcess = invertFactory->create(pluginKey);

        else if(medianFactory)
            myProcess = medianFactory->create(pluginKey);

        else if(multiplyFactory)
            myProcess = multiplyFactory->create(pluginKey);

        else if(normalizeFactory)
            myProcess = normalizeFactory->create(pluginKey);

        else if(shrinkFactory)
            myProcess = shrinkFactory->create(pluginKey);

        else if(subtractFactory)
            myProcess = subtractFactory->create(pluginKey);

        else if(windowingFactory)
            myProcess = windowingFactory->create(pluginKey);

        return myProcess;
    }

    medAbstractProcessPresenter* getPresenter()
    {
        if (addFactoryPresenter)
            return addFactoryPresenter->create(myProcess);

        else if(divideFactoryPresenter)
            return divideFactoryPresenter->create(myProcess);

        else if(gaussianFactoryPresenter)
            return gaussianFactoryPresenter->create(myProcess);

        else if(invertFactoryPresenter)
            return invertFactoryPresenter->create(myProcess);

        else if(medianFactoryPresenter)
            return medianFactoryPresenter->create(myProcess);

        else if(multiplyFactoryPresenter)
            return multiplyFactoryPresenter->create(myProcess);

        else if(normalizeFactoryPresenter)
            return normalizeFactoryPresenter->create(myProcess);

        else if(shrinkFactoryPresenter)
            return shrinkFactoryPresenter->create(myProcess);

        else if(subtractFactoryPresenter)
            return subtractFactoryPresenter->create(myProcess);

        else if(windowingFactoryPresenter)
            return windowingFactoryPresenter->create(myProcess);
    }
};

class medFilteringWorkspacePrivate
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

medFilteringWorkspace::medFilteringWorkspace(QWidget *parent): medAbstractWorkspaceLegacy (parent), d(new medFilteringWorkspacePrivate)
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
    d->processTypeComboBox->addItem("Single Filter");
    d->processTypeComboBox->addItem("Arithmetic");
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

    d->workspaceToolBox = new medToolBox;
    d->workspaceToolBox->setTitle("Process controller");
    d->workspaceToolBox->addWidget(processTypeWidget);
    d->workspaceToolBox->addWidget(processWidget);
    this->addToolBox(d->workspaceToolBox);
}

medFilteringWorkspace::~medFilteringWorkspace()
{
    delete d;
    d = NULL;
}

void medFilteringWorkspace::setupTabbedViewContainer()
{

}

void medFilteringWorkspace::setProcessType(int index)
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

        case SingleFilter:
        {

            QStringList plugins = medCore::singleFilterOperation::addFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::addFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    HackySolution aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.addFactory = &medCore::singleFilterOperation::addFilter::pluginFactory();
                    aSolution.addFactoryPresenter = &medWidgets::singleFilterOperation::addFilter::presenterFactory();

                    d->vectorOfHacks->push_back(aSolution);
                }
            }


            plugins = medCore::singleFilterOperation::divideFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::divideFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    HackySolution aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.divideFactory = &medCore::singleFilterOperation::divideFilter::pluginFactory();
                    aSolution.divideFactoryPresenter = &medWidgets::singleFilterOperation::divideFilter::presenterFactory();

                    d->vectorOfHacks->push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::gaussianFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::gaussianFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    HackySolution aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.gaussianFactory = &medCore::singleFilterOperation::gaussianFilter::pluginFactory();
                    aSolution.gaussianFactoryPresenter = &medWidgets::singleFilterOperation::gaussianFilter::presenterFactory();

                    d->vectorOfHacks->push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::invertFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::invertFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    HackySolution aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.invertFactory = &medCore::singleFilterOperation::invertFilter::pluginFactory();
                    aSolution.invertFactoryPresenter = &medWidgets::singleFilterOperation::invertFilter::presenterFactory();

                    d->vectorOfHacks->push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::medianFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::medianFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    HackySolution aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.medianFactory = &medCore::singleFilterOperation::medianFilter::pluginFactory();
                    aSolution.medianFactoryPresenter = &medWidgets::singleFilterOperation::medianFilter::presenterFactory();

                    d->vectorOfHacks->push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::multiplyFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::multiplyFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    HackySolution aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.multiplyFactory = &medCore::singleFilterOperation::multiplyFilter::pluginFactory();
                    aSolution.multiplyFactoryPresenter = &medWidgets::singleFilterOperation::multiplyFilter::presenterFactory();

                    d->vectorOfHacks->push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::normalizeFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::normalizeFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    HackySolution aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.normalizeFactory = &medCore::singleFilterOperation::normalizeFilter::pluginFactory();
                    aSolution.normalizeFactoryPresenter = &medWidgets::singleFilterOperation::normalizeFilter::presenterFactory();

                    d->vectorOfHacks->push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::shrinkFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::shrinkFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    HackySolution aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.shrinkFactory = &medCore::singleFilterOperation::shrinkFilter::pluginFactory();
                    aSolution.shrinkFactoryPresenter = &medWidgets::singleFilterOperation::shrinkFilter::presenterFactory();

                    d->vectorOfHacks->push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::subtractFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::subtractFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    HackySolution aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.subtractFactory = &medCore::singleFilterOperation::subtractFilter::pluginFactory();
                    aSolution.subtractFactoryPresenter = &medWidgets::singleFilterOperation::subtractFilter::presenterFactory();

                    d->vectorOfHacks->push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::windowingFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::windowingFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    HackySolution aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.windowingFactory = &medCore::singleFilterOperation::windowingFilter::pluginFactory();
                    aSolution.windowingFactoryPresenter = &medWidgets::singleFilterOperation::windowingFilter::presenterFactory();

                    d->vectorOfHacks->push_back(aSolution);
                }
            }

            break;
        }
        case Arithmetic:
        default:
        {
            QStringList plugins = medCore::arithmeticOperation::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::arithmeticOperation::pluginFactory().create(pluginKey);
                if (process)
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
            }

            break;
        }
    }

    d->processSelectorComboBox->blockSignals(false);
}

void medFilteringWorkspace::setProcessSelection(int index)
{
    if (index == 0)
        return;

    ProcessTypes tProc = (ProcessTypes)d->processTypeComboBox->currentIndex();
    QString pluginKey = d->processSelectorComboBox->itemData(index).toString();

    switch (tProc)
    {
        case MorphoMath:
        {
            medAbstractMorphomathOperationProcess *process = medCore::morphomathOperation::pluginFactory().create(pluginKey);
            d->process = process;
            d->presenter = medWidgets::morphomathOperation::presenterFactory().create(process);

            break;
        }

        case SingleFilter:
        {
            for (size_t i = 0; i < d->vectorOfHacks->size(); i++)
            {
                if (d->vectorOfHacks->at(i).pluginKey == pluginKey)
                {
                    d->process = d->vectorOfHacks->at(i).getProcess();
                    d->presenter = d->vectorOfHacks->at(i).getPresenter();
                    break;
                }
            }

            break;
        }
        case Arithmetic:
        default:
        {
            medAbstractArithmeticOperationProcess *process = medCore::arithmeticOperation::pluginFactory().create(pluginKey);
            d->process = process;
            d->presenter = medWidgets::arithmeticOperation::presenterFactory().create(process);

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

bool medFilteringWorkspace::isUsable()
{
    return true;
}
