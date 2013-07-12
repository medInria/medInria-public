#include <cmath>
#include <medSHViewToolBox.h>
#include <medCore/medSHAbstractViewInteractor.h>
#include <dtkCore/dtkAbstractView.h>

class medSHViewToolBoxPrivate {
public:
    QComboBox*    tesselationTypeComboBox;
    QComboBox*    tesselationBasisComboBox;

    QSlider*      sampleRateSlider;
    QCheckBox*    flipCheckBox[3];
    QCheckBox*    EnhanceCheckBox;

    QSlider*      glyphResolutionSlider;
    QSlider*      minorScalingSlider;
    QSlider*      majorScalingSlider;

    QSlider*      SliceSlider[3];
    QSpinBox*     SliceSpinBox[3];

    QCheckBox*    hideShowCheckBox[3];  //  Show/Hide Axial/Coronal/Sagittal planes.

    QStringList   tesselationTypeList;
    QStringList   tesselationBasisList;
};

medSHViewToolBox::medSHViewToolBox(QWidget *parent): medToolBox(parent),d(new medSHViewToolBoxPrivate) {

    QWidget* displayWidget = new QWidget(this);

    d->tesselationTypeList  << "Icosahedron" << "Octahedron" << "Tetrahedron";
    d->tesselationBasisList << "SHMatrix" << "SHMatrixMaxThesis" << "SHMatrixTournier" << "SHMatrixRshBasis";

    // combobox to control the glyph shape

    d->tesselationTypeComboBox = new QComboBox(displayWidget);
    d->tesselationTypeComboBox->addItems(d->tesselationTypeList);

    // combobox to control the spherical Harmonics basis

    d->tesselationBasisComboBox = new QComboBox(displayWidget);
    d->tesselationBasisComboBox->addItems(d->tesselationBasisList);

    // slider to control sample rate

    d->sampleRateSlider =  new QSlider(Qt::Horizontal,displayWidget);
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

    QHBoxLayout* flipAxesLayout = new QHBoxLayout;

    static const char* FlipNames[] = { "X", "Y", "Z" };
    for (unsigned i=0;i<3;++i) {
        d->flipCheckBox[i] = new QCheckBox(FlipNames[i],displayWidget);
        flipAxesLayout->addWidget(d->flipCheckBox[i]);
    }
    d->flipCheckBox[2]->setCheckState(Qt::Checked);
    d->EnhanceCheckBox = new QCheckBox("Enhance",displayWidget);
    flipAxesLayout->addWidget(d->EnhanceCheckBox);

    // slider to control glyph resolution
    d->glyphResolutionSlider = new QSlider(Qt::Horizontal,displayWidget);
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

    connect(d->glyphResolutionSlider,SIGNAL(valueChanged(int)),   glyphResolutionSpinBox,  SLOT(setValue(int)));
    connect(glyphResolutionSpinBox,  SIGNAL(editingFinished(int)),d->glyphResolutionSlider,SLOT(setValue(int)));

    // minor scaling

    d->minorScalingSlider = new QSlider(Qt::Horizontal,displayWidget);
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

    connect(d->minorScalingSlider,SIGNAL(valueChanged(int)),minorScalingSpinBox,  SLOT(setValue(int)));
    connect(minorScalingSpinBox,  SIGNAL(valueChanged(int)),d->minorScalingSlider,SLOT(setValue(int)));

    // major scaling

    d->majorScalingSlider = new QSlider(Qt::Horizontal,displayWidget);
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

    connect(d->majorScalingSlider,SIGNAL(valueChanged(int)),majorScalingSpinBox,  SLOT(setValue(int)));
    connect(majorScalingSpinBox,  SIGNAL(valueChanged(int)),d->majorScalingSlider,SLOT(setValue(int)));

    // hide or show axial, coronal, and sagittal
 
    QHBoxLayout* slicesLayout = new QHBoxLayout;
    static const char* SliceNames[] = { "Axial", "Coronal", "Sagittal" };
    for (unsigned i=0;i<3;++i) {
        d->hideShowCheckBox[i] = new QCheckBox(SliceNames[i],displayWidget);
        d->hideShowCheckBox[i]->setChecked(true);
        slicesLayout->addWidget(d->hideShowCheckBox[i]);
    }

    // layout all the controls in the toolbox
    QFormLayout *layout = new QFormLayout(displayWidget);
    layout->addRow(tr("Tesselation:"),d->tesselationTypeComboBox);
    layout->addRow(tr("SH basis:"),d->tesselationBasisComboBox);

    layout->addRow(tr("Sample rate:"),sampleRateLayout);
    layout->addRow(tr("Flip:"),flipAxesLayout);
    layout->addRow(tr("Resolution:"),glyphResolutionLayout);
    layout->addRow(tr("Minor scaling:"),minorScalingLayout);
    layout->addRow(tr("Major scaling:"),majorScalingLayout);
    layout->addRow(tr("Hide/show:"),slicesLayout);
    layout->addRow(tr("Axial Slice:"),SliceLayout[2]);
    layout->addRow(tr("Coronal Slice:"),SliceLayout[1]);
    layout->addRow(tr("Sagital Slice:"),SliceLayout[0]);

    // connect all the signals

    connect(d->tesselationTypeComboBox, SIGNAL(currentIndexChanged(const QString&)),this,SIGNAL(tesselationTypeChanged(const QString&)));
    connect(d->tesselationBasisComboBox,SIGNAL(currentIndexChanged(const QString&)),this,SIGNAL(tesselationBasisChanged(const QString&)));

    connect(d->sampleRateSlider,     SIGNAL(valueChanged(int)),this,SIGNAL(sampleRateChanged(int)));
    connect(d->glyphResolutionSlider,SIGNAL(valueChanged(int)),this,SIGNAL(glyphResolutionChanged(int)));

    connect(d->SliceSlider[0],SIGNAL(valueChanged(int)),this,SIGNAL(xSliceChanged(int)));
    connect(d->SliceSlider[1],SIGNAL(valueChanged(int)),this,SIGNAL(ySliceChanged(int)));
    connect(d->SliceSlider[2],SIGNAL(valueChanged(int)),this,SIGNAL(zSliceChanged(int)));

    // some signals (checkboxes) require one more step to translate from Qt::CheckState to bool

    connect(d->flipCheckBox[0],SIGNAL(stateChanged(int)),this,SLOT(onFlipXCheckBoxStateChanged(int)));
    connect(d->flipCheckBox[1],SIGNAL(stateChanged(int)),this,SLOT(onFlipYCheckBoxStateChanged(int)));
    connect(d->flipCheckBox[2],SIGNAL(stateChanged(int)),this,SLOT(onFlipZCheckBoxStateChanged(int)));

    connect(d->EnhanceCheckBox,SIGNAL(stateChanged(int)),this,SLOT(onEnhanceCheckBoxStateChanged(int)));

    connect(d->hideShowCheckBox[0],SIGNAL(stateChanged(int)),this,SLOT(onHideShowAxialChanged(int)));
    connect(d->hideShowCheckBox[1],SIGNAL(stateChanged(int)),this,SLOT(onHideShowCoronalChanged(int)));
    connect(d->hideShowCheckBox[2],SIGNAL(stateChanged(int)),this,SLOT(onHideShowSagittalChanged(int)));

    //  We need to calculate one single number for the scale, out of the minor and major scales

    connect(d->minorScalingSlider,SIGNAL(valueChanged(int)),this,SLOT(onMinorScalingChanged(const int)));
    connect(d->majorScalingSlider,SIGNAL(valueChanged(int)),this,SLOT(onMajorScalingChanged(const int)));

    this->setTitle("Spherical Harmonics View");
    this->addWidget(displayWidget);
}

