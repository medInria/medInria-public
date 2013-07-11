#include "medSHViewToolBox.h"
#include <medCore/medSHAbstractViewInteractor.h>
#include <dtkCore/dtkAbstractView.h>
#include <math.h>

class medSHViewToolBoxPrivate {
public:
    QComboBox*    tesselationTypeComboBox;
    QComboBox*    tesselationBasisComboBox;

    QSlider*      sampleRateSlider;
    QCheckBox*    flipXCheckBox;
    QCheckBox*    flipYCheckBox;
    QCheckBox*    flipZCheckBox;
    QCheckBox*    NormalizeCheckBox;
//    QRadioButton* eigenVectorV1RadioButton;
//    QRadioButton* eigenVectorV2RadioButton;
//    QRadioButton* eigenVectorV3RadioButton;
    QCheckBox*    reverseBackgroundColorCheckBox;
    QSlider*      glyphResolutionSlider;
    QSlider*      minorScalingSlider;
    QSlider*      majorScalingSlider;

    QSlider*      SliceSlider[3];
    QSpinBox*     SliceSpinBox[3];

    QCheckBox*    hideShowAxialCheckBox;
    QCheckBox*    hideShowCoronalCheckBox;
    QCheckBox*    hideShowSagittalCheckBox;

    QStringList   tesselationTypeList;
    QStringList   tesselationBasisList;
};

