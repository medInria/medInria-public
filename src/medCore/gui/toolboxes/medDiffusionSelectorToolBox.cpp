/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDataFactory.h>
#include <medAbstractDiffusionProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include <medAbstractImageData.h>
#include <medPluginManager.h>

#include <medDropSite.h>
#include <medDataManager.h>
#include <medToolBoxTab.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>
#include <medMessageController.h>
#include <medMetaDataKeys.h>

#include "medDiffusionSelectorToolBox.h"
#include "medDiffusionAbstractToolBox.h"

class medDiffusionSelectorToolBoxPrivate
{
public:

    QHash<QString, medDiffusionAbstractToolBox*> toolBoxes;
    medDiffusionAbstractToolBox *currentToolBox;

    medDiffusionSelectorToolBox::SelectorType selectorType;

    QLabel *gradientFileLabel;
    QPushButton *browseButton;

    QPushButton *runButton;
    QPushButton *cancelButton;

    QComboBox *methodCombo;
    QVBoxLayout *mainLayout;

    //QLabel *inputLabel;
    QComboBox *chooseInput;
    //TODO smartPointing have to be managed only in abstract processes -rde
    QMap <QString, dtkSmartPointer <medAbstractImageData> > inputsMap;
};

medDiffusionSelectorToolBox::medDiffusionSelectorToolBox(QWidget *parent, SelectorType type) : medToolBox(parent), d(new medDiffusionSelectorToolBoxPrivate)
{
    d->currentToolBox = 0;
    d->selectorType = type;

    // /////////////////////////////////////////////////////////////////
    // main page
    // /////////////////////////////////////////////////////////////////
    QWidget *mainPage = new QWidget(this);

    QString labelTitle;
    QString toolboxCategory;
    QString runButtonText;

    switch (type)
    {
        case Estimation:
            labelTitle = tr("Estimation algorithm:");
            toolboxCategory = "diffusion-estimation";
            runButtonText = tr("Estimate model");
            break;

        case ScalarMaps:
            labelTitle = tr("Scalar maps:");
            toolboxCategory = "diffusion-scalar-maps";
            runButtonText = "";
            break;

        case Tractography:
        default:
            labelTitle = tr("Tractography algorithm:");
            toolboxCategory = "diffusion-tractography";
            runButtonText = tr("Track fibers");
            break;
    }

    QLabel *methodLabel = new QLabel(labelTitle, mainPage);
    d->methodCombo = new QComboBox(mainPage);

    QHBoxLayout *methodLayout = new QHBoxLayout;
    methodLayout->addWidget(methodLabel);
    methodLayout->addWidget(d->methodCombo);

    d->mainLayout = new QVBoxLayout(mainPage);
    d->mainLayout->addLayout(methodLayout);

    // --- Setting up custom toolboxes list ---
    d->methodCombo->addItem( tr( "Choose..." ) );
    medToolBoxFactory* tbFactory = medToolBoxFactory::instance();
    int i=1;
    foreach(QString toolbox, tbFactory->toolBoxesFromCategory(toolboxCategory))
    {
        medToolBoxDetails* details = tbFactory->toolBoxDetailsFromId(toolbox);
        d->methodCombo->addItem(details->name, toolbox);
        d->methodCombo->setItemData(i,
                                  details->description,
                                  Qt::ToolTipRole);
        i++;
    }

    connect(d->methodCombo, SIGNAL(activated(int)), this, SLOT(chooseToolBox(int)));

    QHBoxLayout *inputLayout = new QHBoxLayout;
    QLabel *inputDescriptionLabel = new QLabel(mainPage);
    inputDescriptionLabel->setText(tr("Input image:"));
    inputLayout->addWidget(inputDescriptionLabel);
    
    d->chooseInput = new QComboBox(mainPage);
    d->chooseInput->addItem(tr("Please drop an image"));
	d->chooseInput->setToolTip(tr("Browse available images for processing"));
    d->chooseInput->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    inputLayout->addWidget(d->chooseInput);
    d->mainLayout->addLayout(inputLayout);

    d->browseButton = 0;
    d->runButton = 0;
    d->cancelButton = 0;

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

    if (d->selectorType != ScalarMaps)
    {
        d->runButton = new QPushButton(runButtonText, mainPage);
        d->runButton->setEnabled(false);
        d->mainLayout->addWidget(d->runButton);

        connect(d->runButton,SIGNAL(clicked()),this,SLOT(createProcess()));

        d->cancelButton = new QPushButton(tr("Cancel"),mainPage);
        d->cancelButton->hide();
        d->mainLayout->addWidget(d->cancelButton);

        connect(d->cancelButton,SIGNAL(clicked()),this,SIGNAL(processCancelled()));
    }

    this->addWidget(mainPage);
    this->setEnabled(false);
}

