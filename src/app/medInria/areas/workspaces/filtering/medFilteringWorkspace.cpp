/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
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
class SingleFiltersGathering
{
public:
    // fun times
    medAbstractGaussianFilterProcessPluginFactory* gaussianFactory;
    medAbstractMedianFilterProcessPluginFactory* medianFactory;
    medAbstractNormalizeFilterProcessPluginFactory* normalizeFactory;
    medAbstractShrinkFilterProcessPluginFactory* shrinkFactory;
    medAbstractWindowingFilterProcessPluginFactory* windowingFactory;
    medAbstractImageDenoisingProcessPluginFactory* denoisingFactory;
    medAbstractRelaxometryEstimationProcessPluginFactory* relaxometryFactory;
    medAbstractSymmetryPlaneAlignmentProcessPluginFactory* symmetryFactory;
    medAbstractBiasCorrectionProcessPluginFactory* biasCorrectionFactory;

    medAbstractGaussianFilterProcessPresenterFactory* gaussianFactoryPresenter;
    medAbstractMedianFilterProcessPresenterFactory* medianFactoryPresenter;
    medAbstractNormalizeFilterProcessPresenterFactory* normalizeFactoryPresenter;
    medAbstractShrinkFilterProcessPresenterFactory* shrinkFactoryPresenter;
    medAbstractWindowingFilterProcessPresenterFactory* windowingFactoryPresenter;
    medAbstractImageDenoisingProcessPresenterFactory* denoisingFactoryPresenter;
    medAbstractRelaxometryEstimationProcessPresenterFactory* relaxometryFactoryPresenter;
    medAbstractSymmetryPlaneAlignmentProcessPresenterFactory* symmetryFactoryPresenter;
    medAbstractBiasCorrectionProcessPresenterFactory* biasCorrectionFactoryPresenter;

    QString pluginKey;
    medAbstractProcess* myProcess;

    SingleFiltersGathering()
    {
        gaussianFactory = 0;
        medianFactory = 0;
        normalizeFactory = 0;
        shrinkFactory = 0;
        windowingFactory = 0;
        denoisingFactory = 0;
        relaxometryFactory = 0;
        symmetryFactory = 0;
        biasCorrectionFactory = 0;

        gaussianFactoryPresenter = 0;
        medianFactoryPresenter = 0;
        normalizeFactoryPresenter = 0;
        shrinkFactoryPresenter = 0;
        windowingFactoryPresenter = 0;
        denoisingFactoryPresenter = 0;
        relaxometryFactoryPresenter = 0;
        symmetryFactoryPresenter = 0;
        biasCorrectionFactoryPresenter = 0;

        myProcess = 0;
    }

    medAbstractProcess* getProcess()
    {
        if(gaussianFactory)
            myProcess = gaussianFactory->create(pluginKey);

        else if(medianFactory)
            myProcess = medianFactory->create(pluginKey);

        else if(normalizeFactory)
            myProcess = normalizeFactory->create(pluginKey);

        else if(shrinkFactory)
            myProcess = shrinkFactory->create(pluginKey);

        else if(windowingFactory)
            myProcess = windowingFactory->create(pluginKey);

        else if(denoisingFactory)
            myProcess = denoisingFactory->create(pluginKey);

        else if(relaxometryFactory)
            myProcess = relaxometryFactory->create(pluginKey);

        else if(symmetryFactory)
            myProcess = symmetryFactory->create(pluginKey);

        else if (biasCorrectionFactory)
           myProcess = biasCorrectionFactory->create(pluginKey);

        return myProcess;
    }

    medAbstractProcessPresenter* getPresenter()
    {
        if(gaussianFactoryPresenter)
            return gaussianFactoryPresenter->create(myProcess);

        else if(medianFactoryPresenter)
            return medianFactoryPresenter->create(myProcess);

        else if(normalizeFactoryPresenter)
            return normalizeFactoryPresenter->create(myProcess);

        else if(shrinkFactoryPresenter)
            return shrinkFactoryPresenter->create(myProcess);

        else if(windowingFactoryPresenter)
            return windowingFactoryPresenter->create(myProcess);

        else if(denoisingFactoryPresenter)
            return denoisingFactoryPresenter->create(myProcess);

        else if(relaxometryFactoryPresenter)
            return relaxometryFactoryPresenter->create(myProcess);

        else if (symmetryFactoryPresenter)
           return symmetryFactoryPresenter->create(myProcess);

        else if (biasCorrectionFactoryPresenter)
           return biasCorrectionFactoryPresenter->create(myProcess);

        return 0;
    }
};

