/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medTensorViewToolBox.h"
#include <dtkCore/dtkAbstractViewInteractor.h>
#include <dtkCore/dtkAbstractView.h>
#include <math.h>

class medTensorViewToolBoxPrivate
{
public:
    QComboBox*    glyphShapeComboBox;
    QSlider*      sampleRateSlider;
    QCheckBox*    flipXCheckBox;
    QCheckBox*    flipYCheckBox;
    QCheckBox*    flipZCheckBox;
    QRadioButton* eigenVectorV1RadioButton;
    QRadioButton* eigenVectorV2RadioButton;
    QRadioButton* eigenVectorV3RadioButton;
    QCheckBox*    reverseBackgroundColorCheckBox;
    QSlider*      glyphResolutionSlider;
    QSpinBox*     scaleBase;
    QSpinBox*     scaleExp;
    QCheckBox*    hideShowAxialCheckBox;
    QCheckBox*    hideShowCoronalCheckBox;
    QCheckBox*    hideShowSagittalCheckBox;

    QStringList glyphShapesList;
};

medTensorViewToolBox::medTensorViewToolBox(QWidget *parent) : medToolBox(parent), d(new medTensorViewToolBoxPrivate)
{
    QWidget* displayWidget = new QWidget(this);

    d->glyphShapesList = QStringList();
    d->glyphShapesList << "Lines" << "Disks" << "Arrows" << "Cubes" << "Cylinders" << "Ellipsoids" << "Superquadrics";

    // combobox to control the glyph shape
    QString glyphShapeToolTip = tr("Tensors will be displayed using the selected shape.");
    QLabel* glyphShapeLabel = new QLabel(tr("Shape:"));
    glyphShapeLabel->setToolTip(glyphShapeToolTip);
    d->glyphShapeComboBox = new QComboBox(displayWidget);
    d->glyphShapeComboBox->addItems(d->glyphShapesList);
    d->glyphShapeComboBox->setToolTip(glyphShapeToolTip);


    // slider to control sample rate
    QString sampleRateToolTip = tr("The higher this value, the more spread the sampling, hence fewer tensors will appear.");
    QLabel* sampleRateLabel = new QLabel(tr("Sample rate:"));
    sampleRateLabel->setToolTip(sampleRateToolTip);
    d->sampleRateSlider =  new QSlider(Qt::Horizontal, displayWidget);
    d->sampleRateSlider->setMinimum(1);
    d->sampleRateSlider->setMaximum(10);
    d->sampleRateSlider->setSingleStep(1);
    d->sampleRateSlider->setValue(1);
    d->sampleRateSlider->setToolTip(sampleRateToolTip);

    QSpinBox* sampleRateSpinBox = new QSpinBox(displayWidget);
    sampleRateSpinBox->setMinimum(1);
    sampleRateSpinBox->setMaximum(10);
    sampleRateSpinBox->setSingleStep(1);
    sampleRateSpinBox->setValue(1);
    sampleRateSpinBox->setToolTip(sampleRateToolTip);

    QHBoxLayout* sampleRateLayout = new QHBoxLayout;
    sampleRateLayout->addWidget(d->sampleRateSlider);
    sampleRateLayout->addWidget(sampleRateSpinBox);

    connect(d->sampleRateSlider, SIGNAL(valueChanged(int)), sampleRateSpinBox, SLOT(setValue(int)));
    connect(sampleRateSpinBox, SIGNAL(valueChanged(int)), d->sampleRateSlider, SLOT(setValue(int)));

    // flipX, flipY and flipZ checkboxes
    d->flipXCheckBox = new QCheckBox("X", displayWidget);
    d->flipXCheckBox->setToolTip(tr("Flip X axis."));
    d->flipYCheckBox = new QCheckBox("Y", displayWidget);
    d->flipYCheckBox->setToolTip(tr("Flip Y axis."));
    d->flipZCheckBox = new QCheckBox("Z", displayWidget);
    d->flipZCheckBox->setToolTip(tr("Flip Z axis."));

    QHBoxLayout* flipAxesLayout = new QHBoxLayout;
    flipAxesLayout->addWidget(d->flipXCheckBox);
    flipAxesLayout->addWidget(d->flipYCheckBox);
    flipAxesLayout->addWidget(d->flipZCheckBox);

    // eigen vectors
    QString eigenVectorToolTip = tr("Choose which eigenvector is used for mapping the color mode.");
    d->eigenVectorV1RadioButton = new QRadioButton("v1", displayWidget);
    d->eigenVectorV1RadioButton->setToolTip(eigenVectorToolTip);
    d->eigenVectorV2RadioButton = new QRadioButton("v2", displayWidget);
    d->eigenVectorV2RadioButton->setToolTip(eigenVectorToolTip);
    d->eigenVectorV3RadioButton = new QRadioButton("v3", displayWidget);
    d->eigenVectorV3RadioButton->setToolTip(eigenVectorToolTip);

    QButtonGroup *eigenVectorRadioGroup = new QButtonGroup(displayWidget);
    eigenVectorRadioGroup->addButton(d->eigenVectorV1RadioButton);
    eigenVectorRadioGroup->addButton(d->eigenVectorV2RadioButton);
    eigenVectorRadioGroup->addButton(d->eigenVectorV3RadioButton);
    eigenVectorRadioGroup->setExclusive(true);

    QHBoxLayout *eigenVectorGroupLayout = new QHBoxLayout;
    eigenVectorGroupLayout->addWidget(d->eigenVectorV1RadioButton);
    eigenVectorGroupLayout->addWidget(d->eigenVectorV2RadioButton);
    eigenVectorGroupLayout->addWidget(d->eigenVectorV3RadioButton);
    //eigenVectorGroupLayout->addStretch(1);

    d->eigenVectorV1RadioButton->setChecked(true);

    // reverse background color
    //d->reverseBackgroundColorCheckBox = new QCheckBox("Reverse Background Color", displayWidget);

    // slider to control glyph resolution
    QString glyphResolutionToolTip = tr("Increases or decreases the amount of detail of each tensor's glyph.");
    QLabel* glyphResolutionLabel = new QLabel(tr("Resolution:"));
    glyphResolutionLabel->setToolTip(glyphResolutionToolTip);
    d->glyphResolutionSlider =  new QSlider(Qt::Horizontal, displayWidget);
    d->glyphResolutionSlider->setMinimum(2);
    d->glyphResolutionSlider->setMaximum(20);
    d->glyphResolutionSlider->setSingleStep(1);
    d->glyphResolutionSlider->setValue(6);
    d->glyphResolutionSlider->setToolTip(glyphResolutionToolTip);

    QSpinBox* glyphResolutionSpinBox = new QSpinBox(displayWidget);
    glyphResolutionSpinBox->setMinimum(2);
    glyphResolutionSpinBox->setMaximum(20);
    glyphResolutionSpinBox->setSingleStep(1);
    glyphResolutionSpinBox->setValue(6);
    glyphResolutionSpinBox->setToolTip(glyphResolutionToolTip);

    QHBoxLayout* glyphResolutionLayout = new QHBoxLayout;
    glyphResolutionLayout->addWidget(d->glyphResolutionSlider);
    glyphResolutionLayout->addWidget(glyphResolutionSpinBox);

    connect(d->glyphResolutionSlider, SIGNAL(valueChanged(int)), glyphResolutionSpinBox, SLOT(setValue(int)));
    connect(glyphResolutionSpinBox, SIGNAL(valueChanged(int)), d->glyphResolutionSlider, SLOT(setValue(int)));

    // scale = scaleBase * 10 ^ scaleExp

    QString scaleToolTip = tr("Modifies the glyph scaling factor, hence its size.");
    QLabel* scaleLabel = new QLabel(tr("Scale:"));
    scaleLabel->setToolTip(scaleToolTip);

    d->scaleBase = new QSpinBox(displayWidget);
    d->scaleBase->setMinimum(1);
    d->scaleBase->setMaximum(9);
    d->scaleBase->setSingleStep(1);
    d->scaleBase->setValue(1);
    d->scaleBase->setToolTip(scaleToolTip);

    d->scaleExp = new QSpinBox(displayWidget);
    d->scaleExp->setMinimum(-10);
    d->scaleExp->setMaximum(10);
    d->scaleExp->setSingleStep(1);
    d->scaleExp->setValue(0);
    d->scaleExp->setToolTip(scaleToolTip);

    QLabel* toTheExpLabel = new QLabel(" x 10 ^ ", displayWidget);
    toTheExpLabel->setToolTip(scaleToolTip);
    QHBoxLayout* scaleLayout = new QHBoxLayout;
    scaleLayout->addWidget(d->scaleBase);
    scaleLayout->addWidget(toTheExpLabel);
    scaleLayout->addWidget(d->scaleExp);

    // hide or show axial, coronal, and sagittal
    QString hideShowAxesToolTip = tr("Hide or show a specific axis.");
    QLabel* hideShowAxesLabel = new QLabel(tr("Hide/show:"));
    hideShowAxesLabel->setToolTip(hideShowAxesToolTip);
    d->hideShowAxialCheckBox = new QCheckBox("Axial", displayWidget);
    d->hideShowAxialCheckBox->setToolTip(hideShowAxesToolTip);
    d->hideShowCoronalCheckBox = new QCheckBox("Coronal", displayWidget);
    d->hideShowCoronalCheckBox->setToolTip(hideShowAxesToolTip);
    d->hideShowSagittalCheckBox = new QCheckBox("Sagittal", displayWidget);
    d->hideShowSagittalCheckBox->setToolTip(hideShowAxesToolTip);

    d->hideShowAxialCheckBox->setChecked(true);
    d->hideShowCoronalCheckBox->setChecked(true);
    d->hideShowSagittalCheckBox->setChecked(true);

    QHBoxLayout* slicesLayout = new QHBoxLayout;
    slicesLayout->addWidget(d->hideShowAxialCheckBox);
    slicesLayout->addWidget(d->hideShowCoronalCheckBox);
    slicesLayout->addWidget(d->hideShowSagittalCheckBox);

    // layout all the controls in the toolbox
    QFormLayout *layout = new QFormLayout(displayWidget);
    layout->addRow(glyphShapeLabel, d->glyphShapeComboBox);
    layout->addRow(sampleRateLabel, sampleRateLayout);
    layout->addRow(tr("Flip:"), flipAxesLayout);
    layout->addRow(tr("Color with:"), eigenVectorGroupLayout);
    layout->addRow(glyphResolutionLabel, glyphResolutionLayout);
    layout->addRow(scaleLabel, scaleLayout);
    layout->addRow(hideShowAxesLabel, slicesLayout);

    // connect all the signals
    connect(d->glyphShapeComboBox,              SIGNAL(currentIndexChanged(const QString&)), this, SIGNAL(glyphShapeChanged(const QString&)));
    connect(d->sampleRateSlider,              SIGNAL(valueChanged(int)),                   this, SIGNAL(sampleRateChanged(int)));
    connect(d->glyphResolutionSlider,         SIGNAL(valueChanged(int)),                   this, SIGNAL(glyphResolutionChanged(int)));

    // some signals (checkboxes) require one more step to translate from Qt::CheckState to bool
    connect(d->flipXCheckBox,                   SIGNAL(stateChanged(int)),                   this, SLOT(onFlipXCheckBoxStateChanged(int)));
    connect(d->flipYCheckBox,                   SIGNAL(stateChanged(int)),                   this, SLOT(onFlipYCheckBoxStateChanged(int)));
    connect(d->flipZCheckBox,                   SIGNAL(stateChanged(int)),                   this, SLOT(onFlipZCheckBoxStateChanged(int)));
    //connect(d->reverseBackgroundColorCheckBox,  SIGNAL(stateChanged(int)),                   this, SLOT(onReverseBackgroundColorChanged(int)));
    connect(d->hideShowAxialCheckBox,           SIGNAL(stateChanged(int)),                   this, SLOT(onHideShowAxialChanged(int)));
    connect(d->hideShowCoronalCheckBox,         SIGNAL(stateChanged(int)),                   this, SLOT(onHideShowCoronalChanged(int)));
    connect(d->hideShowSagittalCheckBox,        SIGNAL(stateChanged(int)),                   this, SLOT(onHideShowSagittalChanged(int)));

    // we also need to translate radio buttons boolean states to an eigen vector
    connect(d->eigenVectorV1RadioButton,   SIGNAL(toggled(bool)),                       this, SLOT(onEigenVectorV1Toggled(bool)));
    connect(d->eigenVectorV2RadioButton,   SIGNAL(toggled(bool)),                       this, SLOT(onEigenVectorV2Toggled(bool)));
    connect(d->eigenVectorV3RadioButton,   SIGNAL(toggled(bool)),                       this, SLOT(onEigenVectorV3Toggled(bool)));

    // we need to calculate one single number for the scale, out of the minor and major scales
    connect(d->scaleBase,            SIGNAL(valueChanged(int)),                   this, SLOT(onMinorScalingChanged(int)));
    connect(d->scaleExp,            SIGNAL(valueChanged(int)),                   this, SLOT(onMajorScalingChanged(int)));

    this->setTitle("Tensor View");
    this->addWidget(displayWidget);
}

