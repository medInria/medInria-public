/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include <medAbstractDataImage.h>
#include <medPluginManager.h>
#include <medViewManager.h>

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
    
    QPushButton *runButton;
    QPushButton *cancelButton;
    
    QComboBox *methodCombo;
    QVBoxLayout *mainLayout;
    
    QLabel *inputLabel;
    
    dtkSmartPointer <medAbstractDataImage> input;
};

medDiffusionSelectorToolBox::medDiffusionSelectorToolBox(QWidget *parent, SelectorType type) : medToolBox(parent), d(new medDiffusionSelectorToolBoxPrivate)
{
    d->currentToolBox = 0;
    d->selectorType = type;

    // /////////////////////////////////////////////////////////////////
    // main page
    // /////////////////////////////////////////////////////////////////
    QWidget *mainPage        = new QWidget(this);
    
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
    
    QLabel  *methodLabel = new QLabel(labelTitle, mainPage);
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
    
    QHBoxLayout *inputLabelLayout = new QHBoxLayout;
    QLabel *inputDescriptionLabel = new QLabel(mainPage);
    inputDescriptionLabel->setText(tr("Input image:"));
    inputLabelLayout->addWidget(inputDescriptionLabel);
    d->inputLabel = new QLabel(mainPage);
    d->inputLabel->setText("None");
    d->inputLabel->setAlignment(Qt::AlignRight);
    d->inputLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    inputLabelLayout->addWidget(d->inputLabel);
    d->mainLayout->addLayout(inputLabelLayout);
    
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

void medDiffusionSelectorToolBox::selectInputImage(const medDataIndex& index)
{
    if (!index.isValid())
        return;
    
    dtkSmartPointer <medAbstractDataImage> data = medDataManager::instance()->data (index);
    
    this->setInputImage(data);
}

void medDiffusionSelectorToolBox::setInputImage(medAbstractDataImage *data)
{
    if (!data)
        return;

    d->input = data;
    
    d->inputLabel->setText(data->metadata(medMetaDataKeys::SeriesDescription.key()));
    
    if (d->selectorType == Estimation)
        this->checkInputGradientDirections();
}

void medDiffusionSelectorToolBox::clearInput()
{
    d->input = 0;
    d->inputLabel->setText("None");
}

void medDiffusionSelectorToolBox::checkInputGradientDirections()
{
    if (!d->input)
        return;
        
    if (!d->input->hasMetaData ("DiffusionGradientList"))
    {
        QMessageBox msgBox;
        msgBox.setText("No diffusion gradient attached to the image.");
        msgBox.setInformativeText("Do you want to specify them manually?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        
        switch (ret)
        {
            case QMessageBox::Ok:
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
                
                dtkAbstractData *gradients = dtkAbstractDataFactory::instance()->create ("itkDataDiffusionGradientList");
                if (!gradients)
                    return;
                
                if (!gradients->read(fileName))
                    return;
                
                int i=0;
                QStringList gradientList;
                
                medAbstractDataImage::MatrixType orientationMatrix;
                orientationMatrix = d->input->orientationMatrix();
                
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
                
                if (d->input->tDimension()==gradientList.count()/3)
                    d->input->addMetaData ("DiffusionGradientList", gradientList);
                else if (d->input->tDimension()==gradientList.count()/3+1)
                {
                    // add the null gradient in this case
                    gradientList.push_front("0.0");
                    gradientList.push_front("0.0");
                    gradientList.push_front("0.0");
                    d->input->addMetaData ("DiffusionGradientList", gradientList);
                }
                else
                {
                    QString gradCount, imDims;
                    gradCount.setNum(gradientList.count()/3);
                    imDims.setNum(d->input->tDimension());
                    medMessageController::instance()->showError("Mismatch between gradient length (" +gradCount + ") and image dimension (" + imDims + ").",3000);
                    return;
                }
                
                break;
            }
                
            case QMessageBox::Cancel:
                return;
                
            default:
                return;
        }
    }
}

void medDiffusionSelectorToolBox::createProcess()
{
    if (!d->input)
        return;
    
    dtkAbstractProcess *process = d->currentToolBox->createProcess();
    
    process->setInput(d->input);
    
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
    
    emit processCreated(process, processText);
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

    this->clearInput();
    
    if (d->selectorType != ScalarMaps)
        d->runButton->setEnabled(false);
}
