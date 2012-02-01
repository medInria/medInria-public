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
    QSpinBox*     scaleBase;
    QSpinBox*     scaleExp;
    QCheckBox*    hideShowAxialCheckBox;
    QCheckBox*    hideShowCoronalCheckBox;
    QCheckBox*    hideShowSagittalCheckBox;

    QStringList glyphShapesList;
};

medToolBoxDiffusionTensorView::medToolBoxDiffusionTensorView(QWidget *parent) : medToolBox(parent), d(new medToolBoxDiffusionTensorViewPrivate)
{
    QWidget* displayWidget = new QWidget(this);

    d->glyphShapesList = QStringList();
    d->glyphShapesList << "Lines" << "Disks" << "Arrows" << "Cubes" << "Cylinders" << "Ellipsoids" << "Superquadrics";

    // combobox to control the glyph shape
    d->glyphShapeComboBox = new QComboBox(displayWidget);
    d->glyphShapeComboBox->addItems(d->glyphShapesList);

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
    sampleRateLayout->addWidget(d->sampleRateSlider);
    sampleRateLayout->addWidget(sampleRateSpinBox);

    connect(d->sampleRateSlider, SIGNAL(valueChanged(int)), sampleRateSpinBox, SLOT(setValue(int)));
    connect(sampleRateSpinBox, SIGNAL(valueChanged(int)), d->sampleRateSlider, SLOT(setValue(int)));

    // flipX, flipY and flipZ checkboxes
    d->flipXCheckBox = new QCheckBox("X", displayWidget);
    d->flipYCheckBox = new QCheckBox("Y", displayWidget);
    d->flipZCheckBox = new QCheckBox("Z", displayWidget);

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

    d->eigenVectorV1RadioButton->setChecked(true);

    // reverse background color
    //d->reverseBackgroundColorCheckBox = new QCheckBox("Reverse Background Color", displayWidget);

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
    glyphResolutionLayout->addWidget(d->glyphResolutionSlider);
    glyphResolutionLayout->addWidget(glyphResolutionSpinBox);

    connect(d->glyphResolutionSlider, SIGNAL(valueChanged(int)), glyphResolutionSpinBox, SLOT(setValue(int)));
    connect(glyphResolutionSpinBox, SIGNAL(valueChanged(int)), d->glyphResolutionSlider, SLOT(setValue(int)));

    // scale = scaleBase * 10 ^ scaleExp

    d->scaleBase = new QSpinBox(displayWidget);
    d->scaleBase->setMinimum(1);
    d->scaleBase->setMaximum(9);
    d->scaleBase->setSingleStep(1);
    d->scaleBase->setValue(1);

    d->scaleExp = new QSpinBox(displayWidget);
    d->scaleExp->setMinimum(-10);
    d->scaleExp->setMaximum(10);
    d->scaleExp->setSingleStep(1);
    d->scaleExp->setValue(0);

    QHBoxLayout* scaleLayout = new QHBoxLayout;
    scaleLayout->addWidget(d->scaleBase);
    scaleLayout->addWidget(new QLabel(" x 10 ^ ", displayWidget));
    scaleLayout->addWidget(d->scaleExp);

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
    QFormLayout *layout = new QFormLayout(displayWidget);
    layout->addRow(tr("Shape:"), d->glyphShapeComboBox);
    layout->addRow(tr("Sample rate:"), sampleRateLayout);
    layout->addRow(tr("Flip:"), flipAxesLayout);
    layout->addRow(tr("Color with:"), eigenVectorGroupLayout);
    layout->addRow(tr("Resolution:"), glyphResolutionLayout);
    layout->addRow(tr("Scale:"), scaleLayout);
    layout->addRow(tr("Hide/show:"), slicesLayout);

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

int medToolBoxDiffusionTensorView::glyphResolution(void)
{
    return d->glyphResolutionSlider->value();
}

double medToolBoxDiffusionTensorView::scale(void)
{
    int minorScale = d->scaleBase->value();
    int majorScaleExponent = d->scaleExp->value();
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
        emit eigenVectorChanged(3);
}

void medToolBoxDiffusionTensorView::onEigenVectorV2Toggled(bool isSelected)
{
    if (isSelected)
        emit eigenVectorChanged(2);
}

void medToolBoxDiffusionTensorView::onEigenVectorV3Toggled(bool isSelected)
{
    if (isSelected)
        emit eigenVectorChanged(1);
}

void medToolBoxDiffusionTensorView::onMinorScalingChanged(int minorScale)
{
    int majorScaleExponent = d->scaleExp->value();
    double majorScale = pow(10.0, majorScaleExponent);
    double scale = majorScale * minorScale;
    emit scalingChanged(scale);
}

void medToolBoxDiffusionTensorView::onMajorScalingChanged(int majorScaleExponent)
{
    int minorScale = d->scaleBase->value();
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