medSHViewToolBox::~medSHViewToolBox() {
    delete d;
    d = NULL;
}

QString medSHViewToolBox::tesselationType() {
    return d->tesselationTypeComboBox->currentText();
}

QString medSHViewToolBox::tesselationBasis() {
    return d->tesselationBasisComboBox->currentText();
}

int medSHViewToolBox::sampleRate() {
    return d->sampleRateSlider->value();
}

void medSHViewToolBox::onGlyphResolutionChanged(const int resolution) {
    std::cerr << "Change of scale: " << resolution << std::endl;
    emit(glyphResolutionChanged(resolution));
}

bool medSHViewToolBox::isFlipX() {
    return d->flipCheckBox[0]->checkState()==Qt::Checked;
}

bool medSHViewToolBox::isFlipY() {
    return d->flipCheckBox[1]->checkState()==Qt::Checked;
}

bool medSHViewToolBox::isFlipZ() {
    return d->flipCheckBox[2]->checkState()==Qt::Checked;
}

bool medSHViewToolBox::isEnhanced() {
    return d->EnhanceCheckBox->checkState()==Qt::Checked;
}

int medSHViewToolBox::glyphResolution() {
    return d->glyphResolutionSlider->value();
}

// void medSHViewToolBox::setGlyphResolution(const int value)
// {
//     d->glyphResolutionSlider->blockSignals(true);
//     d->glyphResolutionSlider->setValue(value);
//     d->glyphResolutionSlider->blockSignals(false);
// }