class MorphomathGathering
{
public:
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

class ArithmeticGathering
{
public:
    // Image arithmetic stuff
    medAbstractAddFilterProcessPluginFactory* addFactory;
    medAbstractSubtractFilterProcessPluginFactory* subtractFactory;
    medAbstractMultiplyFilterProcessPluginFactory* multiplyFactory;
    medAbstractDivideFilterProcessPluginFactory* divideFactory;
    medAbstractAddImageProcessPluginFactory* addImageFactory;
    medAbstractSubtractImageProcessPluginFactory* subtractImageFactory;
    medAbstractMultiplyImageProcessPluginFactory* multiplyImageFactory;
    medAbstractDivideImageProcessPluginFactory* divideImageFactory;
    medAbstractInvertFilterProcessPluginFactory* invertFactory;
    medAbstractMaskImageProcessPluginFactory* maskFactory;

    medAbstractAddFilterProcessPresenterFactory* addFactoryPresenter;
    medAbstractSubtractFilterProcessPresenterFactory* subtractFactoryPresenter;
    medAbstractMultiplyFilterProcessPresenterFactory* multiplyFactoryPresenter;
    medAbstractDivideFilterProcessPresenterFactory* divideFactoryPresenter;
    medAbstractAddImageProcessPresenterFactory* addImageFactoryPresenter;
    medAbstractSubtractImageProcessPresenterFactory* subtractImageFactoryPresenter;
    medAbstractMultiplyImageProcessPresenterFactory* multiplyImageFactoryPresenter;
    medAbstractDivideImageProcessPresenterFactory* divideImageFactoryPresenter;
    medAbstractInvertFilterProcessPresenterFactory* invertFactoryPresenter;
    medAbstractMaskImageProcessPresenterFactory* maskFactoryPresenter;

    QString pluginKey;
    medAbstractProcess* myProcess;

    ArithmeticGathering()
    {
        addFactory = 0;
        subtractFactory = 0;
        multiplyFactory = 0;
        divideFactory = 0;
        addImageFactory = 0;
        subtractImageFactory = 0;
        multiplyImageFactory = 0;
        divideImageFactory = 0;
        invertFactory = 0;
        maskFactory = 0;

        addFactoryPresenter = 0;
        subtractFactoryPresenter = 0;
        multiplyFactoryPresenter = 0;
        divideFactoryPresenter = 0;
        addImageFactoryPresenter = 0;
        subtractImageFactoryPresenter = 0;
        multiplyImageFactoryPresenter = 0;
        divideImageFactoryPresenter = 0;
        invertFactoryPresenter = 0;
        maskFactoryPresenter = 0;

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

        if (addImageFactory)
            myProcess = addImageFactory->create(pluginKey);

        else if(subtractImageFactory)
            myProcess = subtractImageFactory->create(pluginKey);

        else if(multiplyImageFactory)
            myProcess = multiplyImageFactory->create(pluginKey);

        else if(divideImageFactory)
            myProcess = divideImageFactory->create(pluginKey);

        else if(invertFactory)
            myProcess = invertFactory->create(pluginKey);

        else if(maskFactory)
            myProcess = maskFactory->create(pluginKey);

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

        if (addImageFactoryPresenter)
            return addImageFactoryPresenter->create(myProcess);

        else if(subtractImageFactoryPresenter)
            return subtractImageFactoryPresenter->create(myProcess);

        else if(multiplyImageFactoryPresenter)
            return multiplyImageFactoryPresenter->create(myProcess);

        else if(divideImageFactoryPresenter)
            return divideImageFactoryPresenter->create(myProcess);

        else if(invertFactoryPresenter)
            return invertFactoryPresenter->create(myProcess);

        else if(maskFactoryPresenter)
            return maskFactoryPresenter->create(myProcess);

        return 0;
    }
};

struct  stProcessAndToolBox
{
    medAbstractProcess *process;
    medAbstractProcessPresenter *presenter;
    QWidget *toolBox;
};

class medFilteringWorkspacePrivate
{
public:
    int iProcessSelection;