medTensorViewToolBox::~medTensorViewToolBox()
{
    delete d;
    d = NULL;
}

QString medTensorViewToolBox::glyphShape(void)
{
    return d->glyphShapeComboBox->currentText();
}

int medTensorViewToolBox::sampleRate(void)
{
    return d->sampleRateSlider->value();
}

bool medTensorViewToolBox::isFlipX(void)
{
    return d->flipXCheckBox->checkState() == Qt::Checked;
}

bool medTensorViewToolBox::isFlipY(void)
{
    return d->flipYCheckBox->checkState() == Qt::Checked;
}

bool medTensorViewToolBox::isFlipZ(void)
{
    return d->flipZCheckBox->checkState() == Qt::Checked;
}

int medTensorViewToolBox::eigenVector(void)
{
    if (d->eigenVectorV1RadioButton->isChecked())
    {
        return 3;
    }
    else if (d->eigenVectorV2RadioButton->isChecked())
    {
        return 2;
    }
    else if (d->eigenVectorV3RadioButton->isChecked())
    {
        return 1;
    }
    qWarning() << "medTollBoxDiffusionTensorView::eigenVector() returning wrong value";
    return 1;
}

int medTensorViewToolBox::glyphResolution(void)
{
    return d->glyphResolutionSlider->value();
}

