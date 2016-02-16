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

#include <medJobManager.h>

#include <medToolBox.h>
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>

#include <QDebug>

// Yikes, this is ugly but necessary
struct SingleFiltersGathering
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

    SingleFiltersGathering()
    {
        addFactory = 0;
        divideFactory = 0;
        gaussianFactory = 0;
        invertFactory = 0;
        medianFactory = 0;
        multiplyFactory = 0;
        normalizeFactory = 0;
        shrinkFactory = 0;
        subtractFactory = 0;
        windowingFactory = 0;

        addFactoryPresenter = 0;
        divideFactoryPresenter = 0;
        gaussianFactoryPresenter = 0;
        invertFactoryPresenter = 0;
        medianFactoryPresenter = 0;
        multiplyFactoryPresenter = 0;
        normalizeFactoryPresenter = 0;
        shrinkFactoryPresenter = 0;
        subtractFactoryPresenter = 0;
        windowingFactoryPresenter = 0;

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

        return 0;
    }
};

struct MorphomathGathering
{
    // fun times
    medAbstractErodeImageProcessPluginFactory* erodeFactory;
    medAbstractDilateImageProcessPluginFactory* dilateFactory;
    medAbstractOpeningImageProcessPluginFactory* openFactory;
    medAbstractClosingImageProcessPluginFactory* closeFactory;

    medAbstractErodeImageProcessPresenterFactory* erodeFactoryPresenter;
    medAbstractDilateImageProcessPresenterFactory* dilateFactoryPresenter;
    medAbstractOpeningImageProcessPresenterFactory* openFactoryPresenter;
    medAbstractClosingImageProcessPresenterFactory* closeFactoryPresenter;

    QString pluginKey;
    medAbstractProcess* myProcess;

    MorphomathGathering()
    {
        erodeFactory = 0;
        dilateFactory = 0;
        openFactory = 0;
        closeFactory = 0;

        erodeFactoryPresenter = 0;
        dilateFactoryPresenter = 0;
        openFactoryPresenter = 0;
        closeFactoryPresenter = 0;

        myProcess = 0;
    }

    medAbstractProcess* getProcess()
    {
        if (erodeFactory)
            myProcess = erodeFactory->create(pluginKey);

        else if(dilateFactory)
            myProcess = dilateFactory->create(pluginKey);

        else if(openFactory)
            myProcess = openFactory->create(pluginKey);

        else if(closeFactory)
            myProcess = closeFactory->create(pluginKey);

        return myProcess;
    }

    medAbstractProcessPresenter* getPresenter()
    {
        if (erodeFactoryPresenter)
            return erodeFactoryPresenter->create(myProcess);

        else if(dilateFactoryPresenter)
            return dilateFactoryPresenter->create(myProcess);

        else if(openFactoryPresenter)
            return openFactoryPresenter->create(myProcess);

        else if(closeFactoryPresenter)
            return closeFactoryPresenter->create(myProcess);

        return 0;
    }
};

struct ArithmeticGathering
{
    // fun times
    medAbstractAddImageProcessPluginFactory* addFactory;
    medAbstractSubtractImageProcessPluginFactory* subtractFactory;
    medAbstractMultiplyImageProcessPluginFactory* multiplyFactory;
    medAbstractDivideImageProcessPluginFactory* divideFactory;

    medAbstractAddImageProcessPresenterFactory* addFactoryPresenter;
    medAbstractSubtractImageProcessPresenterFactory* subtractFactoryPresenter;
    medAbstractMultiplyImageProcessPresenterFactory* multiplyFactoryPresenter;
    medAbstractDivideImageProcessPresenterFactory* divideFactoryPresenter;

    QString pluginKey;
    medAbstractProcess* myProcess;

    ArithmeticGathering()
    {
        addFactory = 0;
        subtractFactory = 0;
        multiplyFactory = 0;
        divideFactory = 0;

        addFactoryPresenter = 0;
        subtractFactoryPresenter = 0;
        multiplyFactoryPresenter = 0;
        divideFactoryPresenter = 0;

        myProcess = 0;
    }

    medAbstractProcess* getProcess()
    {
        if (addFactory)
            myProcess = addFactory->create(pluginKey);

        else if(subtractFactory)
            myProcess = subtractFactory->create(pluginKey);

        else if(multiplyFactory)
            myProcess = multiplyFactory->create(pluginKey);

        else if(divideFactory)
            myProcess = divideFactory->create(pluginKey);

        return myProcess;
    }

