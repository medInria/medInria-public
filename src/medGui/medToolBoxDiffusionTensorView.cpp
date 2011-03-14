#include "medToolBoxDiffusionTensorView.h"

class medToolBoxDiffusionTensorViewPrivate
{
public:
    QComboBox*    glyphShape;
    QSlider*      sampleRate;
    QCheckBox*    flipX;
    QCheckBox*    flipY;
    QCheckBox*    flipZ;
    QRadioButton* eigenVectorV1;
    QRadioButton* eigenVectorV2;
    QRadioButton* eigenVectorV3;
    QCheckBox*    reverseBackgroundColor;
    QSlider*      glyphResolution;

    // labels for showing the slider's actual value
    QLabel* sampleRateLabel;
};

medToolBoxDiffusionTensorView::medToolBoxDiffusionTensorView(QWidget *parent) : medToolBox(parent), d(new medToolBoxDiffusionTensorViewPrivate)
{
    QWidget* displayWidget = new QWidget(this);

    // combobox to control the glyph shape
    d->glyphShape = new QComboBox(displayWidget);
    d->glyphShape->addItem("Lines");
    d->glyphShape->addItem("Arrows");
    d->glyphShape->addItem("Disks");
    d->glyphShape->addItem("Cylinders");
    d->glyphShape->addItem("Cubes");
    d->glyphShape->addItem("Ellipsoids");
    d->glyphShape->addItem("Superquadrics");

    QHBoxLayout* glyphShapeLayout = new QHBoxLayout;
    glyphShapeLayout->addWidget(new QLabel("Shape: "));
    glyphShapeLayout->addWidget(d->glyphShape);

    // slider to control sample rate
    d->sampleRate =  new QSlider(Qt::Horizontal, displayWidget);
    d->sampleRate->setMinimum(1);
    d->sampleRate->setMaximum(10);
    d->sampleRate->setSingleStep(1);
    d->sampleRate->setValue(1);

    d->sampleRateLabel = new QLabel("1", displayWidget);

    QHBoxLayout* sampleRateLayout = new QHBoxLayout;
    sampleRateLayout->addWidget(new QLabel("Sample rate: "));
    sampleRateLayout->addWidget(d->sampleRate);
    sampleRateLayout->addWidget(d->sampleRateLabel);

    connect(d->sampleRate, SIGNAL(valueChanged(int)), d->sampleRateLabel, SLOT(setNum(int)));

    // flipX, flipY and flipZ checkboxes
    d->flipX = new QCheckBox("Flip X", displayWidget);
    d->flipY = new QCheckBox("Flip Y", displayWidget);
    d->flipZ = new QCheckBox("Flip Z", displayWidget);

    QHBoxLayout* flipAxesLayout = new QHBoxLayout;
    flipAxesLayout->addWidget(d->flipX);
    flipAxesLayout->addWidget(d->flipY);
    flipAxesLayout->addWidget(d->flipZ);

    // eigen vectors
    d->eigenVectorV1 = new QRadioButton("v1", displayWidget);
    d->eigenVectorV2 = new QRadioButton("v2", displayWidget);
    d->eigenVectorV3 = new QRadioButton("v3", displayWidget);

    QButtonGroup *eigenVectorRadioGroup = new QButtonGroup(displayWidget);
    eigenVectorRadioGroup->addButton(d->eigenVectorV1);
    eigenVectorRadioGroup->addButton(d->eigenVectorV2);
    eigenVectorRadioGroup->addButton(d->eigenVectorV3);
    eigenVectorRadioGroup->setExclusive(true);

    QHBoxLayout *eigenVectorGroupLayout = new QHBoxLayout;
    eigenVectorGroupLayout->addWidget(d->eigenVectorV1);
    eigenVectorGroupLayout->addWidget(d->eigenVectorV2);
    eigenVectorGroupLayout->addWidget(d->eigenVectorV3);
    //eigenVectorGroupLayout->addStretch(1);

    QVBoxLayout* eigenLayout = new QVBoxLayout;
    eigenLayout->addWidget(new QLabel("Eigen Vector for color-coding:"));
    eigenLayout->addLayout(eigenVectorGroupLayout);

    d->eigenVectorV1->setChecked(true);

    // reverse background color
    d->reverseBackgroundColor = new QCheckBox("Reverse Background Color", displayWidget);

    // slider to control glyph resolution
    d->glyphResolution =  new QSlider(Qt::Horizontal, displayWidget);
    d->glyphResolution->setMinimum(2);
    d->glyphResolution->setMaximum(20);
    d->glyphResolution->setSingleStep(1);
    d->glyphResolution->setValue(6);
    //d->glyphResolution->setTickPosition(QSlider::TicksAbove);


    QHBoxLayout* glyphResolutionLayout = new QHBoxLayout;
    glyphResolutionLayout->addWidget(new QLabel("Glyph resolution: "));
    glyphResolutionLayout->addWidget(d->glyphResolution);

    // layout all the controls in the toolbox
    QVBoxLayout* layout = new QVBoxLayout(displayWidget);
    layout->addLayout(glyphShapeLayout);
    layout->addLayout(sampleRateLayout);
    layout->addLayout(flipAxesLayout);
    layout->addLayout(eigenLayout);
    layout->addWidget(d->reverseBackgroundColor);
    layout->addLayout(glyphResolutionLayout);

    // connect all the signals
    connect(d->glyphShape,              SIGNAL(currentIndexChanged(const QString&)), this, SIGNAL(glyphShapeChanged(const QString&)));
    connect(d->sampleRate,              SIGNAL(valueChanged(int)),                   this, SIGNAL(sampleRateChanged(int)));
    connect(d->reverseBackgroundColor,  SIGNAL(stateChanged(int)),                   this, SIGNAL(reverseBackgroundColor(bool)));
    connect(d->glyphResolution,         SIGNAL(valueChanged(int)),                   this, SIGNAL(glyphResolutionChanged(int)));

    // the axes signals require one more step to translate from Qt::CheckState to bool
    connect(d->flipX,           SIGNAL(stateChanged(int)),                   this, SLOT(onFlipXCheckBoxStateChanged(int)));
    connect(d->flipY,           SIGNAL(stateChanged(int)),                   this, SLOT(onFlipYCheckBoxStateChanged(int)));
    connect(d->flipZ,           SIGNAL(stateChanged(int)),                   this, SLOT(onFlipZCheckBoxStateChanged(int)));

    // we also need to translate radio buttons boolean states to an eigen vector
    connect(d->eigenVectorV1,   SIGNAL(toggled(bool)),                       this, SLOT(onEigenVectorV1Toggled(bool)));
    connect(d->eigenVectorV2,   SIGNAL(toggled(bool)),                       this, SLOT(onEigenVectorV2Toggled(bool)));
    connect(d->eigenVectorV3,   SIGNAL(toggled(bool)),                       this, SLOT(onEigenVectorV3Toggled(bool)));

    this->setTitle("Tensor View");
    this->addWidget(displayWidget);
}

medToolBoxDiffusionTensorView::~medToolBoxDiffusionTensorView()
{
    delete d;
    d = NULL;
}

void medToolBoxDiffusionTensorView::update (dtkAbstractView *view)
{
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