double medSHViewToolBox::scale() {
    const int minorScale = d->minorScalingSlider->value();
    const int majorScaleExponent = d->majorScalingSlider->value();
    const double majorScale = pow(10.0,majorScaleExponent);
    return majorScale*minorScale;
}

bool medSHViewToolBox::isShowAxial() {
    return d->hideShowCheckBox[0]->checkState()==Qt::Checked;
}

bool medSHViewToolBox::isShowCoronal() {
    return d->hideShowCheckBox[1]->checkState()==Qt::Checked;
}

bool medSHViewToolBox::isShowSagittal() {
    return d->hideShowCheckBox[2]->checkState()==Qt::Checked;
}

void medSHViewToolBox::onFlipXCheckBoxStateChanged(const int checkBoxState) {
    emit flipX(checkBoxState==Qt::Checked);
}

void medSHViewToolBox::onFlipYCheckBoxStateChanged(const int checkBoxState) {
    emit flipY(checkBoxState==Qt::Checked);
}

void medSHViewToolBox::onFlipZCheckBoxStateChanged(const int checkBoxState) {
    emit flipZ(checkBoxState==Qt::Checked);
}

void medSHViewToolBox::onEnhanceCheckBoxStateChanged(const int checkBoxState) {
    emit normalize(checkBoxState==Qt::Checked);
}

void medSHViewToolBox::onScaleChanged(const double mantissa,const int exponent) {
    const double scale_factor = pow(10.0,exponent);
    onScaleChanged(mantissa*scale_factor);
}

void medSHViewToolBox::onScaleChanged(const double scale) {
    std::cerr << "Scale change: " << scale << std::endl;
    emit scalingChanged(scale);
}

void medSHViewToolBox::onMinorScalingChanged(const int minorScale) {
    const int majorScaleExponent = d->majorScalingSlider->value();
    onScaleChanged(minorScale,majorScaleExponent);
}

void medSHViewToolBox::onMajorScalingChanged(const int majorScaleExponent) {
    const double minorScale = d->minorScalingSlider->value();
    onScaleChanged(minorScale,majorScaleExponent);
}

void medSHViewToolBox::onHideShowAxialChanged(const int checkBoxState) {
    emit hideShowAxial(checkBoxState==Qt::Checked);
}

void medSHViewToolBox::onHideShowCoronalChanged(const int checkBoxState) {
    emit hideShowCoronal(checkBoxState==Qt::Checked);
}

void medSHViewToolBox::onHideShowSagittalChanged(const int checkBoxState) {
    emit hideShowSagittal(checkBoxState==Qt::Checked);
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
