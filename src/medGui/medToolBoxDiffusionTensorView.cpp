#include "medToolBoxDiffusionTensorView.h"
#include <dtkCore/dtkAbstractViewInteractor.h>
#include <dtkCore/dtkAbstractView.h>
#include <math.h>

class medToolBoxDiffusionTensorViewPrivate
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
    QSlider*      minorScalingSlider;
    QSlider*      majorScalingSlider;
    QCheckBox*    hideShowAxialCheckBox;
    QCheckBox*    hideShowCoronalCheckBox;
    QCheckBox*    hideShowSagittalCheckBox;

    QStringList glyphShapesList;
};

medToolBoxDiffusionTensorView::medToolBoxDiffusionTensorView(QWidget *parent) : medToolBox(parent), d(new medToolBoxDiffusionTensorViewPrivate)
{
    QWidget* displayWidget = new QWidget(this);

    d->glyphShapesList = *(new QStringList());
    d->glyphShapesList << "Lines" << "Disks" << "Arrows" << "Cubes" << "Cylinders" << "Ellipsoids" << "Superquadrics";

    // combobox to control the glyph shape
    d->glyphShapeComboBox = new QComboBox(displayWidget);
    d->glyphShapeComboBox->addItems(d->glyphShapesList);

    QHBoxLayout* glyphShapeLayout = new QHBoxLayout;
    glyphShapeLayout->addWidget(new QLabel("Shape: "));
    glyphShapeLayout->addWidget(d->glyphShapeComboBox);

    // slider to control sample rate
    d->sampleRateSlider =  new QSlider(Qt::Horizontal, displayWidget);
    d->sampleRateSlider->setMinimum(1);
    d->sampleRateSlider->setMaximum(10);
    d->sampleRateSlider->setSingleStep(1);
    d->sampleRateSlider->setValue(1);

    QSpinBox* sampleRateSpinBox = new QSpinBox(displayWidget);
    sampleRateSpinBox->setMinimum(1);
    sampleRateSpinBox->setMaximum(10);
    sampleRateSpinBox->setSingleStep(1);
    sampleRateSpinBox->setValue(1);

    QHBoxLayout* sampleRateLayout = new QHBoxLayout;
    sampleRateLayout->addWidget(new QLabel("Sample rate: "));
    sampleRateLayout->addWidget(d->sampleRateSlider);
    sampleRateLayout->addWidget(sampleRateSpinBox);

    connect(d->sampleRateSlider, SIGNAL(valueChanged(int)), sampleRateSpinBox, SLOT(setValue(int)));
    connect(sampleRateSpinBox, SIGNAL(valueChanged(int)), d->sampleRateSlider, SLOT(setValue(int)));

    // flipX, flipY and flipZ checkboxes
    d->flipXCheckBox = new QCheckBox("Flip X", displayWidget);
    d->flipYCheckBox = new QCheckBox("Flip Y", displayWidget);
    d->flipZCheckBox = new QCheckBox("Flip Z", displayWidget);

    QHBoxLayout* flipAxesLayout = new QHBoxLayout;
    flipAxesLayout->addWidget(d->flipXCheckBox);
    flipAxesLayout->addWidget(d->flipYCheckBox);
    flipAxesLayout->addWidget(d->flipZCheckBox);

    // eigen vectors
    d->eigenVectorV1RadioButton = new QRadioButton("v1", displayWidget);
    d->eigenVectorV2RadioButton = new QRadioButton("v2", displayWidget);
    d->eigenVectorV3RadioButton = new QRadioButton("v3", displayWidget);

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

    QVBoxLayout* eigenLayout = new QVBoxLayout;
    eigenLayout->addWidget(new QLabel("Eigen Vector for color-coding:"));
    eigenLayout->addLayout(eigenVectorGroupLayout);

    d->eigenVectorV1RadioButton->setChecked(true);

    // reverse background color
    d->reverseBackgroundColorCheckBox = new QCheckBox("Reverse Background Color", displayWidget);

    // slider to control glyph resolution
    d->glyphResolutionSlider =  new QSlider(Qt::Horizontal, displayWidget);
    d->glyphResolutionSlider->setMinimum(2);
    d->glyphResolutionSlider->setMaximum(20);
    d->glyphResolutionSlider->setSingleStep(1);
    d->glyphResolutionSlider->setValue(6);

    QSpinBox* glyphResolutionSpinBox = new QSpinBox(displayWidget);
    glyphResolutionSpinBox->setMinimum(2);
    glyphResolutionSpinBox->setMaximum(20);
    glyphResolutionSpinBox->setSingleStep(1);
    glyphResolutionSpinBox->setValue(6);

    QHBoxLayout* glyphResolutionLayout = new QHBoxLayout;
    glyphResolutionLayout->addWidget(new QLabel("Glyph resolution: "));
    glyphResolutionLayout->addWidget(d->glyphResolutionSlider);
    glyphResolutionLayout->addWidget(glyphResolutionSpinBox);

    connect(d->glyphResolutionSlider, SIGNAL(valueChanged(int)), glyphResolutionSpinBox, SLOT(setValue(int)));
    connect(glyphResolutionSpinBox, SIGNAL(valueChanged(int)), d->glyphResolutionSlider, SLOT(setValue(int)));

    // minor scaling
    d->minorScalingSlider =  new QSlider(Qt::Horizontal, displayWidget);
    d->minorScalingSlider->setMinimum(1);
    d->minorScalingSlider->setMaximum(9);
    d->minorScalingSlider->setSingleStep(1);
    d->minorScalingSlider->setValue(1);

    QSpinBox* minorScalingSpinBox = new QSpinBox(displayWidget);
    minorScalingSpinBox->setMinimum(1);
    minorScalingSpinBox->setMaximum(9);
    minorScalingSpinBox->setSingleStep(1);
    minorScalingSpinBox->setValue(1);

    QHBoxLayout* minorScalingLayout = new QHBoxLayout;
    minorScalingLayout->addWidget(new QLabel("Minor scaling: "));
    minorScalingLayout->addWidget(d->minorScalingSlider);
    minorScalingLayout->addWidget(minorScalingSpinBox);

    connect(d->minorScalingSlider, SIGNAL(valueChanged(int)), minorScalingSpinBox, SLOT(setValue(int)));
    connect(minorScalingSpinBox, SIGNAL(valueChanged(int)), d->minorScalingSlider, SLOT(setValue(int)));

    // major scaling
    d->majorScalingSlider =  new QSlider(Qt::Horizontal, displayWidget);
    d->majorScalingSlider->setMinimum(-10);
    d->majorScalingSlider->setMaximum(10);
    d->majorScalingSlider->setSingleStep(1);
    d->majorScalingSlider->setValue(0);

    QSpinBox* majorScalingSpinBox = new QSpinBox(displayWidget);
    majorScalingSpinBox->setMinimum(-10);
    majorScalingSpinBox->setMaximum(10);
    majorScalingSpinBox->setSingleStep(1);
    majorScalingSpinBox->setValue(0);

    QHBoxLayout* majorScalingLayout = new QHBoxLayout;
    majorScalingLayout->addWidget(new QLabel("Major scaling (10^n): "));
    majorScalingLayout->addWidget(d->majorScalingSlider);
    majorScalingLayout->addWidget(majorScalingSpinBox);

    connect(d->majorScalingSlider, SIGNAL(valueChanged(int)), majorScalingSpinBox, SLOT(setValue(int)));
    connect(majorScalingSpinBox, SIGNAL(valueChanged(int)), d->majorScalingSlider, SLOT(setValue(int)));

    // hide or show axial, coronal, and sagittal
    d->hideShowAxialCheckBox = new QCheckBox("Axial", displayWidget);
    d->hideShowCoronalCheckBox = new QCheckBox("Coronal", displayWidget);
    d->hideShowSagittalCheckBox = new QCheckBox("Sagittal", displayWidget);

    d->hideShowAxialCheckBox->setChecked(true);
    d->hideShowCoronalCheckBox->setChecked(true);
    d->hideShowSagittalCheckBox->setChecked(true);

    QHBoxLayout* slicesLayout = new QHBoxLayout;
    slicesLayout->addWidget(d->hideShowAxialCheckBox);
    slicesLayout->addWidget(d->hideShowCoronalCheckBox);
    slicesLayout->addWidget(d->hideShowSagittalCheckBox);

    // layout all the controls in the toolbox
    QVBoxLayout* layout = new QVBoxLayout(displayWidget);
    layout->addLayout(glyphShapeLayout);
    layout->addLayout(sampleRateLayout);
    //layout->addWidget(new QLabel("Flip axes:"));
    layout->addLayout(flipAxesLayout);
    layout->addLayout(eigenLayout);
    layout->addWidget(d->reverseBackgroundColorCheckBox);
    layout->addLayout(glyphResolutionLayout);
    layout->addLayout(minorScalingLayout);
    layout->addLayout(majorScalingLayout);
    layout->addWidget(new QLabel("Hide or show slices:"));
    layout->addLayout(slicesLayout);


    // connect all the signals
    connect(d->glyphShapeComboBox,              SIGNAL(currentIndexChanged(const QString&)), this, SIGNAL(glyphShapeChanged(const QString&)));
    connect(d->sampleRateSlider,              SIGNAL(valueChanged(int)),                   this, SIGNAL(sampleRateChanged(int)));
    connect(d->glyphResolutionSlider,         SIGNAL(valueChanged(int)),                   this, SIGNAL(glyphResolutionChanged(int)));

    // some signals (checkboxes) require one more step to translate from Qt::CheckState to bool
    connect(d->flipXCheckBox,                   SIGNAL(stateChanged(int)),                   this, SLOT(onFlipXCheckBoxStateChanged(int)));
    connect(d->flipYCheckBox,                   SIGNAL(stateChanged(int)),                   this, SLOT(onFlipYCheckBoxStateChanged(int)));
    connect(d->flipZCheckBox,                   SIGNAL(stateChanged(int)),                   this, SLOT(onFlipZCheckBoxStateChanged(int)));
    connect(d->reverseBackgroundColorCheckBox,  SIGNAL(stateChanged(int)),                   this, SLOT(onReverseBackgroundColorChanged(int)));
    connect(d->hideShowAxialCheckBox,           SIGNAL(stateChanged(int)),                   this, SLOT(onHideShowAxialChanged(int)));
    connect(d->hideShowCoronalCheckBox,         SIGNAL(stateChanged(int)),                   this, SLOT(onHideShowCoronalChanged(int)));
    connect(d->hideShowSagittalCheckBox,        SIGNAL(stateChanged(int)),                   this, SLOT(onHideShowSagittalChanged(int)));

    // we also need to translate radio buttons boolean states to an eigen vector
    connect(d->eigenVectorV1RadioButton,   SIGNAL(toggled(bool)),                       this, SLOT(onEigenVectorV1Toggled(bool)));
    connect(d->eigenVectorV2RadioButton,   SIGNAL(toggled(bool)),                       this, SLOT(onEigenVectorV2Toggled(bool)));
    connect(d->eigenVectorV3RadioButton,   SIGNAL(toggled(bool)),                       this, SLOT(onEigenVectorV3Toggled(bool)));

    // we need to calculate one single number for the scale, out of the minor and major scales
    connect(d->minorScalingSlider,            SIGNAL(valueChanged(int)),                   this, SLOT(onMinorScalingChanged(int)));
    connect(d->majorScalingSlider,            SIGNAL(valueChanged(int)),                   this, SLOT(onMajorScalingChanged(int)));

    this->setTitle("Tensor View");
    this->addWidget(displayWidget);
}