medSHViewToolBox::medSHViewToolBox(QWidget *parent): medToolBox(parent),d(new medSHViewToolBoxPrivate) {

    QWidget* displayWidget = new QWidget(this);

    d->tesselationTypeList << "Icosahedron" << "Octahedron" << "Tetrahedron";
    d->tesselationBasisList << "SHMatrix" << "SHMatrixMaxThesis" << "SHMatrixTournier" << "SHMatrixRshBasis";

    // combobox to control the glyph shape

    d->tesselationTypeComboBox = new QComboBox(displayWidget);
    d->tesselationTypeComboBox->addItems(d->tesselationTypeList);

    // combobox to control the spherical Harmonics basis

    d->tesselationBasisComboBox = new QComboBox(displayWidget);
    d->tesselationBasisComboBox->addItems(d->tesselationBasisList);

    // slider to control sample rate

    d->sampleRateSlider =  new QSlider(Qt::Horizontal, displayWidget);
    d->sampleRateSlider->setMinimum(1);
    d->sampleRateSlider->setMaximum(10);
    d->sampleRateSlider->setSingleStep(1);
    d->sampleRateSlider->setValue(1);
    d->sampleRateSlider->setTracking(false);

    QSpinBox* sampleRateSpinBox = new QSpinBox(displayWidget);
    sampleRateSpinBox->setMinimum(1);
    sampleRateSpinBox->setMaximum(10);
    sampleRateSpinBox->setSingleStep(1);
    sampleRateSpinBox->setValue(1);

    QHBoxLayout* sampleRateLayout = new QHBoxLayout;
    sampleRateLayout->addWidget(d->sampleRateSlider);
    sampleRateLayout->addWidget(sampleRateSpinBox);

    connect(d->sampleRateSlider,SIGNAL(valueChanged(int)),sampleRateSpinBox,  SLOT(setValue(int)));
    connect(sampleRateSpinBox,  SIGNAL(valueChanged(int)),d->sampleRateSlider,SLOT(setValue(int)));

    // slider to control x SLICE

    QHBoxLayout* SliceLayout[3];
    for (unsigned i=0;i<3;++i) {
        d->SliceSlider[i] = new QSlider(Qt::Horizontal,displayWidget);
        d->SliceSlider[i]->setMinimum(0);
        d->SliceSlider[i]->setMaximum(100);
        d->SliceSlider[i]->setSingleStep(1);
        d->SliceSlider[i]->setValue(0);
        d->SliceSlider[i]->setTracking(false);

        d->SliceSpinBox[i] = new QSpinBox(displayWidget);
        d->SliceSpinBox[i]->setMinimum(0);
        d->SliceSpinBox[i]->setMaximum(100);
        d->SliceSpinBox[i]->setSingleStep(1);
        d->SliceSpinBox[i]->setValue(0);

        SliceLayout[i] = new QHBoxLayout;
        SliceLayout[i]->addWidget(d->SliceSlider[i]);
        SliceLayout[i]->addWidget(d->SliceSpinBox[i]);

        connect(d->SliceSlider[i], SIGNAL(valueChanged(int)),d->SliceSpinBox[i],SLOT(setValue(int)));
        connect(d->SliceSpinBox[i],SIGNAL(valueChanged(int)),d->SliceSlider[i], SLOT(setValue(int)));
    }

    // flipX, flipY and flipZ checkboxes
    d->flipXCheckBox = new QCheckBox("X", displayWidget);
    d->flipYCheckBox = new QCheckBox("Y", displayWidget);
    d->flipZCheckBox = new QCheckBox("Z", displayWidget);
    d->flipZCheckBox->setCheckState(Qt::Checked);
    d->NormalizeCheckBox = new QCheckBox("Normalize", displayWidget);


    QHBoxLayout* flipAxesLayout = new QHBoxLayout;
    flipAxesLayout->addWidget(d->flipXCheckBox);
    flipAxesLayout->addWidget(d->flipYCheckBox);
    flipAxesLayout->addWidget(d->flipZCheckBox);
    flipAxesLayout->addWidget(d->NormalizeCheckBox);

//    // eigen vectors
//    d->eigenVectorV1RadioButton = new QRadioButton("v1", displayWidget);
//    d->eigenVectorV2RadioButton = new QRadioButton("v2", displayWidget);
//    d->eigenVectorV3RadioButton = new QRadioButton("v3", displayWidget);

//    QButtonGroup *eigenVectorRadioGroup = new QButtonGroup(displayWidget);
//    eigenVectorRadioGroup->addButton(d->eigenVectorV1RadioButton);
//    eigenVectorRadioGroup->addButton(d->eigenVectorV2RadioButton);
//    eigenVectorRadioGroup->addButton(d->eigenVectorV3RadioButton);
//    eigenVectorRadioGroup->setExclusive(true);

//    QHBoxLayout *eigenVectorGroupLayout = new QHBoxLayout;
//    eigenVectorGroupLayout->addWidget(d->eigenVectorV1RadioButton);
//    eigenVectorGroupLayout->addWidget(d->eigenVectorV2RadioButton);
//    eigenVectorGroupLayout->addWidget(d->eigenVectorV3RadioButton);
//    //eigenVectorGroupLayout->addStretch(1);

//    d->eigenVectorV1RadioButton->setChecked(true);

    // reverse background color
    //d->reverseBackgroundColorCheckBox = new QCheckBox("Reverse Background Color", displayWidget);

    // slider to control glyph resolution
    d->glyphResolutionSlider =  new QSlider(Qt::Horizontal, displayWidget);
    d->glyphResolutionSlider->setMinimum(0);
    d->glyphResolutionSlider->setMaximum(10);
    d->glyphResolutionSlider->setSingleStep(1);
    d->glyphResolutionSlider->setValue(2);
    d->glyphResolutionSlider->setTracking(false);

    QSpinBox* glyphResolutionSpinBox = new QSpinBox(displayWidget);
    glyphResolutionSpinBox->setMinimum(0);
    glyphResolutionSpinBox->setMaximum(10);
    glyphResolutionSpinBox->setSingleStep(1);
    glyphResolutionSpinBox->setValue(2);

    QHBoxLayout* glyphResolutionLayout = new QHBoxLayout;
    glyphResolutionLayout->addWidget(d->glyphResolutionSlider);
    glyphResolutionLayout->addWidget(glyphResolutionSpinBox);

    connect(d->glyphResolutionSlider, SIGNAL(valueChanged(int)), glyphResolutionSpinBox, SLOT(setValue(int)));
    connect(glyphResolutionSpinBox, SIGNAL(valueChanged(int)), d->glyphResolutionSlider, SLOT(setValue(int)));

    // minor scaling
    d->minorScalingSlider =  new QSlider(Qt::Horizontal, displayWidget);
    d->minorScalingSlider->setMinimum(0);
    d->minorScalingSlider->setMaximum(9);
    d->minorScalingSlider->setSingleStep(1);
    d->minorScalingSlider->setValue(3);
    d->minorScalingSlider->setTracking(false);

    QSpinBox* minorScalingSpinBox = new QSpinBox(displayWidget);
    minorScalingSpinBox->setMinimum(0);
    minorScalingSpinBox->setMaximum(9);
    minorScalingSpinBox->setSingleStep(1);
    minorScalingSpinBox->setValue(3);

    QHBoxLayout* minorScalingLayout = new QHBoxLayout;
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
    d->majorScalingSlider->setTracking(false);

    QSpinBox* majorScalingSpinBox = new QSpinBox(displayWidget);
    majorScalingSpinBox->setMinimum(-10);
    majorScalingSpinBox->setMaximum(10);
    majorScalingSpinBox->setSingleStep(1);
    majorScalingSpinBox->setValue(0);

    QHBoxLayout* majorScalingLayout = new QHBoxLayout;
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
    QFormLayout *layout = new QFormLayout(displayWidget);
    layout->addRow(tr("Tesselation:"), d->tesselationTypeComboBox);
    layout->addRow(tr("SH basis:"), d->tesselationBasisComboBox);

    layout->addRow(tr("Sample rate:"), sampleRateLayout);
    layout->addRow(tr("Flip:"), flipAxesLayout);
//    layout->addRow(tr("Color with:"), eigenVectorGroupLayout);
    layout->addRow(tr("Resolution:"), glyphResolutionLayout);
    layout->addRow(tr("Minor scaling:"), minorScalingLayout);
    layout->addRow(tr("Major scaling:"), majorScalingLayout);
    layout->addRow(tr("Hide/show:"), slicesLayout);
    layout->addRow(tr("Axial Slice:"),   SliceLayout[2]);
    layout->addRow(tr("Coronal Slice:"), SliceLayout[1]);
    layout->addRow(tr("Sagital Slice:"), SliceLayout[0]);

    // connect all the signals
    connect(d->tesselationTypeComboBox,              SIGNAL(currentIndexChanged(const QString&)), this, SIGNAL(tesselationTypeChanged(const QString&)));
    connect(d->tesselationBasisComboBox,              SIGNAL(currentIndexChanged(const QString&)), this, SIGNAL(tesselationBasisChanged(const QString&)));

    connect(d->sampleRateSlider,              SIGNAL(valueChanged(int)),                   this, SIGNAL(sampleRateChanged(int)));
    connect(d->glyphResolutionSlider,         SIGNAL(valueChanged(int)),                   this, SIGNAL(glyphResolutionChanged(int)));

    connect(d->SliceSlider[0],SIGNAL(valueChanged(int)),this,SIGNAL(xSliceChanged(int)));
    connect(d->SliceSlider[1],SIGNAL(valueChanged(int)),this,SIGNAL(ySliceChanged(int)));
    connect(d->SliceSlider[2],SIGNAL(valueChanged(int)),this,SIGNAL(zSliceChanged(int)));

    // some signals (checkboxes) require one more step to translate from Qt::CheckState to bool
    connect(d->flipXCheckBox,                   SIGNAL(stateChanged(int)),                   this, SLOT(onFlipXCheckBoxStateChanged(int)));
    connect(d->flipYCheckBox,                   SIGNAL(stateChanged(int)),                   this, SLOT(onFlipYCheckBoxStateChanged(int)));
    connect(d->flipZCheckBox,                   SIGNAL(stateChanged(int)),                   this, SLOT(onFlipZCheckBoxStateChanged(int)));

    connect(d->NormalizeCheckBox,                   SIGNAL(stateChanged(int)),                   this, SLOT(onNormalizeCheckBoxStateChanged(int)));

    //connect(d->reverseBackgroundColorCheckBox,  SIGNAL(stateChanged(int)),                   this, SLOT(onReverseBackgroundColorChanged(int)));
    connect(d->hideShowAxialCheckBox,           SIGNAL(stateChanged(int)),                   this, SLOT(onHideShowAxialChanged(int)));
    connect(d->hideShowCoronalCheckBox,         SIGNAL(stateChanged(int)),                   this, SLOT(onHideShowCoronalChanged(int)));
    connect(d->hideShowSagittalCheckBox,        SIGNAL(stateChanged(int)),                   this, SLOT(onHideShowSagittalChanged(int)));

//    // we also need to translate radio buttons boolean states to an eigen vector
//    connect(d->eigenVectorV1RadioButton,   SIGNAL(toggled(bool)),                       this, SLOT(onEigenVectorV1Toggled(bool)));
//    connect(d->eigenVectorV2RadioButton,   SIGNAL(toggled(bool)),                       this, SLOT(onEigenVectorV2Toggled(bool)));
//    connect(d->eigenVectorV3RadioButton,   SIGNAL(toggled(bool)),                       this, SLOT(onEigenVectorV3Toggled(bool)));

    // we need to calculate one single number for the scale, out of the minor and major scales
    connect(d->minorScalingSlider,            SIGNAL(valueChanged(int)),                   this, SLOT(onMinorScalingChanged(int)));
    connect(d->majorScalingSlider,            SIGNAL(valueChanged(int)),                   this, SLOT(onMajorScalingChanged(int)));


    this->setTitle("Spherical Harmonics View");
    this->addWidget(displayWidget);
}