    medAbstractProcessPresenter* getPresenter()
    {
        if (addFactoryPresenter)
            return addFactoryPresenter->create(myProcess);

        else if(subtractFactoryPresenter)
            return subtractFactoryPresenter->create(myProcess);

        else if(multiplyFactoryPresenter)
            return multiplyFactoryPresenter->create(myProcess);

        else if(divideFactoryPresenter)
            return divideFactoryPresenter->create(myProcess);

        return 0;
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

    std::vector <SingleFiltersGathering> singleFiltersVector;
    std::vector <MorphomathGathering> morphoMathsVector;
    std::vector <ArithmeticGathering> arithmeticsVector;
};

medFilteringWorkspace::medFilteringWorkspace(QWidget *parent): medAbstractWorkspaceLegacy (parent), d(new medFilteringWorkspacePrivate)
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

    d->singleFiltersVector.clear();
    d->morphoMathsVector.clear();
    d->arithmeticsVector.clear();

    switch (tProc)
    {
        case MorphoMath:
        {
            QStringList plugins = medCore::morphomathOperation::erodeImage::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::morphomathOperation::erodeImage::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    MorphomathGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.erodeFactory = &medCore::morphomathOperation::erodeImage::pluginFactory();
                    aSolution.erodeFactoryPresenter = &medWidgets::morphomathOperation::erodeImage::presenterFactory();

                    d->morphoMathsVector.push_back(aSolution);
                }
            }

            plugins = medCore::morphomathOperation::dilateImage::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::morphomathOperation::dilateImage::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    MorphomathGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.dilateFactory = &medCore::morphomathOperation::dilateImage::pluginFactory();
                    aSolution.dilateFactoryPresenter = &medWidgets::morphomathOperation::dilateImage::presenterFactory();