    QComboBox   *processTypeComboBox;
    QComboBox   *processSelectorComboBox;
    QPushButton *poCreateFilterButton;

    medToolBox *workspaceToolBox;
    medToolBox *FiltersParamToolBox;
    int iCurrentTab;
    std::vector<stProcessAndToolBox> oProcessTab;

    std::vector <SingleFiltersGathering> singleFiltersVector;
    std::vector <MorphomathGathering> morphoMathsVector;
    std::vector <ArithmeticGathering> arithmeticsVector;
};

medFilteringWorkspace::medFilteringWorkspace(QWidget *parent): medAbstractWorkspaceLegacy (parent), d(new medFilteringWorkspacePrivate)
{
    d->iProcessSelection = 0;
    d->processTypeComboBox = nullptr;
    d->processSelectorComboBox = nullptr;
    d->workspaceToolBox = nullptr;
    d->iCurrentTab = -1;


    QWidget *processTypeWidget = new QWidget;
    QLabel *processTypeLabel = new QLabel("Process Type", processTypeWidget);
    QHBoxLayout *processTypeLayout = new QHBoxLayout;
    processTypeLayout->addWidget(processTypeLabel);
    d->processTypeComboBox = new QComboBox;
    processTypeLayout->addWidget(d->processTypeComboBox);
    d->processTypeComboBox->addItem("None selected");
    d->processTypeComboBox->addItem("Mathematical morphology");
    d->processTypeComboBox->addItem("Image filters");
    d->processTypeComboBox->addItem("Image arithmetic");
    processTypeWidget->setLayout(processTypeLayout);

    connect(d->processTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setProcessType(int)));

    QWidget *processWidget = new QWidget;
    QLabel *processLabel = new QLabel("Process", processWidget);
    QHBoxLayout *processLayout = new QHBoxLayout;
    processLayout->addWidget(processLabel);
    d->processSelectorComboBox = new QComboBox;
    processLayout->addWidget(d->processSelectorComboBox);
    processWidget->setLayout(processLayout);

    connect(d->processSelectorComboBox, SIGNAL(currentIndexChanged(int)),this,SLOT(setProcessSelection(int)));

    QWidget *poDummyWidget4MarginOfCreateButton = new QWidget();
    QGridLayout *poDummyCreatButtonLayout = new QGridLayout();
    d->poCreateFilterButton = new QPushButton("Create filter");
    d->poCreateFilterButton->setEnabled(false);
    poDummyWidget4MarginOfCreateButton->setLayout(poDummyCreatButtonLayout);
    poDummyCreatButtonLayout->addWidget(d->poCreateFilterButton);
    connect(d->poCreateFilterButton, SIGNAL(clicked()), this, SLOT(createFilterEnvironment()));

    d->workspaceToolBox = new medToolBox;
    d->workspaceToolBox->setTitle("Process controller");
    d->workspaceToolBox->addWidget(processTypeWidget);
    d->workspaceToolBox->addWidget(processWidget);
    d->workspaceToolBox->addWidget(poDummyWidget4MarginOfCreateButton);
    this->addToolBox(d->workspaceToolBox);

    d->FiltersParamToolBox = new medToolBox;
    d->FiltersParamToolBox->setTitle("Parameters of filter");
    this->addToolBox(d->FiltersParamToolBox);

    connect(tabbedViewContainers(), SIGNAL(currentChanged(int)), this, SLOT(setCurrentTab(int)));
    connect(tabbedViewContainers(), SIGNAL(tabCloseRequested(int)), this, SLOT(closingTab(int)));
    
}

medFilteringWorkspace::~medFilteringWorkspace()
{
    delete d;
    d = nullptr;
}

void medFilteringWorkspace::setupTabbedViewContainer()
{
    this->tabbedViewContainers()->cornerWidget()->hide();
}