medToolBoxDiffusionTensorView::~medToolBoxDiffusionTensorView()
{
    delete d;
    d = NULL;
}

QString medToolBoxDiffusionTensorView::glyphShape(void)
{
    return d->glyphShapeComboBox->currentText();
}

int medToolBoxDiffusionTensorView::sampleRate(void)
{
    return d->sampleRateSlider->value();
}

bool medToolBoxDiffusionTensorView::isFlipX(void)
{
    return d->flipXCheckBox->checkState() == Qt::Checked;
}

bool medToolBoxDiffusionTensorView::isFlipY(void)
{
    return d->flipYCheckBox->checkState() == Qt::Checked;
}

bool medToolBoxDiffusionTensorView::isFlipZ(void)
{
    return d->flipZCheckBox->checkState() == Qt::Checked;
}

int medToolBoxDiffusionTensorView::eigenVector(void)
{
    if (d->eigenVectorV1RadioButton->isChecked())
    {
        return 1;
    }
    else if (d->eigenVectorV2RadioButton->isChecked())
    {
        return 2;
    }
    else if (d->eigenVectorV3RadioButton->isChecked())
    {
        return 3;
    }
}

int medToolBoxDiffusionTensorView::glyphResolution(void)
{
    return d->glyphResolutionSlider->value();
}