medSHViewToolBox::~medSHViewToolBox()
{
    delete d;
    d = NULL;
}

QString medSHViewToolBox::tesselationType(void)
{
    return d->tesselationTypeComboBox->currentText();
}

QString medSHViewToolBox::tesselationBasis(void)
{
    return d->tesselationBasisComboBox->currentText();
}

int medSHViewToolBox::sampleRate(void)
{
    return d->sampleRateSlider->value();
}

bool medSHViewToolBox::isFlipX(void)
{
    return d->flipXCheckBox->checkState() == Qt::Checked;
}

bool medSHViewToolBox::isFlipY(void)
{
    return d->flipYCheckBox->checkState() == Qt::Checked;
}

bool medSHViewToolBox::isFlipZ(void)
{
    return d->flipZCheckBox->checkState() == Qt::Checked;
}

bool medSHViewToolBox::isMaxThesisFunc(void)
{
    return d->NormalizeCheckBox->checkState() == Qt::Checked;
}

//int medSHViewToolBox::eigenVector(void)
//{
//    if (d->eigenVectorV1RadioButton->isChecked())
//    {
//        return 3;
//    }
//    else if (d->eigenVectorV2RadioButton->isChecked())
//    {
//        return 2;
//    }
//    else if (d->eigenVectorV3RadioButton->isChecked())
//    {
//        return 1;
//    }
//    qWarning() << "medTollBoxDiffusionSHView::eigenVector() returning wrong value";
//    return 1;
//}

