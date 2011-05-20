#include "medViewContainerFiltering.h"

//TODO: synchronize views

class medViewContainerFilteringPrivate
{
public:
    medViewContainerCustom* inputViewContainer;
    medViewContainerCustom* outputViewContainer;
    QList<dtkAbstractView *> views;
};

medViewContainerFiltering::medViewContainerFiltering(QWidget * parent):
        medViewContainerCustom(parent), d3(new medViewContainerFilteringPrivate)
{
    split(1, 2);

    d3->inputViewContainer = children()[0];
    d3->outputViewContainer = children()[1];

    //Set cues for the user:
    d3->inputViewContainer->setInfo(tr("Input"));
    d3->outputViewContainer->setInfo(tr("Output"));

    d3->views.append(dtkAbstractViewFactory::instance()->create("v3dView"));
    d3->inputViewContainer->setAcceptDrops(false);
    d3->inputViewContainer->update();

    d3->views.append(dtkAbstractViewFactory::instance()->create("v3dView"));
    d3->outputViewContainer->setAcceptDrops(false);
    d3->outputViewContainer->update();
}

medViewContainerFiltering::~medViewContainerFiltering()
{
    delete d3;
    d3 = NULL;
}

void medViewContainerFiltering::updateInput(dtkAbstractData *data)
{
    if(!data)
        return;

//  d3->views[0]->unlink(d3->views[1]);

    d3->views[0]->setData(data,0);
    d3->views[0]->reset();
    d3->views[0]->update();

//  d3->views[0]->link(d3->views[1]);

    d3->inputViewContainer->setView(d3->views[0]);

    d3->views[1]->reset();
    d3->outputViewContainer->setView(d3->views[1]);
}

void medViewContainerFiltering::updateOutput(dtkAbstractData *data)
{
    if(!data)
        return;

    d3->views[1]->reset();
    d3->views[1]->setData(data,0);
    d3->views[1]->reset();
    d3->views[1]->update();

    d3->outputViewContainer->setView(d3->views[1]);

//    d3->views[1]->link(d3->views[0]);
}
