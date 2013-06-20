#include "medSHViewToolBox.h"
#include <medCore/medSHAbstractViewInteractor.h>
#include <dtkCore/dtkAbstractView.h>
#include <math.h>

class medSHViewToolBoxPrivate
{
public:
    QComboBox*    tesselationTypeComboBox;
    QComboBox*    tesselationBasisComboBox;

    QSlider*      sampleRateSlider;
    QCheckBox*    flipXCheckBox;
    QCheckBox*    flipYCheckBox;
    QCheckBox*    flipZCheckBox;
    QCheckBox*    MaxThesisFuncCheckBox;
//    QRadioButton* eigenVectorV1RadioButton;
//    QRadioButton* eigenVectorV2RadioButton;
//    QRadioButton* eigenVectorV3RadioButton;
    QCheckBox*    reverseBackgroundColorCheckBox;
    QSlider*      glyphResolutionSlider;
    QSlider*      minorScalingSlider;
    QSlider*      majorScalingSlider;

    QSlider*      xSliceSlider;
    QSlider*      ySliceSlider;
    QSlider*      zSliceSlider;

    QSpinBox* xSliceSpinBox;
    QSpinBox* ySliceSpinBox;
    QSpinBox* zSliceSpinBox;


//    QVector3D* position;

    QCheckBox*    hideShowAxialCheckBox;
    QCheckBox*    hideShowCoronalCheckBox;
    QCheckBox*    hideShowSagittalCheckBox;

    QStringList tesselationTypeList;
    QStringList tesselationBasisList;

};