double medTensorViewToolBox::scale(void)
{
    int minorScale = d->scaleBase->value();
    int majorScaleExponent = d->scaleExp->value();
    double majorScale = pow(10.0, majorScaleExponent);
    double scale = majorScale * minorScale;
    return scale;
}

bool medTensorViewToolBox::isShowAxial(void)
{
    return d->hideShowAxialCheckBox->checkState() == Qt::Checked;
}

bool medTensorViewToolBox::isShowCoronal(void)
{
    return d->hideShowCoronalCheckBox->checkState() == Qt::Checked;
}

bool medTensorViewToolBox::isShowSagittal(void)
{
    return d->hideShowSagittalCheckBox->checkState() == Qt::Checked;
}

void medTensorViewToolBox::onFlipXCheckBoxStateChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit flipX(false);
    else if (checkBoxState == Qt::Checked)
        emit flipX(true);
}

void medTensorViewToolBox::onFlipYCheckBoxStateChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit flipY(false);
    else if (checkBoxState == Qt::Checked)
        emit flipY(true);
}

void medTensorViewToolBox::onFlipZCheckBoxStateChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit flipZ(false);
    else if (checkBoxState == Qt::Checked)
        emit flipZ(true);
}

void medTensorViewToolBox::onReverseBackgroundColorChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit reverseBackgroundColor(false);
    else if (checkBoxState == Qt::Checked)
        emit reverseBackgroundColor(true);
}