int medSHViewToolBox::glyphResolution(void)
{
    return d->glyphResolutionSlider->value();
}

// void medSHViewToolBox::setGlyphResolution(const int value)
// {
//     d->glyphResolutionSlider->blockSignals(true);
//     d->glyphResolutionSlider->setValue(value);
//     d->glyphResolutionSlider->blockSignals(false);
// }

double medSHViewToolBox::scale(void)
{
    int minorScale = d->minorScalingSlider->value();
    int majorScaleExponent = d->majorScalingSlider->value();
    double majorScale = pow(10.0, majorScaleExponent);
    double scale = majorScale * minorScale;
    return scale;
}

bool medSHViewToolBox::isShowAxial(void)
{
    return d->hideShowAxialCheckBox->checkState() == Qt::Checked;
}

bool medSHViewToolBox::isShowCoronal(void)
{
    return d->hideShowCoronalCheckBox->checkState() == Qt::Checked;
}

bool medSHViewToolBox::isShowSagittal(void)
{
    return d->hideShowSagittalCheckBox->checkState() == Qt::Checked;
}

void medSHViewToolBox::onFlipXCheckBoxStateChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit flipX(false);
    else if (checkBoxState == Qt::Checked)
        emit flipX(true);
}

void medSHViewToolBox::onFlipYCheckBoxStateChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit flipY(false);
    else if (checkBoxState == Qt::Checked)
        emit flipY(true);
}