medSHViewToolBox::medSHViewToolBox(QWidget *parent) : medToolBox(parent), d(new medSHViewToolBoxPrivate)
{

    QWidget* displayWidget = new QWidget(this);

    d->tesselationTypeList = *(new QStringList());
    d->tesselationTypeList << "Icosahedron" << "Octahedron" << "Tetrahedron";

    d->tesselationBasisList = *(new QStringList());
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

    // slider to control x SLICE
    d->xSliceSlider =  new QSlider(Qt::Horizontal, displayWidget);
    d->xSliceSlider->setMinimum(0);
    d->xSliceSlider->setMaximum(100);
    d->xSliceSlider->setSingleStep(1);
    d->xSliceSlider->setValue(0);

    d->xSliceSpinBox = new QSpinBox(displayWidget);
    d->xSliceSpinBox->setMinimum(0);
    d->xSliceSpinBox->setMaximum(100);
    d->xSliceSpinBox->setSingleStep(1);
    d->xSliceSpinBox->setValue(0);

    QHBoxLayout* xSliceLayout = new QHBoxLayout;
    xSliceLayout->addWidget(d->xSliceSlider);
    xSliceLayout->addWidget(d->xSliceSpinBox);

    connect(d->xSliceSlider, SIGNAL(valueChanged(int)), d->xSliceSpinBox, SLOT(setValue(int)));
    connect(d->xSliceSpinBox, SIGNAL(valueChanged(int)), d->xSliceSlider, SLOT(setValue(int)));

    // slider to control y SLICE
    d->ySliceSlider =  new QSlider(Qt::Horizontal, displayWidget);
    d->ySliceSlider->setMinimum(0);
    d->ySliceSlider->setMaximum(100);
    d->ySliceSlider->setSingleStep(1);
    d->ySliceSlider->setValue(0);

    d->ySliceSpinBox = new QSpinBox(displayWidget);
    d->ySliceSpinBox->setMinimum(0);
    d->ySliceSpinBox->setMaximum(100);
    d->ySliceSpinBox->setSingleStep(1);
    d->ySliceSpinBox->setValue(0);

    QHBoxLayout* ySliceLayout = new QHBoxLayout;
    ySliceLayout->addWidget(d->ySliceSlider);
    ySliceLayout->addWidget(d->ySliceSpinBox);

    connect(d->ySliceSlider, SIGNAL(valueChanged(int)), d->ySliceSpinBox, SLOT(setValue(int)));
    connect(d->ySliceSpinBox, SIGNAL(valueChanged(int)), d->ySliceSlider, SLOT(setValue(int)));

    // slider to control z SLICE
    d->zSliceSlider =  new QSlider(Qt::Horizontal, displayWidget);
    d->zSliceSlider->setMinimum(0);
    d->zSliceSlider->setMaximum(100);
    d->zSliceSlider->setSingleStep(1);
    d->zSliceSlider->setValue(0);

    d->zSliceSpinBox = new QSpinBox(displayWidget);
    d->zSliceSpinBox->setMinimum(0);
    d->zSliceSpinBox->setMaximum(100);
    d->zSliceSpinBox->setSingleStep(1);
    d->zSliceSpinBox->setValue(0);

    QHBoxLayout* zSliceLayout = new QHBoxLayout;
    zSliceLayout->addWidget(d->zSliceSlider);
    zSliceLayout->addWidget(d->zSliceSpinBox);

    connect(d->zSliceSlider, SIGNAL(valueChanged(int)), d->zSliceSpinBox, SLOT(setValue(int)));
    connect(d->zSliceSpinBox, SIGNAL(valueChanged(int)), d->zSliceSlider, SLOT(setValue(int)));


    // flipX, flipY and flipZ checkboxes
    d->flipXCheckBox = new QCheckBox("X", displayWidget);
    d->flipYCheckBox = new QCheckBox("Y", displayWidget);
    d->flipZCheckBox = new QCheckBox("Z", displayWidget);
    d->MaxThesisFuncCheckBox = new QCheckBox("SH Max PhD", displayWidget);


    QHBoxLayout* flipAxesLayout = new QHBoxLayout;
    flipAxesLayout->addWidget(d->flipXCheckBox);
    flipAxesLayout->addWidget(d->flipYCheckBox);
    flipAxesLayout->addWidget(d->flipZCheckBox);
    flipAxesLayout->addWidget(d->MaxThesisFuncCheckBox);

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
    d->glyphResolutionSlider->setMinimum(4);
    d->glyphResolutionSlider->setMaximum(10);
    d->glyphResolutionSlider->setSingleStep(2);
    d->glyphResolutionSlider->setValue(4);
    d->glyphResolutionSlider->setPageStep(2);

    QSpinBox* glyphResolutionSpinBox = new QSpinBox(displayWidget);
    glyphResolutionSpinBox->setMinimum(4);
    glyphResolutionSpinBox->setMaximum(10);
    glyphResolutionSpinBox->setSingleStep(2);
    glyphResolutionSpinBox->setValue(4);

    QHBoxLayout* glyphResolutionLayout = new QHBoxLayout;
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
    d->minorScalingSlider->setPageStep(1);


    QSpinBox* minorScalingSpinBox = new QSpinBox(displayWidget);
    minorScalingSpinBox->setMinimum(1);
    minorScalingSpinBox->setMaximum(9);
    minorScalingSpinBox->setSingleStep(1);
    minorScalingSpinBox->setValue(1);

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
    d->majorScalingSlider->setPageStep(1);

    d->majorScalingSlider->setValue(0);

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
    layout->addRow(tr("Axial Slice:"), zSliceLayout);
    layout->addRow(tr("Coronal Slice:"), ySliceLayout);
    layout->addRow(tr("Sagital Slice:"), xSliceLayout);

    // connect all the signals
    connect(d->tesselationTypeComboBox,              SIGNAL(currentIndexChanged(const QString&)), this, SIGNAL(tesselationTypeChanged(const QString&)));
    connect(d->tesselationBasisComboBox,              SIGNAL(currentIndexChanged(const QString&)), this, SIGNAL(tesselationBasisChanged(const QString&)));

    connect(d->sampleRateSlider,              SIGNAL(valueChanged(int)),                   this, SIGNAL(sampleRateChanged(int)));
    connect(d->glyphResolutionSlider,         SIGNAL(valueChanged(int)),                   this, SIGNAL(glyphResolutionChanged(int)));
    connect(d->xSliceSlider,         SIGNAL(valueChanged(int)),                   this, SIGNAL(xSliceChanged(int)));
    connect(d->ySliceSlider,         SIGNAL(valueChanged(int)),                   this, SIGNAL(ySliceChanged(int)));
    connect(d->zSliceSlider,         SIGNAL(valueChanged(int)),                   this, SIGNAL(zSliceChanged(int)));

    // some signals (checkboxes) require one more step to translate from Qt::CheckState to bool
    connect(d->flipXCheckBox,                   SIGNAL(stateChanged(int)),                   this, SLOT(onFlipXCheckBoxStateChanged(int)));
    connect(d->flipYCheckBox,                   SIGNAL(stateChanged(int)),                   this, SLOT(onFlipYCheckBoxStateChanged(int)));
    connect(d->flipZCheckBox,                   SIGNAL(stateChanged(int)),                   this, SLOT(onFlipZCheckBoxStateChanged(int)));

    connect(d->MaxThesisFuncCheckBox,                   SIGNAL(stateChanged(int)),                   this, SLOT(onMaxThesisFuncCheckBoxStateChanged(int)));

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
    return d->MaxThesisFuncCheckBox->checkState() == Qt::Checked;
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

void medSHViewToolBox::onMaxThesisFuncCheckBoxStateChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit maxThesisFunc(false);
    else if (checkBoxState == Qt::Checked)
        emit maxThesisFunc(true);
}

