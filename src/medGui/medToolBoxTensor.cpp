#include "medToolBoxTensor.h"

class medToolBoxTensorPrivate
{
public:
    QCheckBox    *dummyCheckBox;
};


medToolBoxTensor::medToolBoxTensor(QWidget *parent) : medToolBox(parent), d(new medToolBoxTensorPrivate)
{
    QWidget *displayWidget = new QWidget(this);

    d->dummyCheckBox = new QCheckBox("I am dummy", displayWidget);

    QHBoxLayout *layout = new QHBoxLayout(displayWidget);
    layout->addWidget(d->dummyCheckBox);

    //connect (d->displayCheckBox,       SIGNAL(toggled(bool)),            this, SIGNAL (GPUActivated (bool)));

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