void medTensorViewToolBox::onEigenVectorV1Toggled(bool isSelected)
{
    if (isSelected)
        emit eigenVectorChanged(3);
}

void medTensorViewToolBox::onEigenVectorV2Toggled(bool isSelected)
{
    if (isSelected)
        emit eigenVectorChanged(2);
}

void medTensorViewToolBox::onEigenVectorV3Toggled(bool isSelected)
{
    if (isSelected)
        emit eigenVectorChanged(1);
}

void medTensorViewToolBox::onMinorScalingChanged(int minorScale)
{
    int majorScaleExponent = d->scaleExp->value();
    double majorScale = pow(10.0, majorScaleExponent);
    double scale = majorScale * minorScale;
    emit scalingChanged(scale);
}

void medTensorViewToolBox::onMajorScalingChanged(int majorScaleExponent)
{
    int minorScale = d->scaleBase->value();
    double majorScale = pow(10.0, majorScaleExponent);
    double scale = majorScale * minorScale;
    emit scalingChanged(scale);
}

void medTensorViewToolBox::onHideShowAxialChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit hideShowAxial(false);
    else if (checkBoxState == Qt::Checked)
        emit hideShowAxial(true);
}

void medTensorViewToolBox::onHideShowCoronalChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit hideShowCoronal(false);
    else if (checkBoxState == Qt::Checked)
        emit hideShowCoronal(true);
}

void medTensorViewToolBox::onHideShowSagittalChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit hideShowSagittal(false);
    else if (checkBoxState == Qt::Checked)
        emit hideShowSagittal(true);
}

void medTensorViewToolBox::update (dtkAbstractView *view)
{
    if (!view)
        return;

    // the tensor view toolbox is expected to control all tensors
    // i.e. is general to all tensors, hence we do not update its values
    // for every view

//    //dtkAbstractViewInteractor* interactor = view->interactor("Tensor");
//    dtkAbstractViewInteractor* interactor = view->interactor("v3dViewTensorInteractor");
//
//    if(interactor)
//    {
//        QString glyphShape = interactor->property("GlyphShape");
//
//        int index = d->glyphShapes.indexOf(glyphShape);
//
//        d->glyphShape->blockSignals(true);
//        d->glyphShape->setCurrentIndex(index);
//        d->glyphShape->blockSignals(false);
//    }
}