void medFilteringWorkspace::setProcessType(int index)
{
    setProcessSelection(0);

    ProcessTypes tProc = (ProcessTypes)index;

    d->processSelectorComboBox->blockSignals(true);
    d->processSelectorComboBox->clear();
    d->processSelectorComboBox->addItem("None selected");

    d->singleFiltersVector.clear();
    d->morphoMathsVector.clear();
    d->arithmeticsVector.clear();

    if (index == 0)
    {
        return;
    }

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
            QStringList plugins = medCore::singleFilterOperation::gaussianFilter::pluginFactory().keys();
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

            plugins = medCore::singleFilterOperation::imageDenoising::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::imageDenoising::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    SingleFiltersGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.denoisingFactory = &medCore::singleFilterOperation::imageDenoising::pluginFactory();
                    aSolution.denoisingFactoryPresenter = &medWidgets::singleFilterOperation::imageDenoising::presenterFactory();

                    d->singleFiltersVector.push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::relaxometryEstimation::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::relaxometryEstimation::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    SingleFiltersGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.relaxometryFactory = &medCore::singleFilterOperation::relaxometryEstimation::pluginFactory();
                    aSolution.relaxometryFactoryPresenter = &medWidgets::singleFilterOperation::relaxometryEstimation::presenterFactory();

                    d->singleFiltersVector.push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::symmetryAlignment::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::symmetryAlignment::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    SingleFiltersGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.symmetryFactory = &medCore::singleFilterOperation::symmetryAlignment::pluginFactory();
                    aSolution.symmetryFactoryPresenter = &medWidgets::singleFilterOperation::symmetryAlignment::presenterFactory();

                    d->singleFiltersVector.push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::biasCorrection::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
               medAbstractProcess *process = medCore::singleFilterOperation::biasCorrection::pluginFactory().create(pluginKey);
               if (process)
               {
                  d->processSelectorComboBox->addItem(process->caption(), pluginKey);
                  SingleFiltersGathering aSolution;
                  aSolution.pluginKey = pluginKey;
                  aSolution.biasCorrectionFactory = &medCore::singleFilterOperation::biasCorrection::pluginFactory();
                  aSolution.biasCorrectionFactoryPresenter = &medWidgets::singleFilterOperation::biasCorrection::presenterFactory();

                  d->singleFiltersVector.push_back(aSolution);
               }
            }

            break;
        }

        case Arithmetic:
        default:
        {
            QStringList plugins = medCore::singleFilterOperation::addFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::addFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    ArithmeticGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.addFactory = &medCore::singleFilterOperation::addFilter::pluginFactory();
                    aSolution.addFactoryPresenter = &medWidgets::singleFilterOperation::addFilter::presenterFactory();

                    d->arithmeticsVector.push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::subtractFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::subtractFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    ArithmeticGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.subtractFactory = &medCore::singleFilterOperation::subtractFilter::pluginFactory();
                    aSolution.subtractFactoryPresenter = &medWidgets::singleFilterOperation::subtractFilter::presenterFactory();

                    d->arithmeticsVector.push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::multiplyFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::multiplyFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    ArithmeticGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.multiplyFactory = &medCore::singleFilterOperation::multiplyFilter::pluginFactory();
                    aSolution.multiplyFactoryPresenter = &medWidgets::singleFilterOperation::multiplyFilter::presenterFactory();

                    d->arithmeticsVector.push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::divideFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::divideFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    ArithmeticGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.divideFactory = &medCore::singleFilterOperation::divideFilter::pluginFactory();
                    aSolution.divideFactoryPresenter = &medWidgets::singleFilterOperation::divideFilter::presenterFactory();

                    d->arithmeticsVector.push_back(aSolution);
                }
            }

            plugins = medCore::arithmeticOperation::addImage::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::arithmeticOperation::addImage::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    ArithmeticGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.addImageFactory = &medCore::arithmeticOperation::addImage::pluginFactory();
                    aSolution.addImageFactoryPresenter = &medWidgets::arithmeticOperation::addImage::presenterFactory();

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
                    aSolution.subtractImageFactory = &medCore::arithmeticOperation::subtractImage::pluginFactory();
                    aSolution.subtractImageFactoryPresenter = &medWidgets::arithmeticOperation::subtractImage::presenterFactory();

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
                    aSolution.multiplyImageFactory = &medCore::arithmeticOperation::multiplyImage::pluginFactory();
                    aSolution.multiplyImageFactoryPresenter = &medWidgets::arithmeticOperation::multiplyImage::presenterFactory();

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
                    aSolution.divideImageFactory = &medCore::arithmeticOperation::divideImage::pluginFactory();
                    aSolution.divideImageFactoryPresenter = &medWidgets::arithmeticOperation::divideImage::presenterFactory();

                    d->arithmeticsVector.push_back(aSolution);
                }
            }

            plugins = medCore::singleFilterOperation::invertFilter::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::singleFilterOperation::invertFilter::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    ArithmeticGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.invertFactory = &medCore::singleFilterOperation::invertFilter::pluginFactory();
                    aSolution.invertFactoryPresenter = &medWidgets::singleFilterOperation::invertFilter::presenterFactory();

                    d->arithmeticsVector.push_back(aSolution);
                }
            }

            plugins = medCore::maskImage::pluginFactory().keys();
            foreach(QString pluginKey, plugins)
            {
                medAbstractProcess *process = medCore::maskImage::pluginFactory().create(pluginKey);
                if (process)
                {
                    d->processSelectorComboBox->addItem(process->caption(),pluginKey);
                    ArithmeticGathering aSolution;
                    aSolution.pluginKey = pluginKey;
                    aSolution.maskFactory = &medCore::maskImage::pluginFactory();
                    aSolution.maskFactoryPresenter = &medWidgets::maskImage::presenterFactory();

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
    d->iProcessSelection = index;
    d->poCreateFilterButton->setEnabled(d->iProcessSelection != 0);
}

/**
* @brief  This function creates environment for a filter.
* @detail Creates environment for a filter like new tab, ProcessPresenter, toolbox with title and process.
*/
void medFilteringWorkspace::createFilterEnvironment()
{
    medAbstractProcess *poProcess = nullptr;
    medAbstractProcessPresenter *poPresenter = nullptr;
    int index = d->iProcessSelection;

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
                    poProcess = d->morphoMathsVector[i].getProcess();
                    poPresenter = d->morphoMathsVector[i].getPresenter();
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
                    poProcess = d->singleFiltersVector[i].getProcess();
                    poPresenter = d->singleFiltersVector[i].getPresenter();
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
                    poProcess = d->arithmeticsVector[i].getProcess();
                    poPresenter = d->arithmeticsVector[i].getPresenter();
                    break;
                }
            }

            break;
        }
    }

    QWidget *poToolBoxWidget = poPresenter->buildToolBoxWidget();

    d->oProcessTab.push_back({ poProcess, poPresenter, poToolBoxWidget });
    if (d->iCurrentTab >= 0 && d->iCurrentTab < static_cast<int>(d->oProcessTab.size()))
    {
        d->oProcessTab[d->iCurrentTab].toolBox->hide();
    }
    d->FiltersParamToolBox->setTitle(QString("Parameters of ") + poProcess->caption());
    d->FiltersParamToolBox->addWidget(poToolBoxWidget);

    d->iCurrentTab = d->oProcessTab.size()-1;

    if (this->tabbedViewContainers()->currentWidget() == 0)
    {
        this->tabbedViewContainers()->setSplitter(this->tabbedViewContainers()->currentIndex(), poPresenter->buildViewContainerSplitter());
    }
    else
    {
        this->tabbedViewContainers()->setSplitter(this->tabbedViewContainers()->count(), poPresenter->buildViewContainerSplitter());
    }
}

void medFilteringWorkspace::setCurrentTab(int index)
{
    if ( (index >= 0) && (d->iCurrentTab >= 0) && d->oProcessTab.size() && (d->iCurrentTab != index))
    {
        stProcessAndToolBox stTmp = d->oProcessTab[index];
        d->FiltersParamToolBox->setTitle(QString("Parameters of ") + stTmp.process->caption());

        if (d->iCurrentTab < static_cast<int>(d->oProcessTab.size()))
        {
            d->oProcessTab[d->iCurrentTab].toolBox->hide();
        }
        stTmp.toolBox->show();
        d->iCurrentTab = index;
    }
}

void medFilteringWorkspace::closingTab(int index)
{
    stProcessAndToolBox &stTmp = d->oProcessTab[index];
    d->FiltersParamToolBox->removeWidget(stTmp.toolBox);
    d->iCurrentTab = d->oProcessTab.size();
    delete stTmp.presenter;
    delete stTmp.process;
    delete stTmp.toolBox;
    stTmp.presenter = nullptr;
    stTmp.process   = nullptr;
    stTmp.toolBox   = nullptr;
    d->oProcessTab.erase((d->oProcessTab.begin()) + index);
}

bool medFilteringWorkspace::isUsable()
{
    return true;
}
