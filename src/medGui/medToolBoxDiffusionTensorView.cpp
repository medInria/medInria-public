#include "medToolBoxDiffusionTensorView.h"

class medToolBoxDiffusionTensorViewPrivate
{
public:
    QComboBox*    glyphShape;
    QSlider*      sampleRate;
    QCheckBox*    flipX;
    QCheckBox*    flipY;
    QCheckBox*    flipZ;
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

    QHBoxLayout* sampleRateLayout = new QHBoxLayout;
    sampleRateLayout->addWidget(new QLabel("Sample rate: "));
    sampleRateLayout->addWidget(d->sampleRate);

    // flipX, flipY and flipZ checkboxes
    d->flipX = new QCheckBox("Flip X", displayWidget);
    d->flipY = new QCheckBox("Flip Y", displayWidget);
    d->flipZ = new QCheckBox("Flip Z", displayWidget);

    QHBoxLayout* flipAxesLayout = new QHBoxLayout;
    flipAxesLayout->addWidget(d->flipX);
    flipAxesLayout->addWidget(d->flipY);
    flipAxesLayout->addWidget(d->flipZ);

    // layout all the controls in the toolbox
    QVBoxLayout* layout = new QVBoxLayout(displayWidget);
    layout->addLayout(glyphShapeLayout);
    layout->addLayout(sampleRateLayout);
    layout->addLayout(flipAxesLayout);

    // connect all the signals
    connect(d->glyphShape,      SIGNAL(currentIndexChanged(const QString&)), this, SIGNAL(glyphShapeChanged(const QString&)));
    connect(d->sampleRate,      SIGNAL(valueChanged(int)),                   this, SIGNAL(sampleRateChanged(int)));

    // the axes signals require one more step to translate from Qt::CheckState to bool
    connect(d->flipX,           SIGNAL(stateChanged(int)),                   this, SLOT(onFlipXCheckBoxStateChanged(int)));
    connect(d->flipY,           SIGNAL(stateChanged(int)),                   this, SLOT(onFlipYCheckBoxStateChanged(int)));
    connect(d->flipZ,           SIGNAL(stateChanged(int)),                   this, SLOT(onFlipZCheckBoxStateChanged(int)));

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
