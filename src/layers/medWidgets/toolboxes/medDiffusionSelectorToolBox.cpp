/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
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
#include <medAbstractDiffusionModelEstimationProcess.h>
#include <medAbstractDiffusionScalarMapsProcess.h>
#include <medAbstractTractographyProcess.h>
#include <medAbstractDiffusionModelEstimationProcessPresenter.h>
#include <medAbstractDiffusionScalarMapsProcessPresenter.h>
#include <medAbstractTractographyProcessPresenter.h>

class medDiffusionSelectorToolBoxPrivate
{
public:
    medDiffusionSelectorToolBox::SelectorType selectorType;

    QLabel *gradientFileLabel;
    QPushButton *browseButton;

    medAbstractProcess *currentProcess;
    medAbstractProcessPresenter *currentProcessPresenter;

    QComboBox *methodCombo;
    QVBoxLayout *mainLayout;

    QComboBox *chooseInput;
    QMap <QString, dtkSmartPointer <medAbstractImageData> > inputsMap;

    QWidget *currentToolBox;

    dtkSmartPointer <medAbstractData> processOutput;
};

medDiffusionSelectorToolBox::medDiffusionSelectorToolBox(QWidget *parent, SelectorType type) : medToolBox(parent), d(new medDiffusionSelectorToolBoxPrivate)
{
    d->currentProcess = 0;
    d->currentProcessPresenter = 0;
    d->currentToolBox = 0;
    d->processOutput = 0;
    d->selectorType = type;

    // /////////////////////////////////////////////////////////////////
    // main page
    // /////////////////////////////////////////////////////////////////
    QWidget *mainPage = new QWidget(this);
    QString labelTitle;

    d->methodCombo = new QComboBox(mainPage);
    // --- Setting up custom toolboxes list ---
    d->methodCombo->addItem( tr( "Choose..." ) );

    switch(d->selectorType)
    {
        case Estimation:
            labelTitle = tr("Estimation algorithm:");
            foreach(QString pluginKey, medCore::diffusionModelEstimation::pluginFactory().keys())
            {
                medAbstractProcess *process = medCore::diffusionModelEstimation::pluginFactory().create(pluginKey);
                d->methodCombo->addItem(process->caption(),pluginKey);
            }
            break;

        case ScalarMaps:
            labelTitle = tr("Scalar maps:");
            foreach(QString pluginKey, medCore::diffusionScalarMaps::pluginFactory().keys())
            {
                medAbstractProcess *process = medCore::diffusionScalarMaps::pluginFactory().create(pluginKey);
                d->methodCombo->addItem(process->caption(),pluginKey);
            }
            break;

        case Tractography:
        default:
            labelTitle = tr("Tractography algorithm:");
            foreach(QString pluginKey, medCore::tractography::pluginFactory().keys())
            {
                medAbstractProcess *process = medCore::tractography::pluginFactory().create(pluginKey);
                d->methodCombo->addItem(process->caption(),pluginKey);
            }
            break;
    }

    QLabel *methodLabel = new QLabel(labelTitle, mainPage);
    QHBoxLayout *methodLayout = new QHBoxLayout;
    methodLayout->addWidget(methodLabel);
    methodLayout->addWidget(d->methodCombo);

    d->mainLayout = new QVBoxLayout(mainPage);
    d->mainLayout->addLayout(methodLayout);

    connect(d->methodCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(chooseProcess(int)));

    QHBoxLayout *inputLayout = new QHBoxLayout;
    QLabel *inputDescriptionLabel = new QLabel(mainPage);
    inputDescriptionLabel->setText(tr("Input image:"));
    inputLayout->addWidget(inputDescriptionLabel);
    
    d->chooseInput = new QComboBox(mainPage);
    d->chooseInput->addItem(tr("Please drop an image"));
	d->chooseInput->setToolTip(tr("Browse available images for processing"));
    d->chooseInput->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    connect(d->chooseInput,SIGNAL(currentIndexChanged(int)),this,SLOT(updateCurrentProcessInput(int)));
    inputLayout->addWidget(d->chooseInput);
    d->mainLayout->addLayout(inputLayout);

    d->browseButton = 0;

    if (d->selectorType == Estimation)
    {
        QHBoxLayout *gradientFileLayout = new QHBoxLayout;
        QLabel *gradientDescriptionLabel = new QLabel(mainPage);
        gradientDescriptionLabel->setText(tr("Gradient file:"));
        gradientFileLayout->addWidget(gradientDescriptionLabel);

        d->gradientFileLabel = new QLabel(mainPage);
        d->gradientFileLabel->setText("None");
        d->gradientFileLabel->setAlignment(Qt::AlignRight);
        d->gradientFileLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
        gradientFileLayout->addWidget(d->gradientFileLabel);

        d->browseButton = new QPushButton("...",mainPage);
        d->browseButton->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
        connect(d->browseButton, SIGNAL(clicked()), this, SLOT(setInputGradientFile()));
        gradientFileLayout->addWidget(d->browseButton);

        d->mainLayout->addLayout(gradientFileLayout);
    }

    this->addWidget(mainPage);
    this->setEnabled(false);
}

