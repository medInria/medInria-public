/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDiffusionSelectorToolBox.h>

#include <medAbstractDataFactory.h>
#include <medAbstractImageData.h>
#include <medAbstractFibersData.h>
#include <medAbstractDiffusionModelImageData.h>

#include <medDataManager.h>
#include <medMessageController.h>
#include <medMetaDataKeys.h>

#include <medCore.h>
#include <medWidgets.h>
#include <medDiffusionModelEstimationMetaProcess.h>
#include <medAbstractDiffusionScalarMapsProcess.h>
#include <medAbstractTractographyProcess.h>
#include <medDiffusionModelEstimationMetaProcessPresenter.h>
#include <medAbstractDiffusionScalarMapsProcessPresenter.h>
#include <medAbstractTractographyProcessPresenter.h>

class medDiffusionSelectorToolBoxPrivate
{
public:
    medDiffusionSelectorToolBox::SelectorType selectorType;

    medAbstractProcess *currentProcess;
    medAbstractProcessPresenter *currentProcessPresenter;

    QComboBox *methodCombo;
    QVBoxLayout *mainLayout;

    QComboBox *chooseInput;
    QMap <QString, medAbstractImageData *> inputsMap;

    QWidget *currentToolBox;

    dtkSmartPointer <medAbstractData> processOutput;
};

medDiffusionSelectorToolBox::medDiffusionSelectorToolBox(QWidget *parent, SelectorType type) : medToolBox(parent), d(new medDiffusionSelectorToolBoxPrivate)
{
    d->currentProcess = nullptr;
    d->currentProcessPresenter = nullptr;
    d->currentToolBox = nullptr;
    d->processOutput = nullptr;
    d->methodCombo = nullptr;
    d->selectorType = type;

    // /////////////////////////////////////////////////////////////////
    // main page
    // /////////////////////////////////////////////////////////////////
    QWidget *mainPage = new QWidget(this);
    QString labelTitle;

    if (d->selectorType != Estimation)
    {
        d->methodCombo = new QComboBox(mainPage);
        // --- Setting up custom toolboxes list ---
        d->methodCombo->addItem( tr( "Choose..." ) );
    }

    d->mainLayout = new QVBoxLayout(mainPage);
    QHBoxLayout *inputLayout = new QHBoxLayout;
    QLabel *inputDescriptionLabel = new QLabel(mainPage);
    inputDescriptionLabel->setText(tr("Input image"));
    inputLayout->addWidget(inputDescriptionLabel);

    d->chooseInput = new QComboBox(mainPage);
    d->chooseInput->addItem(tr("Please drop an image"));
    d->chooseInput->setToolTip(tr("Browse available images for processing"));
    d->chooseInput->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    connect(d->chooseInput,SIGNAL(currentIndexChanged(int)),this,SLOT(updateCurrentProcessInput(int)));
    inputLayout->addWidget(d->chooseInput);
    d->mainLayout->addLayout(inputLayout);

    switch(d->selectorType)
    {
        case Estimation:
        {
            medDiffusionModelEstimationMetaProcess *process = new medDiffusionModelEstimationMetaProcess();
            d->currentProcess = process;

            connect(process, &medAbstractJob::finished, this, &medDiffusionSelectorToolBox::jobFinished);
            connect(process,SIGNAL(running(bool)),this,SIGNAL(jobRunning(bool)));

            medDiffusionModelEstimationMetaProcessPresenter *processPresenter = new medDiffusionModelEstimationMetaProcessPresenter(process);
            d->currentProcessPresenter = processPresenter;
            d->currentToolBox = d->currentProcessPresenter->buildToolBoxWidget();
            if (d->currentToolBox)
                d->mainLayout->addWidget(d->currentToolBox);
            break;
        }

        case ScalarMaps:
            labelTitle = tr("Scalar maps");
            for(QString pluginKey : medCore::diffusionScalarMaps::pluginFactory().keys())
            {
                medAbstractProcess *process = medCore::diffusionScalarMaps::pluginFactory().create(pluginKey);
                d->methodCombo->addItem(process->caption(),pluginKey);
            }
            break;

        case Tractography:
        default:
            labelTitle = tr("Tractography");
            for(QString pluginKey : medCore::tractography::pluginFactory().keys())
            {
                medAbstractProcess *process = medCore::tractography::pluginFactory().create(pluginKey);
                d->methodCombo->addItem(process->caption(),pluginKey);
            }
            break;
    }

    QLabel *methodLabel = new QLabel(labelTitle, mainPage);

    if (d->methodCombo)
    {
        QHBoxLayout *methodLayout = new QHBoxLayout;
        methodLayout->addWidget(methodLabel);

        methodLayout->addWidget(d->methodCombo);
        connect(d->methodCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(chooseProcess(int)));

        d->mainLayout->addLayout(methodLayout);
    }

    this->addWidget(mainPage);
    this->setEnabled(false);
}

medDiffusionSelectorToolBox::~medDiffusionSelectorToolBox(void)
{
    delete d;
    d = nullptr;
}

