#include "medToolBoxSegmentationView.h"

#include <QPushButton>

class medToolBoxSegmentationViewPrivate
{
public:
    QPushButton *addSeedPointButton;
};


medToolBoxSegmentationView::medToolBoxSegmentationView(QWidget *parent) : 
    medToolBox(parent), 
    d(new medToolBoxSegmentationViewPrivate)
{
    QWidget *displayWidget = new QWidget(this);

    QVBoxLayout *displayLayout = new QVBoxLayout(displayWidget);

    d->addSeedPointButton = new QPushButton(tr("Add Seed Point") , displayWidget );

    displayLayout->addWidget(d->addSeedPointButton);

    connect (d->addSeedPointButton,     SIGNAL(pressed()),            this, SIGNAL (addSeedPointPressed ()));

    this->setTitle("Segmentation");
    this->addWidget(displayWidget);
}

medToolBoxSegmentationView::~medToolBoxSegmentationView()
{
    delete d;
    d = NULL;
}

void medToolBoxSegmentationView::update (dtkAbstractView *view)
{
}