medDiffusionSelectorToolBox::~medDiffusionSelectorToolBox(void)
{
    delete d;
    d = NULL;
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
            medAbstractDiffusionModelEstimationProcess *process = medCore::diffusionModelEstimation::pluginFactory().create(identifier);
            d->currentProcess = process;

            if (d->inputsMap[inputId])
                process->setInput(d->inputsMap[inputId]);

            connect(process,SIGNAL(finished(medJobExitStatus)),this,SIGNAL(jobFinished(medJobExitStatus)));
            connect(process,SIGNAL(running(bool)),this,SIGNAL(jobRunning(bool)));

            d->currentProcessPresenter = medWidgets::diffusionModelEstimation::presenterFactory().create(process);
            break;
        }

        case ScalarMaps:
        {
            medAbstractDiffusionScalarMapsProcess *process = medCore::diffusionScalarMaps::pluginFactory().create(identifier);
            d->currentProcess = process;

            if (d->inputsMap[inputId])
            {
                medAbstractDiffusionModelImageData *image = qobject_cast <medAbstractDiffusionModelImageData *> (d->inputsMap[inputId]);
                process->setInput(image);
            }

            connect(process,SIGNAL(finished(medJobExitStatus)),this,SIGNAL(jobFinished(medJobExitStatus)));
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
                medAbstractDiffusionModelImageData *image = qobject_cast <medAbstractDiffusionModelImageData *> (d->inputsMap[inputId]);
                process->setInput(image);
            }

            connect(process,SIGNAL(finished(medJobExitStatus)),this,SIGNAL(jobFinished(medJobExitStatus)));
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
        d->currentToolBox = 0;
    }

    d->currentToolBox = d->currentProcessPresenter->buildToolBoxWidget();
    d->mainLayout->addWidget(d->currentToolBox);
}

