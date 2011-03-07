#include "medToolBoxTensor.h"

class medToolBoxTensorPrivate
{
public:
    QComboBox	 *glyphShape;
};


medToolBoxTensor::medToolBoxTensor(QWidget *parent) : medToolBox(parent), d(new medToolBoxTensorPrivate)
{
    QWidget *displayWidget = new QWidget(this);

    d->glyphShape = new QComboBox(displayWidget);
    d->glyphShape->addItem("Lines");
    d->glyphShape->addItem("Arrows");
    d->glyphShape->addItem("Disks");
    d->glyphShape->addItem("Cylinders");
    d->glyphShape->addItem("Cubes");
    d->glyphShape->addItem("Ellipsoids");
    d->glyphShape->addItem("Superquadrics");


    QHBoxLayout *layout = new QHBoxLayout(displayWidget);
    layout->addWidget(new QLabel("Shape: "));
    layout->addWidget(d->glyphShape);

    //connect (d->displayCheckBox,       SIGNAL(toggled(bool)),            this, SIGNAL (GPUActivated (bool)));
    connect(d->glyphShape,               SIGNAL(currentIndexChanged(QString)), this, SLOT(onGlyphShapeChanged(QString)));

    this->setTitle("Tensor");
    this->addWidget(displayWidget);
}

medToolBoxTensor::~medToolBoxTensor()
{
    delete d;
    d = NULL;
}

void medToolBoxTensor::update (dtkAbstractView *view)
{
}

void medToolBoxTensor::onGlyphShapeChanged(QString shape)
{
//    if (d->view) {
//        d->view->setProperty("Preset", table);
//        d->view->update();
//    }
}