medDiffusionSelectorToolBox::~medDiffusionSelectorToolBox(void)
{
    delete d;
    d = NULL;
}

void medDiffusionSelectorToolBox::chooseToolBox(int id)
{
    medDiffusionAbstractToolBox *toolbox = NULL;
    //get identifier for toolbox.
    QString identifier = d->methodCombo->itemData(id).toString();
    if (d->toolBoxes.contains (identifier))
        toolbox = d->toolBoxes[identifier];
    else
    {
        medToolBox* tb = medToolBoxFactory::instance()->createToolBox(identifier, this);
        toolbox = qobject_cast<medDiffusionAbstractToolBox*>(tb);

        if (toolbox)
        {
            toolbox->header()->hide();
            d->toolBoxes[identifier] = toolbox;

            if (d->selectorType == ScalarMaps)
            {
                medDiffusionScalarMapsAbstractToolBox *scalarToolBox = qobject_cast <medDiffusionScalarMapsAbstractToolBox *> (toolbox);
                connect(scalarToolBox,SIGNAL(processStartRequested()),this,SLOT(createProcess()));
            }
        }
    }

    if(!toolbox)
    {
        if (d->currentToolBox)
        {
            d->currentToolBox->hide();
            d->mainLayout->removeWidget ( d->currentToolBox );
            d->currentToolBox = 0;
            this->setAboutPluginVisibility(false);

            if (d->selectorType != ScalarMaps)
                d->runButton->setEnabled(false);
        }

        return;
    }

    //get rid of old toolBox
    if (d->currentToolBox) {
        d->currentToolBox->hide();
        d->mainLayout->removeWidget ( d->currentToolBox );
        d->currentToolBox = 0;
    }

    //set the aboutPlugin button.
    //get the plugin from the custom toolbox,
    //since the custom shares the same header as this.
    dtkPlugin* plugin = toolbox->plugin();
    this->setAboutPluginButton(plugin);
    this->setAboutPluginVisibility(true);

    toolbox->show();
    d->mainLayout->addWidget ( toolbox );

    d->currentToolBox = toolbox;

    if (d->selectorType != ScalarMaps)
        d->runButton->setEnabled(true);
}

void medDiffusionSelectorToolBox::addInputImage(medAbstractImageData *data)
{
    if (!data)
        return;

    this->setEnabled(true);
    
    QUuid dataId = QUuid::createUuid();
    if ((d->chooseInput->count() == 1)&&(d->chooseInput->itemData(0) == QVariant()))
        d->chooseInput->removeItem(0);
    
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

    medAbstractData *gradients = medAbstractDataFactory::instance()->create ("medItkDiffusionGradientListData");
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

void medDiffusionSelectorToolBox::createProcess()
{
    QString inputId = d->chooseInput->itemData(d->chooseInput->currentIndex()).toString();
    medAbstractImageData *input = d->inputsMap[inputId];
    
    if (!input)
        return;

    if ((d->selectorType == Estimation) && (!input->hasMetaData("DiffusionGradientList")))
    {
        medMessageController::instance()->showError("No diffusion gradient data provided for estimation",3000);
        return;
    }

    QString processText;

    switch (d->selectorType)
    {
        case Estimation:
            processText = tr("Model estimation");
            break;

        case ScalarMaps:
            processText = tr("Scalar map computation");
            break;

        case Tractography:
        default:
            processText = tr("Fiber tracking");
            break;
    }

    if (d->runButton)
    {
        d->runButton->hide();
        d->cancelButton->show();
    }

    QString processName = d->currentToolBox->processName();
    emit processRequested(processName, processText);
}

void medDiffusionSelectorToolBox::setProcessParameters(medAbstractDiffusionProcess *process)
{
    QString inputId = d->chooseInput->itemData(d->chooseInput->currentIndex()).toString();
    medAbstractImageData *input = d->inputsMap[inputId];
    
    process->setInputImage(input);
    d->currentToolBox->setProcessParameters(process);
}

void medDiffusionSelectorToolBox::resetButtons()
{
    if (d->runButton)
    {
        d->cancelButton->hide();
        d->runButton->show();
    }
}

void medDiffusionSelectorToolBox::clear(void)
{
    foreach (medDiffusionAbstractToolBox *tb, d->toolBoxes)
        tb->deleteLater();

    d->toolBoxes.clear();

    d->currentToolBox = 0;

    d->methodCombo->blockSignals (true);
    d->methodCombo->setCurrentIndex (0);
    d->methodCombo->blockSignals (false);
    this->setAboutPluginVisibility(false);

    this->clearInputs();

    if (d->selectorType != ScalarMaps)
        d->runButton->setEnabled(false);
}