                    d->morphoMathsVector.push_back(aSolution);
                }
            }

            plugins = medCore::morphomathOperation::openingImage::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::morphomathOperation::openingImage::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    MorphomathGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.openFactory = &medCore::morphomathOperation::openingImage::pluginFactory();
                    aSolution.openFactoryPresenter = &medWidgets::morphomathOperation::openingImage::presenterFactory();

                    d->morphoMathsVector.push_back(aSolution);
                }
            }

            plugins = medCore::morphomathOperation::closingImage::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::morphomathOperation::closingImage::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    MorphomathGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.closeFactory = &medCore::morphomathOperation::closingImage::pluginFactory();
                    aSolution.closeFactoryPresenter = &medWidgets::morphomathOperation::closingImage::presenterFactory();

                    d->morphoMathsVector.push_back(aSolution);
                }
            }
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
                    SingleFiltersGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.addFactory = &medCore::singleFilterOperation::addFilter::pluginFactory();
                    aSolution.addFactoryPresenter = &medWidgets::singleFilterOperation::addFilter::presenterFactory();

                    d->singleFiltersVector.push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::divideFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::divideFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    SingleFiltersGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.divideFactory = &medCore::singleFilterOperation::divideFilter::pluginFactory();
                    aSolution.divideFactoryPresenter = &medWidgets::singleFilterOperation::divideFilter::presenterFactory();

                    d->singleFiltersVector.push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::gaussianFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::gaussianFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    SingleFiltersGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.gaussianFactory = &medCore::singleFilterOperation::gaussianFilter::pluginFactory();
                    aSolution.gaussianFactoryPresenter = &medWidgets::singleFilterOperation::gaussianFilter::presenterFactory();

                    d->singleFiltersVector.push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::invertFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::invertFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    SingleFiltersGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.invertFactory = &medCore::singleFilterOperation::invertFilter::pluginFactory();
                    aSolution.invertFactoryPresenter = &medWidgets::singleFilterOperation::invertFilter::presenterFactory();

                    d->singleFiltersVector.push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::medianFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::medianFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    SingleFiltersGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.medianFactory = &medCore::singleFilterOperation::medianFilter::pluginFactory();
                    aSolution.medianFactoryPresenter = &medWidgets::singleFilterOperation::medianFilter::presenterFactory();

                    d->singleFiltersVector.push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::multiplyFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::multiplyFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    SingleFiltersGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.multiplyFactory = &medCore::singleFilterOperation::multiplyFilter::pluginFactory();
                    aSolution.multiplyFactoryPresenter = &medWidgets::singleFilterOperation::multiplyFilter::presenterFactory();

                    d->singleFiltersVector.push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::normalizeFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::normalizeFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    SingleFiltersGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.normalizeFactory = &medCore::singleFilterOperation::normalizeFilter::pluginFactory();
                    aSolution.normalizeFactoryPresenter = &medWidgets::singleFilterOperation::normalizeFilter::presenterFactory();

                    d->singleFiltersVector.push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::shrinkFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::shrinkFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    SingleFiltersGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.shrinkFactory = &medCore::singleFilterOperation::shrinkFilter::pluginFactory();
                    aSolution.shrinkFactoryPresenter = &medWidgets::singleFilterOperation::shrinkFilter::presenterFactory();

                    d->singleFiltersVector.push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::subtractFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::subtractFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    SingleFiltersGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.subtractFactory = &medCore::singleFilterOperation::subtractFilter::pluginFactory();
                    aSolution.subtractFactoryPresenter = &medWidgets::singleFilterOperation::subtractFilter::presenterFactory();

                    d->singleFiltersVector.push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::windowingFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::windowingFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    SingleFiltersGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.windowingFactory = &medCore::singleFilterOperation::windowingFilter::pluginFactory();
                    aSolution.windowingFactoryPresenter = &medWidgets::singleFilterOperation::windowingFilter::presenterFactory();

                    d->singleFiltersVector.push_back(aSolution);
                }
            }

            break;
        }

        case Arithmetic:
        default:
        {
            QStringList plugins = medCore::arithmeticOperation::addImage::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::arithmeticOperation::addImage::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    ArithmeticGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.addFactory = &medCore::arithmeticOperation::addImage::pluginFactory();
                    aSolution.addFactoryPresenter = &medWidgets::arithmeticOperation::addImage::presenterFactory();

                    d->arithmeticsVector.push_back(aSolution);
                }
            }

            plugins = medCore::arithmeticOperation::subtractImage::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::arithmeticOperation::subtractImage::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    ArithmeticGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.subtractFactory = &medCore::arithmeticOperation::subtractImage::pluginFactory();
                    aSolution.subtractFactoryPresenter = &medWidgets::arithmeticOperation::subtractImage::presenterFactory();

                    d->arithmeticsVector.push_back(aSolution);
                }
            }

            plugins = medCore::arithmeticOperation::multiplyImage::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::arithmeticOperation::multiplyImage::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    ArithmeticGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.multiplyFactory = &medCore::arithmeticOperation::multiplyImage::pluginFactory();
                    aSolution.multiplyFactoryPresenter = &medWidgets::arithmeticOperation::multiplyImage::presenterFactory();

                    d->arithmeticsVector.push_back(aSolution);
                }
            }

            plugins = medCore::arithmeticOperation::divideImage::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::arithmeticOperation::divideImage::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    ArithmeticGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.divideFactory = &medCore::arithmeticOperation::divideImage::pluginFactory();
                    aSolution.divideFactoryPresenter = &medWidgets::arithmeticOperation::divideImage::presenterFactory();

                    d->arithmeticsVector.push_back(aSolution);
                }
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
            for (unsigned int i = 0; i < d->morphoMathsVector.size(); i++)
            {
                if (d->morphoMathsVector[i].pluginKey == pluginKey)
                {
                    d->process = d->morphoMathsVector[i].getProcess();
                    d->presenter = d->morphoMathsVector[i].getPresenter();
                    break;
                }
            }

            break;
        }

        case SingleFilter:
        {
            for (unsigned int i = 0; i < d->singleFiltersVector.size(); i++)
            {
                if (d->singleFiltersVector[i].pluginKey == pluginKey)
                {
                    d->process = d->singleFiltersVector[i].getProcess();
                    d->presenter = d->singleFiltersVector[i].getPresenter();
                    break;
                }
            }

            break;
        }

        case Arithmetic:
        default:
        {
            for (unsigned int i = 0; i < d->arithmeticsVector.size(); i++)
            {
                if (d->arithmeticsVector[i].pluginKey == pluginKey)
                {
                    d->process = d->arithmeticsVector[i].getProcess();
                    d->presenter = d->arithmeticsVector[i].getPresenter();
                    break;
                }
            }

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
