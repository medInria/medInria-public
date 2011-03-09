#include "medToolBoxDiffusionTensorView.h"

class medToolBoxDiffusionTensorViewPrivate
{
public:
    QComboBox	 *glyphShape;
};


medToolBoxDiffusionTensorView::medToolBoxDiffusionTensorView(QWidget *parent) : medToolBox(parent), d(new medToolBoxDiffusionTensorViewPrivate)
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

void medToolBoxDiffusionTensorView::onGlyphShapeChanged(QString shape)
{
//    if (d->view) {
//        d->view->setProperty("Preset", table);
//        d->view->update();
//    }
}