//void medSHViewToolBox::onReverseBackgroundColorChanged(int checkBoxState)
//{
//    if (checkBoxState == Qt::Unchecked)
//        emit reverseBackgroundColor(false);
//    else if (checkBoxState == Qt::Checked)
//        emit reverseBackgroundColor(true);
//}

//void medSHViewToolBox::onEigenVectorV1Toggled(bool isSelected)
//{
//    if (isSelected)
//        emit eigenVectorChanged(3);
//}

//void medSHViewToolBox::onEigenVectorV2Toggled(bool isSelected)
//{
//    if (isSelected)
//        emit eigenVectorChanged(2);
//}

//void medSHViewToolBox::onEigenVectorV3Toggled(bool isSelected)
//{
//    if (isSelected)
//        emit eigenVectorChanged(1);
//}

void medSHViewToolBox::onMinorScalingChanged(int minorScale)
{
    int majorScaleExponent = d->majorScalingSlider->value();
    double majorScale = pow(10.0, majorScaleExponent);
    double scale = majorScale * minorScale;
    emit scalingChanged(scale);
}

void medSHViewToolBox::onMajorScalingChanged(int majorScaleExponent)
{
    int minorScale = d->minorScalingSlider->value();
    double majorScale = pow(10.0, majorScaleExponent);
    double scale = majorScale * minorScale;
    emit scalingChanged(scale);
}

//void medSHViewToolBox::onXVoiChanged(int xVoi)
//{
//    d->position->setX(xVoi);
//    emit positionChanged(/*(const)*/ *(d->position), true);
//}

//void medSHViewToolBox::onYVoiChanged(int yVoi)
//{
//}

//void medSHViewToolBox::onZVoiChanged(int zVoi)
//{
//}


void medSHViewToolBox::onHideShowAxialChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit hideShowAxial(false);
    else if (checkBoxState == Qt::Checked)
        emit hideShowAxial(true);
}

void medSHViewToolBox::onHideShowCoronalChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit hideShowCoronal(false);
    else if (checkBoxState == Qt::Checked)
        emit hideShowCoronal(true);
}

void medSHViewToolBox::onHideShowSagittalChanged(int checkBoxState)
{
    if (checkBoxState == Qt::Unchecked)
        emit hideShowSagittal(false);
    else if (checkBoxState == Qt::Checked)
        emit hideShowSagittal(true);
}

void medSHViewToolBox::update (dtkAbstractView *view)
{
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

void medSHViewToolBox::updateWithInteractor (dtkAbstractView *view)
{

    if (!view)
        return;


    if (medSHAbstractViewInteractor *interactor = dynamic_cast<medSHAbstractViewInteractor*>(view->interactor ("v3dViewSHInteractor")))
    {
        int imSize[3]={0,0,0};
        double temp[3]={0,0,0};
        interactor->imageSize(imSize);
        d->xSliceSlider->setMaximum(imSize[0]-1);
        d->ySliceSlider->setMaximum(imSize[1]-1);
        d->zSliceSlider->setMaximum(imSize[2]-1);

        temp[0]=imSize[0]; temp[1]=imSize[1]; temp[2]=imSize[2];

        d->xSliceSlider->setPageStep(ceil(temp[0]/10));
        d->ySliceSlider->setPageStep(ceil(temp[1]/10));
        d->zSliceSlider->setPageStep(ceil(temp[2]/10));

        d->xSliceSlider->setValue(floor(temp[0]/2));
        d->ySliceSlider->setValue(floor(temp[1]/2));
        d->zSliceSlider->setValue(floor(temp[2]/2));

        d->xSliceSpinBox->setMaximum(imSize[0]-1);
        d->ySliceSpinBox->setMaximum(imSize[1]-1);
        d->zSliceSpinBox->setMaximum(imSize[2]-1);

    }

}

