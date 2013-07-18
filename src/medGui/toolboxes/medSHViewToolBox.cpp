/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <cmath>
#include <medSHViewToolBox.h>
#include <medCore/medSHAbstractViewInteractor.h>
#include <dtkCore/dtkAbstractView.h>
#include <medGui/toolboxes/medSliderSpinboxPair.h>

class medSHViewToolBoxPrivate {
public:
    QComboBox*            tesselationTypeComboBox;
    QComboBox*            tesselationBasisComboBox;

    QCheckBox*            flipCheckBox[3];
    QCheckBox*            EnhanceCheckBox;

    medSliderSpinboxPair* sampleRateControl;
    medSliderSpinboxPair* glyphResolutionControl;

    medSliderSpinboxPair* minorScalingControl;
    medSliderSpinboxPair* majorScalingControl;

    medSliderSpinboxPair* SliceControl[3];

    QCheckBox*            hideShowCheckBox[3];  //  Show/Hide Axial/Coronal/Sagittal planes.

    QStringList           tesselationTypeList;
    QStringList           tesselationBasisList;
};

medSHViewToolBox::medSHViewToolBox(QWidget *parent): medToolBox(parent),d(new medSHViewToolBoxPrivate) {

    QWidget* displayWidget = new QWidget(this);

    //  Combobox to control the glyph shape

    d->tesselationTypeList  << "Icosahedron" << "Octahedron" << "Tetrahedron";
    d->tesselationTypeComboBox = new QComboBox(displayWidget);
    d->tesselationTypeComboBox->addItems(d->tesselationTypeList);

    connect(d->tesselationTypeComboBox, SIGNAL(currentIndexChanged(const QString&)),this,SIGNAL(tesselationTypeChanged(const QString&)));

    //  Combobox to control the spherical Harmonics basis

    d->tesselationBasisList << "SHMatrix" << "SHMatrixMaxThesis" << "SHMatrixTournier" << "SHMatrixRshBasis";
    d->tesselationBasisComboBox = new QComboBox(displayWidget);
    d->tesselationBasisComboBox->addItems(d->tesselationBasisList);

    connect(d->tesselationBasisComboBox,SIGNAL(currentIndexChanged(const QString&)),this,SIGNAL(tesselationBasisChanged(const QString&)));

    //  Control sample rate

    d->sampleRateControl = new medSliderSpinboxPair(displayWidget);
    d->sampleRateControl->setMinimum(1);
    d->sampleRateControl->setMaximum(10);
    d->sampleRateControl->setValue(1);

    connect(d->sampleRateControl,SIGNAL(valueChanged(int)),this,SIGNAL(sampleRateChanged(int)));

    //  Control for slices

    for (unsigned i=0;i<3;++i) {
        d->SliceControl[i] = new medSliderSpinboxPair(displayWidget);
        d->SliceControl[i]->setMinimum(0);
        d->SliceControl[i]->setMaximum(100);
        d->SliceControl[i]->setValue(0);
    }

    connect(d->SliceControl[0],SIGNAL(valueChanged(int)),this,SIGNAL(xSliceChanged(int)));
    connect(d->SliceControl[1],SIGNAL(valueChanged(int)),this,SIGNAL(ySliceChanged(int)));
    connect(d->SliceControl[2],SIGNAL(valueChanged(int)),this,SIGNAL(zSliceChanged(int)));

    //  flipX, flipY, flipZ and Enhance checkboxes

    QHBoxLayout* flipAxesLayout = new QHBoxLayout;

    static const char* FlipNames[] = { "X", "Y", "Z" };
    for (unsigned i=0;i<3;++i) {
        d->flipCheckBox[i] = new QCheckBox(FlipNames[i],displayWidget);
        flipAxesLayout->addWidget(d->flipCheckBox[i]);
    }
    d->flipCheckBox[2]->setCheckState(Qt::Checked);

    connect(d->flipCheckBox[0],SIGNAL(stateChanged(int)),this,SLOT(onFlipXCheckBoxStateChanged(int)));
    connect(d->flipCheckBox[1],SIGNAL(stateChanged(int)),this,SLOT(onFlipYCheckBoxStateChanged(int)));
    connect(d->flipCheckBox[2],SIGNAL(stateChanged(int)),this,SLOT(onFlipZCheckBoxStateChanged(int)));

    d->EnhanceCheckBox = new QCheckBox("Enhance",displayWidget);
    flipAxesLayout->addWidget(d->EnhanceCheckBox);

    connect(d->EnhanceCheckBox,SIGNAL(stateChanged(int)),this,SLOT(onEnhanceCheckBoxStateChanged(int)));

    //  Control glyph resolution

    d->glyphResolutionControl = new medSliderSpinboxPair(displayWidget);
    d->glyphResolutionControl->setMinimum(0);
    d->glyphResolutionControl->setMaximum(10);
    d->glyphResolutionControl->setValue(2);

    connect(d->glyphResolutionControl,SIGNAL(valueChanged(int)),this,SIGNAL(glyphResolutionChanged(int)));

    //  We need to calculate one single number for the scale, out of the minor and major scales
    //  scale = minor*10^(major)

    //  Minor scaling

    d->minorScalingControl = new medSliderSpinboxPair(displayWidget);
    d->minorScalingControl->setMinimum(0);
    d->minorScalingControl->setMaximum(9);
    d->minorScalingControl->setValue(3);

    connect(d->minorScalingControl,SIGNAL(valueChanged(int)),this,SLOT(onMinorScalingChanged(const int)));

    //  Major scaling

    d->majorScalingControl = new medSliderSpinboxPair(displayWidget);
    d->majorScalingControl->setMinimum(-10);
    d->majorScalingControl->setMaximum(10);
    d->majorScalingControl->setValue(0);

    connect(d->majorScalingControl,SIGNAL(valueChanged(int)),this,SLOT(onMajorScalingChanged(const int)));

    //  Hide or show axial, coronal, and sagittal
 
    QHBoxLayout* slicesLayout = new QHBoxLayout;
    static const char* SliceNames[] = { "Axial", "Coronal", "Sagittal" };
    for (unsigned i=0;i<3;++i) {
        d->hideShowCheckBox[i] = new QCheckBox(SliceNames[i],displayWidget);
        d->hideShowCheckBox[i]->setChecked(true);
        slicesLayout->addWidget(d->hideShowCheckBox[i]);
    }

    connect(d->hideShowCheckBox[0],SIGNAL(stateChanged(int)),this,SLOT(onHideShowAxialChanged(int)));
    connect(d->hideShowCheckBox[1],SIGNAL(stateChanged(int)),this,SLOT(onHideShowCoronalChanged(int)));
    connect(d->hideShowCheckBox[2],SIGNAL(stateChanged(int)),this,SLOT(onHideShowSagittalChanged(int)));

    //  Layout all the controls in the toolbox

    QFormLayout *layout = new QFormLayout(displayWidget);
    layout->addRow(tr("Tesselation:"),d->tesselationTypeComboBox);
    layout->addRow(tr("SH basis:"),d->tesselationBasisComboBox);
    layout->addRow(tr("Sample rate:"),d->sampleRateControl);
    layout->addRow(tr("Flip:"),flipAxesLayout);
    layout->addRow(tr("Resolution:"),d->glyphResolutionControl);
    layout->addRow(tr("Minor scaling:"),d->minorScalingControl);
    layout->addRow(tr("Major scaling:"),d->majorScalingControl);
    layout->addRow(tr("Hide/show:"),slicesLayout);
    layout->addRow(tr("Axial Slice:"),d->SliceControl[2]);
    layout->addRow(tr("Coronal Slice:"),d->SliceControl[1]);
    layout->addRow(tr("Sagital Slice:"),d->SliceControl[0]);

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
    return d->sampleRateControl->value();
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
    return d->glyphResolutionControl->value();
}

double medSHViewToolBox::scale() {
    const int minorScale = d->minorScalingControl->value();
    const int majorScaleExponent = d->majorScalingControl->value();
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
    emit scalingChanged(scale);
}

void medSHViewToolBox::onMinorScalingChanged(const int minorScale) {
    const int majorScaleExponent = d->majorScalingControl->value();
    onScaleChanged(minorScale,majorScaleExponent);
}

void medSHViewToolBox::onMajorScalingChanged(const int majorScaleExponent) {
    const double minorScale = d->minorScalingControl->value();
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
}

void medSHViewToolBox::updateWithInteractor(dtkAbstractView *view) {

    if (!view)
        return;

    if (medSHAbstractViewInteractor* interactor = dynamic_cast<medSHAbstractViewInteractor*>(view->interactor ("v3dViewSHInteractor"))) {
        int imSize[3];
        interactor->imageSize(imSize);
        for (unsigned i=0;i<3;++i) {
            const double sz = imSize[i];
            d->SliceControl[i]->setMaximum(imSize[i]-1);
            //d->SliceControl[i]->setPageStep(ceil(sz/10));
            d->SliceControl[i]->setValue(floor(sz/2));
        }
    }
}
