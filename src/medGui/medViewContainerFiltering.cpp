#include "medViewContainerFiltering.h"

//TODO: synchronize views

class medViewContainerFilteringPrivate
{
public:
    medViewContainerCustom* inputViewContainer;
    medViewContainerCustom* outputViewContainer;
    dtkAbstractData * inputData;
    dtkAbstractView * inputView;
    dtkAbstractView * outputView;
};

medViewContainerFiltering::medViewContainerFiltering(QWidget * parent):
        medViewContainerCustom(parent), d3(new medViewContainerFilteringPrivate)
{
    split(1, 2);

    d3->inputViewContainer = children()[0];
    d3->outputViewContainer = children()[1];

    //Set cues for the user:
    d3->inputViewContainer->setInfo(tr("Input"));
    d3->outputViewContainer->setInfo(tr("Result of filtering ..."));

    d3->inputView = dtkAbstractViewFactory::instance()->create("v3dView");
    d3->inputViewContainer->setAcceptDrops(false);
    d3->inputViewContainer->update();

    d3->outputView = dtkAbstractViewFactory::instance()->create("v3dView");
    d3->outputViewContainer->setAcceptDrops(false);
    d3->outputViewContainer->update();

    d3->inputData = NULL;
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

    d3->inputView->setData(data,0);
    d3->inputView->reset();
    d3->inputView->update();

//  d3->views[0]->link(d3->views[1]);

    d3->inputViewContainer->setView(d3->inputView);
    d3->inputViewContainer->update();

    if(data != d3->inputData )
    {
//    d3->outputView->clear();
//    d3->outputView = dtkAbstractViewFactory::instance()->create("v3dView");
    qDebug() << "OK here";
    d3->outputViewContainer->clear();
    d3->outputViewContainer = children()[1];
    d3->outputViewContainer->setInfo(tr("Result of filtering ..."));
    d3->outputViewContainer->update();
    }
}

void medViewContainerFiltering::updateOutput(dtkAbstractData *data)
{
    if(!data)
        return;

//    d3->outputView->clear();
    d3->outputView = dtkAbstractViewFactory::instance()->create("v3dView");
    d3->outputView->setData(data,0);
    d3->outputView->reset();
    d3->outputView->update();

    d3->outputViewContainer->setView(d3->outputView);
    d3->outputViewContainer->update();

//    d3->views[1]->link(d3->views[0]);
}
