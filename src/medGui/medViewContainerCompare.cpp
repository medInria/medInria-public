#include "medViewContainerCompare.h"




class medViewContainerComparePrivate
{
public:
    medViewContainerCustom * fixedContainer;
    medViewContainerCustom * movingContainer;
};


medViewContainerCompare::medViewContainerCompare(QWidget * parent):
        medViewContainerCustom(parent), d3(new medViewContainerComparePrivate)
{
    split(1, 2);
    d3->fixedContainer = children()[0];
    d3->movingContainer = children()[1];
    connect(d3->fixedContainer,SIGNAL(dropped(medDataIndex)),
            this,SIGNAL(droppedFixed(medDataIndex)));
    connect(d3->movingContainer,SIGNAL(dropped(medDataIndex)),
            this,SIGNAL(droppedMoving(medDataIndex)));
    //Set cues for the user:
    d3->fixedContainer->setInfo(tr("Fixed"));
    d3->movingContainer->setInfo(tr("Moving"));
}

medViewContainerCompare::~medViewContainerCompare()
{
    delete d3;
    d3 = NULL;
}