void medSHViewToolBox::onFlipZCheckBoxStateChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit flipZ(false);
    else if (checkBoxState == Qt::Checked)
        emit flipZ(true);
}

void medSHViewToolBox::onNormalizeCheckBoxStateChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit normalize(false);
    else if (checkBoxState == Qt::Checked)
        emit normalize(true);
}

void medSHViewToolBox::onScaleChanged(const double mantissa,const int exponent) {
    const double scale_factor = pow(10.0,exponent);
    onScaleChanged(mantissa*scale_factor);
}

void medSHViewToolBox::onScaleChanged(const double scale) {
    emit scalingChanged(scale);
}

void medSHViewToolBox::onMinorScalingChanged(const double minorScale) {
    const int majorScaleExponent = d->majorScalingSlider->value();
    onScaleChanged(minorScale,majorScaleExponent);
}

void medSHViewToolBox::onMajorScalingChanged(const int majorScaleExponent) {
    const double minorScale = d->minorScalingSlider->value();
    onScaleChanged(minorScale,majorScaleExponent);
}

void medSHViewToolBox::onHideShowAxialChanged(const int checkBoxState) {
    if (checkBoxState == Qt::Unchecked)
        emit hideShowAxial(false);
    else if (checkBoxState == Qt::Checked)
        emit hideShowAxial(true);
}

void medSHViewToolBox::onHideShowCoronalChanged(const int checkBoxState) {
    if (checkBoxState == Qt::Unchecked)
        emit hideShowCoronal(false);
    else if (checkBoxState == Qt::Checked)
        emit hideShowCoronal(true);
}

void medSHViewToolBox::onHideShowSagittalChanged(const int checkBoxState) {
    if (checkBoxState == Qt::Unchecked)
        emit hideShowSagittal(false);
    else if (checkBoxState == Qt::Checked)
        emit hideShowSagittal(true);
}

void medSHViewToolBox::update(dtkAbstractView *view) {
    if (!view)
        return;

    // the SH view toolbox is expected to control all SHs
    // i.e. is general to all SHs, hence we do not update its values
    // for every view

//    //dtkAbstractViewInteractor* interactor = view->interactor("SH");
//    dtkAbstractViewInteractor* interactor = view->interactor("v3dViewSHInteractor");
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

void medSHViewToolBox::updateWithInteractor(dtkAbstractView *view) {

    if (!view)
        return;

    if (medSHAbstractViewInteractor* interactor = dynamic_cast<medSHAbstractViewInteractor*>(view->interactor ("v3dViewSHInteractor"))) {
        int imSize[3];
        interactor->imageSize(imSize);
        for (unsigned i=0;i<3;++i) {
            const double sz = imSize[i];
            d->SliceSlider[i]->setMaximum(imSize[i]-1);
            d->SliceSpinBox[i]->setMaximum(imSize[i]-1);
            d->SliceSlider[i]->setPageStep(ceil(sz/10));
            d->SliceSlider[i]->setValue(floor(sz/2));
        }
    }
}