void medDiffusionSelectorToolBox::chooseProcess(int id)
{
    if (id == 0)
        return;

    QString identifier = d->methodCombo->itemData(id).toString();
    QString inputId = d->chooseInput->itemData(d->chooseInput->currentIndex()).toString();

    switch(d->selectorType)
    {
        case Estimation:
        {
            qWarning() << "Should not be here anymore: processes for estimation are handled inside the meta process";
            break;
        }

        case ScalarMaps:
        {
            medAbstractDiffusionScalarMapsProcess *process = medCore::diffusionScalarMaps::pluginFactory().create(identifier);
            d->currentProcess = process;

            if (d->inputsMap[inputId])
            {
                medAbstractDiffusionModelImageData *image = dynamic_cast <medAbstractDiffusionModelImageData *> (d->inputsMap[inputId]);
                process->setInput(image);
            }

            connect(process, &medAbstractJob::finished, this, &medDiffusionSelectorToolBox::jobFinished);
            connect(process,SIGNAL(running(bool)),this,SIGNAL(jobRunning(bool)));

            d->currentProcessPresenter = medWidgets::diffusionScalarMaps::presenterFactory().create(process);
            break;
        }

        case Tractography:
        default:
        {
            medAbstractTractographyProcess *process = medCore::tractography::pluginFactory().create(identifier);
            d->currentProcess = process;

            if (d->inputsMap[inputId])
            {
                medAbstractDiffusionModelImageData *image = dynamic_cast <medAbstractDiffusionModelImageData *> (d->inputsMap[inputId]);
                process->setInput(image);
            }

            connect(process, &medAbstractJob::finished, this, &medDiffusionSelectorToolBox::jobFinished);
            connect(process,SIGNAL(running(bool)),this,SIGNAL(jobRunning(bool)));

            d->currentProcessPresenter = medWidgets::tractography::presenterFactory().create(process);
            break;
        }
    }

    //get rid of old toolBox
    if (d->currentToolBox)
    {
        d->currentToolBox->hide();
        d->mainLayout->removeWidget ( d->currentToolBox );
        d->currentToolBox = nullptr;
    }

    d->currentToolBox = d->currentProcessPresenter->buildToolBoxWidget();
    d->mainLayout->addWidget(d->currentToolBox);
}

medAbstractData *medDiffusionSelectorToolBox::processOutput()
{
    if (!d->currentProcess)
        return nullptr;

    switch(d->selectorType)
    {
        case Estimation:
        {
            medDiffusionModelEstimationMetaProcess *process = qobject_cast <medDiffusionModelEstimationMetaProcess *> (d->currentProcess);
            d->processOutput = process->output();
            break;
        }

        case ScalarMaps:
        {
            medAbstractDiffusionScalarMapsProcess *process = qobject_cast <medAbstractDiffusionScalarMapsProcess *> (d->currentProcess);
            d->processOutput = process->output();
            break;
        }

        case Tractography:
        default:
        {
            medAbstractTractographyProcess *process = qobject_cast <medAbstractTractographyProcess *> (d->currentProcess);
            d->processOutput = process->output();
            break;
        }
    }

    return d->processOutput;
}

void medDiffusionSelectorToolBox::updateCurrentProcessInput(int index)
{
    if (!d->currentProcess)
        return;

    QString inputId = d->chooseInput->itemData(index).toString();

    switch(d->selectorType)
    {
        case Estimation:
        {
            medDiffusionModelEstimationMetaProcess *process = qobject_cast <medDiffusionModelEstimationMetaProcess *> (d->currentProcess);
            medAbstractImageData *image = qobject_cast <medAbstractImageData *> (d->inputsMap[inputId]);
            process->setInput(image);
            break;
        }

        case ScalarMaps:
        {
            medAbstractDiffusionScalarMapsProcess *process = qobject_cast <medAbstractDiffusionScalarMapsProcess *> (d->currentProcess);
            medAbstractDiffusionModelImageData *image = qobject_cast <medAbstractDiffusionModelImageData *> (d->inputsMap[inputId]);
            process->setInput(image);
            break;
        }

        case Tractography:
        default:
        {
            medAbstractTractographyProcess *process = qobject_cast <medAbstractTractographyProcess *> (d->currentProcess);
            medAbstractDiffusionModelImageData *image = qobject_cast <medAbstractDiffusionModelImageData *> (d->inputsMap[inputId]);
            process->setInput(image);
            break;
        }
    }    
}

void medDiffusionSelectorToolBox::addInputImage(medAbstractImageData *data)
{
    if (!data)
        return;
    
    QUuid dataId = QUuid::createUuid();
    if ((d->chooseInput->count() == 1)&&(d->chooseInput->itemData(0) == QVariant()))
        d->chooseInput->removeItem(0);
    
    bool dataUsable = (qobject_cast <medAbstractDiffusionModelImageData *> (data) != 0);
    if (d->selectorType == Estimation)
        dataUsable = !dataUsable;

    if (!dataUsable)
        return;

    this->setEnabled(true);

    d->inputsMap[dataId.toString()] = data;
    d->chooseInput->addItem(data->metadata(medMetaDataKeys::SeriesDescription.key()),dataId.toString());
    d->chooseInput->setToolTip(data->metadata(medMetaDataKeys::SeriesDescription.key()));
    d->chooseInput->setCurrentIndex(d->chooseInput->count() - 1);
}

void medDiffusionSelectorToolBox::clearInputs()
{
    d->chooseInput->clear();
    d->chooseInput->addItem(tr("Please drop an image"));
	d->chooseInput->setToolTip(tr("Browse available images for processing"));
    
    d->inputsMap.clear();
    this->setEnabled(false);
}

void medDiffusionSelectorToolBox::clear(void)
{
    d->currentToolBox = 0;

    d->methodCombo->blockSignals (true);
    d->methodCombo->setCurrentIndex (0);
    d->methodCombo->blockSignals (false);

    this->clearInputs();
}