double medToolBoxDiffusionTensorView::scale(void)
{
    int minorScale = d->minorScalingSlider->value();
    int majorScaleExponent = d->majorScalingSlider->value();
    double majorScale = pow(10.0, majorScaleExponent);
    double scale = majorScale * minorScale;
    return scale;
}

bool medToolBoxDiffusionTensorView::isShowAxial(void)
{
    return d->hideShowAxialCheckBox->checkState() == Qt::Checked;
}

bool medToolBoxDiffusionTensorView::isShowCoronal(void)
{
    return d->hideShowCoronalCheckBox->checkState() == Qt::Checked;
}

bool medToolBoxDiffusionTensorView::isShowSagittal(void)
{
    return d->hideShowSagittalCheckBox->checkState() == Qt::Checked;
}

void medToolBoxDiffusionTensorView::onFlipXCheckBoxStateChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit flipX(false);
    else if (checkBoxState == Qt::Checked)
        emit flipX(true);
}

void medToolBoxDiffusionTensorView::onFlipYCheckBoxStateChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit flipY(false);
    else if (checkBoxState == Qt::Checked)
        emit flipY(true);
}

void medToolBoxDiffusionTensorView::onFlipZCheckBoxStateChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit flipZ(false);
    else if (checkBoxState == Qt::Checked)
        emit flipZ(true);
}

