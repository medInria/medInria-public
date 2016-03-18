/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDiffusionModelEstimationProcessPresenter.h>

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QCheckBox>

#include <medAbstractImageData.h>
#include <medAbstractDiffusionModelImageData.h>
#include <medViewContainerSplitter.h>
#include <medViewContainer.h>
#include <medDataManager.h>
#include <medAbstractImageView.h>

class medAbstractDiffusionModelEstimationProcessPresenterPrivate
{
public:
    medAbstractDiffusionModelEstimationProcess *process;
    QLabel *gradientFileLabel;
    QLabel *bvaluesFileLabel;
    bool gradientsInImageCoordinates;
    bool useRunControls;
};

medAbstractDiffusionModelEstimationProcessPresenter::medAbstractDiffusionModelEstimationProcessPresenter(medAbstractDiffusionModelEstimationProcess *parent)
    : medAbstractProcessPresenter(parent), d(new medAbstractDiffusionModelEstimationProcessPresenterPrivate)
{
    d->process = parent;
    d->useRunControls = true;

    connect(d->process, &medAbstractDiffusionModelEstimationProcess::finished,
            this, &medAbstractDiffusionModelEstimationProcessPresenter::_importOutput,
            Qt::QueuedConnection);
}

medAbstractDiffusionModelEstimationProcessPresenter::~medAbstractDiffusionModelEstimationProcessPresenter()
{
}

void medAbstractDiffusionModelEstimationProcessPresenter::setUseRunControls(bool useRun)
{
    d->useRunControls = useRun;
}

bool medAbstractDiffusionModelEstimationProcessPresenter::useRunControls()
{
    return d->useRunControls;
}

QWidget *medAbstractDiffusionModelEstimationProcessPresenter::buildToolBoxWidget()
{
    QWidget *tbWidget = new QWidget;
    QVBoxLayout *tbLayout = new QVBoxLayout;
    tbWidget->setLayout(tbLayout);

    // Gradients
    QHBoxLayout *gradientFileLayout = new QHBoxLayout;
    QLabel *gradientDescriptionLabel = new QLabel(tbWidget);
    gradientDescriptionLabel->setText(tr("Gradient file:"));
    gradientFileLayout->addWidget(gradientDescriptionLabel);

    d->gradientFileLabel = new QLabel(tbWidget);
    d->gradientFileLabel->setText("None");
    d->gradientFileLabel->setAlignment(Qt::AlignRight);
    d->gradientFileLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    gradientFileLayout->addWidget(d->gradientFileLabel);

    QPushButton *browseButton = new QPushButton("...",tbWidget);
    browseButton->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    connect(browseButton, SIGNAL(clicked()), this, SLOT(setInputGradientFile()));
    gradientFileLayout->addWidget(browseButton);

    tbLayout->addLayout(gradientFileLayout);

    // B-values
    QHBoxLayout *bvaluesFileLayout = new QHBoxLayout;
    QLabel *bvaluesDescriptionLabel = new QLabel(tbWidget);
    bvaluesDescriptionLabel->setText(tr("B-values file:"));
    bvaluesFileLayout->addWidget(bvaluesDescriptionLabel);

    d->bvaluesFileLabel = new QLabel(tbWidget);
    d->bvaluesFileLabel->setText("None");
    d->bvaluesFileLabel->setAlignment(Qt::AlignRight);
    d->bvaluesFileLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    bvaluesFileLayout->addWidget(d->bvaluesFileLabel);

    QPushButton *browseBValuesButton = new QPushButton("...",tbWidget);
    browseBValuesButton->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    connect(browseBValuesButton, SIGNAL(clicked()), this, SLOT(setInputBValuesFile()));
    bvaluesFileLayout->addWidget(browseBValuesButton);

    tbLayout->addLayout(bvaluesFileLayout);

    if (d->useRunControls)
    {
        tbLayout->addWidget(this->buildRunButton());
        tbLayout->addWidget(this->buildCancelButton());
    }

    return tbWidget;
}

medViewContainerSplitter *medAbstractDiffusionModelEstimationProcessPresenter::buildViewContainerSplitter()
{
    medViewContainerSplitter* splitter = new medViewContainerSplitter;
    medViewContainer *inputContainer = new medViewContainer;
    splitter->addViewContainer(inputContainer);

    inputContainer->setDefaultWidget(new QLabel("Input DWI"));
    inputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    inputContainer->setUserSplittable(false);
    inputContainer->setMultiLayered(true);

    connect(inputContainer, &medViewContainer::dataAdded,
            this, &medAbstractDiffusionModelEstimationProcessPresenter::_setInputFromContainer);

    connect(this, SIGNAL(_outputImported(medAbstractData*)),
            inputContainer, SLOT(addData(medAbstractData*)),
            Qt::QueuedConnection);

    return splitter;
}

void medAbstractDiffusionModelEstimationProcessPresenter::_setInputFromContainer(medAbstractData *data)
{
    d->process->setInput(qobject_cast<medAbstractImageData *>(data));
}

void medAbstractDiffusionModelEstimationProcessPresenter::_importOutput(medAbstractJob::medJobExitStatus jobExitStatus)
{
    if(jobExitStatus == medAbstractJob::MED_JOB_EXIT_SUCCESS)
    {
        medDataManager::instance()->importData(d->process->output());
        emit _outputImported(d->process->output());
    }
}

void medAbstractDiffusionModelEstimationProcessPresenter::setInputGradientFile()
{
    QFileDialog *gradientFileDialog = new QFileDialog(0, tr("Choose a gradient file"));
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

    d->process->setGradients(fileName,gradientsInImageCoordinates);

    d->gradientFileLabel->setText(fileName);
    d->gradientFileLabel->setToolTip(fileName);
}


void medAbstractDiffusionModelEstimationProcessPresenter::setInputBValuesFile()
{
    QFileDialog *bvaluesFileDialog = new QFileDialog(0, tr("Choose a b-values file"));
    bvaluesFileDialog->setAcceptMode(QFileDialog::AcceptOpen);

    QString fileName;
    if ( bvaluesFileDialog->exec() )
        fileName = bvaluesFileDialog->selectedFiles().first();

    delete bvaluesFileDialog;

    if (fileName.isEmpty())
        return;

    d->process->setBValues(fileName);

    d->bvaluesFileLabel->setText(fileName);
    d->bvaluesFileLabel->setToolTip(fileName);
}
