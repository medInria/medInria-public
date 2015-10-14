/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkProcessRegistrationDiffeomorphicDemons.h>
#include <itkProcessRegistrationDiffeomorphicDemonsToolBox.h>

#include <QtGui>

#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <medAbstractImageData.h>
#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <dtkCoreSupport/dtkAbstractProcess.h>
#include <dtkCoreSupport/dtkAbstractViewFactory.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractView.h>
#include <medRunnableProcess.h>
#include <medJobManager.h>
#include <medPluginManager.h>

#include <medToolBoxFactory.h>
#include <medRegistrationSelectorToolBox.h>
#include <medProgressionStack.h>

#include "meditkRegistrationDiffeomorphicDemonsProcess.h"
#include "meditkRegistrationDiffeomorphicDemonsProcessToolbox.h"

class meditkProcessRegistrationDiffeomorphicDemonsToolBoxPrivate
{
public:

    medProgressionStack * progressionStack;
    QComboBox * updateRuleBox;
    QComboBox * gradientTypeBox;
    QDoubleSpinBox * maxStepLengthBox;
    QDoubleSpinBox * disFieldStdDevBox;
    QDoubleSpinBox * updateFieldStdDevBox;
    QCheckBox * useHistogramBox;
    QLineEdit * iterationsBox;
    QLineEdit* outputFile;
};

meditkProcessRegistrationDiffeomorphicDemonsToolBox::meditkProcessRegistrationDiffeomorphicDemonsToolBox(QWidget *parent) : medAbstractRegistrationToolbox(parent), d(new meditkProcessRegistrationDiffeomorphicDemonsToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    QFormLayout *layout = new QFormLayout(widget);

    d->iterationsBox = new QLineEdit(this);
    d->iterationsBox->setText("15x10x5");
    d->iterationsBox->setToolTip(tr("Each number of iteration per level must be separated by \"x\". From coarser to finest levels"));

    d->maxStepLengthBox = new QDoubleSpinBox(this);
    d->maxStepLengthBox->setMinimum(0);
    d->maxStepLengthBox->setMaximum(1000);
    d->maxStepLengthBox->setSingleStep(0.01);
    d->maxStepLengthBox->setValue(2.0);
    d->maxStepLengthBox->setToolTip(tr(
                "Maximum length of an update vector (voxel units)."
                " Setting it to 0 implies no restrictions will be made"
                " on the step length."));

    d->updateFieldStdDevBox = new QDoubleSpinBox(this);
    d->updateFieldStdDevBox->setMinimum(0);
    d->updateFieldStdDevBox->setMaximum(1000);
    d->updateFieldStdDevBox->setSingleStep(0.01);
    d->updateFieldStdDevBox->setValue(0.0);
    d->updateFieldStdDevBox->setToolTip(tr(
                "Standard deviation of the Gaussian smoothing"
                "of the update field (voxel units). Setting it below 0.1"
                "means no smoothing will be performed (default 0.0)."));

    d->disFieldStdDevBox = new QDoubleSpinBox(this);
    d->disFieldStdDevBox->setMinimum(0);
    d->disFieldStdDevBox->setMaximum(1000);
    d->disFieldStdDevBox->setSingleStep(0.01);
    d->disFieldStdDevBox->setValue(1.5);
    d->disFieldStdDevBox->setToolTip(tr(
                "Standard deviation of the Gaussian smoothing of "
                "the displacement field (voxel units). Setting it below 0.1 "
                "means no smoothing will be performed (default 1.5)."));
    d->updateRuleBox = new QComboBox(this);
    QStringList updateRules;
    updateRules<< tr("Diffeomorphic") << tr ("Additive") << tr("Compositive");
    d->updateRuleBox->addItems(updateRules);
    //WARNING IF YOU CHANGE THE ORDER OF THE ITEMS, CHANGE TOOLTIPS also!!!
    d->updateRuleBox->setItemData(0,"s <- s o exp(u)",Qt::ToolTipRole);
    d->updateRuleBox->setItemData(1,"s <- s + u",Qt::ToolTipRole);
    d->updateRuleBox->setItemData(2,"s <- s o (Id+u)",Qt::ToolTipRole);

    d->gradientTypeBox = new QComboBox(this);
    d->gradientTypeBox->setToolTip(tr(
                "Type of gradient used for computing the demons force."));
    QStringList gradientTypes;
    gradientTypes<< tr("Symmetrized") << tr ("Fixed Image")
                 << tr("Warped Moving Image")
                 << tr("Mapped Moving Image");
    d->gradientTypeBox->addItems(gradientTypes);
    d->useHistogramBox =  new QCheckBox(this);
    d->useHistogramBox->setChecked(false);
    d->useHistogramBox->setToolTip(tr(
                "Use histogram matching before processing?"));

    d->outputFile=new QLineEdit(this);

    layout->addRow(new QLabel(tr("Iterations per level of res."),this),d->iterationsBox);
    layout->addRow(new QLabel(tr("Update Rule"),this),d->updateRuleBox);
    layout->addRow(new QLabel(tr("Gradient Type"),this),d->gradientTypeBox);
    layout->addRow(new QLabel(tr("Max. Update Step Length"),this),d->maxStepLengthBox);
    layout->addRow(new QLabel(tr("Update Field Std. Deviation"),this),d->updateFieldStdDevBox);
    layout->addRow(new QLabel(tr("Displ. Field Std. Deviation"),this),d->disFieldStdDevBox);
    layout->addRow(new QLabel(tr("Histogram Matching"),this),d->useHistogramBox);
    layout->addRow(new QLabel(tr("output file"),this),d->outputFile);
    // progression stack
}

void meditkProcessRegistrationDiffeomorphicDemonsToolBox::setProcess(medAbstractRegistrationProcess* process)
{
     meditkRegistrationDiffeomorphicDemonsProcess* specificProcess=dynamic_cast< meditkRegistrationDiffeomorphicDemonsProcess*>(process);
    if(!specificProcess)
        return;

    connect(d->disFieldStdDevBox,SIGNAL(valueChanged(double)),specificProcess,SLOT(setDisplacementFieldStandardDeviation(double)));
    connect(d->gradientTypeBox,SIGNAL(currentIndexChanged(int)),specificProcess,SLOT(setGradientType(int)));
    connect(d->iterationsBox,SIGNAL(textEdited(QString)),specificProcess,SLOT(setNumberOfIterations(QString)));
    connect(d->maxStepLengthBox,SIGNAL(valueChanged(double)),specificProcess,SLOT(setMaximumUpdateLength(double)));
    connect(d->updateFieldStdDevBox,SIGNAL(valueChanged(double)),specificProcess,SLOT(setUpdateFieldStandardDeviation(double)));
    connect(d->updateRuleBox,SIGNAL(currentIndexChanged(int)),specificProcess,SLOT(setUpdateRule(int)));
    connect(d->useHistogramBox,SIGNAL(toggled(bool)),specificProcess,SLOT(setUseHistogramMatching(bool)));
    connect(d->outputFile,SIGNAL(textEdited(QString)),specificProcess,SLOT(setOutputFile(QString)));
}

meditkProcessRegistrationDiffeomorphicDemonsToolBox::~meditkProcessRegistrationDiffeomorphicDemonsToolBox()
{
    delete d;

    d = NULL;
}