void medToolBoxDiffusionTensorView::onReverseBackgroundColorChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit reverseBackgroundColor(false);
    else if (checkBoxState == Qt::Checked)
        emit reverseBackgroundColor(true);
}

void medToolBoxDiffusionTensorView::onEigenVectorV1Toggled(bool isSelected)
{
    if (isSelected)
        emit eigenVectorChanged(1);
}

void medToolBoxDiffusionTensorView::onEigenVectorV2Toggled(bool isSelected)
{
    if (isSelected)
        emit eigenVectorChanged(2);
}

void medToolBoxDiffusionTensorView::onEigenVectorV3Toggled(bool isSelected)
{
    if (isSelected)
        emit eigenVectorChanged(3);
}

void medToolBoxDiffusionTensorView::onMinorScalingChanged(int minorScale)
{
    int majorScaleExponent = d->majorScalingSlider->value();
    double majorScale = pow(10.0, majorScaleExponent);
    double scale = majorScale * minorScale;
    emit scalingChanged(scale);
}

void medToolBoxDiffusionTensorView::onMajorScalingChanged(int majorScaleExponent)
{
    int minorScale = d->minorScalingSlider->value();
    double majorScale = pow(10.0, majorScaleExponent);
    double scale = majorScale * minorScale;
    emit scalingChanged(scale);
}

void medToolBoxDiffusionTensorView::onHideShowAxialChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit hideShowAxial(false);
    else if (checkBoxState == Qt::Checked)
        emit hideShowAxial(true);
}

void medToolBoxDiffusionTensorView::onHideShowCoronalChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit hideShowCoronal(false);
    else if (checkBoxState == Qt::Checked)
        emit hideShowCoronal(true);
}

void medToolBoxDiffusionTensorView::onHideShowSagittalChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit hideShowSagittal(false);
    else if (checkBoxState == Qt::Checked)
        emit hideShowSagittal(true);
}

void medToolBoxDiffusionTensorView::update (dtkAbstractView *view)
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
