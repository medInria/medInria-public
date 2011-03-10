#include "medToolBoxDiffusionTensorView.h"

class medToolBoxDiffusionTensorViewPrivate
{
public:
    QComboBox	 *glyphShape;
};

medToolBoxDiffusionTensorView::medToolBoxDiffusionTensorView(QWidget *parent) : medToolBox(parent), d(new medToolBoxDiffusionTensorViewPrivate)
{
    QWidget *displayWidget = new QWidget(this);

    // widget to control the glyph shape
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

    // connect all the signals
    connect(d->glyphShape,      SIGNAL(currentIndexChanged(const QString&)), this, SIGNAL(glyphShapeChanged(const QString&)));

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