medAbstractData *medDiffusionSelectorToolBox::processOutput()
{
    if (!d->currentProcess)
        return 0;

    switch(d->selectorType)
    {
        case Estimation:
        {
            medAbstractDiffusionModelEstimationProcess *process = qobject_cast <medAbstractDiffusionModelEstimationProcess *> (d->currentProcess);
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
            medAbstractDiffusionModelEstimationProcess *process = qobject_cast <medAbstractDiffusionModelEstimationProcess *> (d->currentProcess);
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
    
    if (d->selectorType == Estimation)
        this->checkInputGradientDirections();
}

void medDiffusionSelectorToolBox::clearInputs()
{
    d->chooseInput->clear();
    d->chooseInput->addItem(tr("Please drop an image"));
	d->chooseInput->setToolTip(tr("Browse available images for processing"));
    
    d->inputsMap.clear();

    if (d->selectorType == Estimation)
    {
        d->gradientFileLabel->setText("None");
        d->gradientFileLabel->setToolTip("None");
    }

    this->setEnabled(false);
}

void medDiffusionSelectorToolBox::checkInputGradientDirections()
{
    QString dataId = d->chooseInput->itemData(d->chooseInput->currentIndex()).toString();
    if (dataId == "")
        return;
    
    if (!d->inputsMap[dataId])
        return;

    if (d->inputsMap[dataId]->hasMetaData ("DiffusionGradientList"))
    {
        d->gradientFileLabel->setText("Already provided");
        d->gradientFileLabel->setToolTip("Already provided");
    }
}

void medDiffusionSelectorToolBox::setInputGradientFile()
{
    QFileDialog * gradientFileDialog = new QFileDialog(0, tr("Exporting : please choose a file name and directory"));
    gradientFileDialog->setOption(QFileDialog::DontUseNativeDialog);
    gradientFileDialog->setAcceptMode(QFileDialog::AcceptOpen);

    QCheckBox* gradientsInImageCoordinatesCheckBox = new QCheckBox(gradientFileDialog);
    gradientsInImageCoordinatesCheckBox->setChecked(true);
    gradientsInImageCoordinatesCheckBox->setToolTip(tr("Uncheck this box if your gradients are in world coordinates."));

    QLayout* layout = gradientFileDialog->layout();
    QGridLayout* gridbox = qobject_cast<QGridLayout*>(layout);

    // nasty hack to hide the filter list
    QWidget * filtersLabel = gridbox->itemAtPosition(gridbox->rowCount()-1, 0)->widget();
    QWidget * filtersList = gridbox->itemAtPosition(gridbox->rowCount()-1, 1)->widget();
    filtersLabel->hide(); filtersList->hide();

    if (gridbox)
    {
        gridbox->addWidget(new QLabel("Gradients in image coordinates?", gradientFileDialog), gridbox->rowCount()-1, 0);
        gridbox->addWidget(gradientsInImageCoordinatesCheckBox, gridbox->rowCount()-1, 1);
    }

    gradientFileDialog->setLayout(gridbox);

    QString fileName;
    bool gradientsInImageCoordinates = false;
    if ( gradientFileDialog->exec() )
    {
        fileName = gradientFileDialog->selectedFiles().first();
        gradientsInImageCoordinates = gradientsInImageCoordinatesCheckBox->isChecked();
    }
    
    delete gradientFileDialog;

    if (fileName.isEmpty())
        return;

    medAbstractData *gradients = medAbstractDataFactory::instance()->create ("itkDataDiffusionGradientList");
    if (!gradients)
        return;

    if (!gradients->read(fileName))
        return;

    int i=0;
    QStringList gradientList;

    QString inputId = d->chooseInput->itemData(d->chooseInput->currentIndex()).toString();
    medAbstractImageData *input = d->inputsMap[inputId];

    medAbstractImageData::MatrixType orientationMatrix;
    orientationMatrix = input->orientationMatrix();

    while (double *grad=(double *)(gradients->data(i)))
    {
        double gradVals[3];
        for (unsigned int j = 0;j < 3;++j)
            gradVals[j] = grad[j];

        if (!gradientsInImageCoordinates)
        {
            for (unsigned int j = 0;j < 3;++j)
            {
                gradVals[j] = 0;
                for (unsigned int k = 0;k < 3;++k)
                    gradVals[j] += orientationMatrix[k][j] * grad[k];
            }
        }

        QString s_gx, s_gy, s_gz;
        s_gx = QString::number (gradVals[0], 'g', 10);
        s_gy = QString::number (gradVals[1], 'g', 10);
        s_gz = QString::number (gradVals[2], 'g', 10);
        gradientList.append (s_gx);
        gradientList.append (s_gy);
        gradientList.append (s_gz);
        i++;
    }
    
    if (input->tDimension()==gradientList.count()/3)
    {
        input->setMetaData ("DiffusionGradientList", gradientList);
        d->gradientFileLabel->setText(fileName);
        d->gradientFileLabel->setToolTip(fileName);
    }
    else if (input->tDimension()==gradientList.count()/3+1)
    {
        // add the null gradient in this case
        gradientList.push_front("0.0");
        gradientList.push_front("0.0");
        gradientList.push_front("0.0");
        input->setMetaData ("DiffusionGradientList", gradientList);
        d->gradientFileLabel->setText(fileName);
        d->gradientFileLabel->setToolTip(fileName);
    }
    else
    {
        QString gradCount, imDims;
        gradCount.setNum(gradientList.count()/3);
        imDims.setNum(input->tDimension());
        medMessageController::instance()->showError("Mismatch between gradient length (" +gradCount + ") and image dimension (" + imDims + ").",3000);
    }
}

void medDiffusionSelectorToolBox::clear(void)
{
    d->currentToolBox = 0;

    d->methodCombo->blockSignals (true);
    d->methodCombo->setCurrentIndex (0);
    d->methodCombo->blockSignals (false);

    this->clearInputs();
}
