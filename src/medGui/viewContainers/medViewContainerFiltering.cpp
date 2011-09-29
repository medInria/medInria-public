/* medViewContainerFiltering.cpp ---
 *
 * @author Clément Philipot <clement.philipot@inria.fr>
 *
 */

#include "medViewContainerFiltering.h"
#include "medAbstractView.h"

class medViewContainerFilteringPrivate
{
public:
//     medViewContainer* inputViewContainer;
//     medViewContainer* outputViewContainer;
    medViewContainerCustom* inputViewContainer;
    medViewContainerCustom* outputViewContainer;
    dtkAbstractData * inputData;
    medAbstractView * inputView;
    medAbstractView * outputView;
};

medViewContainerFiltering::medViewContainerFiltering(QWidget * parent):
        medViewContainerCustom(parent), d3(new medViewContainerFilteringPrivate)
{
    split(1, 2);

    d3->inputViewContainer = dynamic_cast<medViewContainerCustom*> (this->childContainers()[0]);
    d3->outputViewContainer = dynamic_cast<medViewContainerCustom*> (this->childContainers()[1]);

//     d3->inputViewContainer = this->childContainers()[0];
//     d3->outputViewContainer = this->childContainers()[1];

    //Set cues for the user:
    d3->inputViewContainer->setInfo(tr("Input to be filtered"));
    d3->outputViewContainer->setInfo(tr("Result of filtering"));

    d3->outputView = dynamic_cast<medAbstractView*> (dtkAbstractViewFactory::instance()->create("v3dView"));
    d3->outputViewContainer->setAcceptDrops(false);
    d3->outputViewContainer->update();

    connect(d3->inputViewContainer,SIGNAL(dropped(medDataIndex)), this, SLOT(updateInput(medDataIndex)));
    connect(d3->inputViewContainer,SIGNAL(dropped(medDataIndex)), this, SIGNAL(droppedInput(medDataIndex)));
}

medViewContainerFiltering::~medViewContainerFiltering()
{
    delete d3;
    d3 = NULL;
}

void medViewContainerFiltering::updateInput (const medDataIndex& index)
{
    if (!index.isValid())
        return;
    
    qDebug() << "update input";

    d3->inputData = medDataManager::instance()->data (index).data();

    if (!d3->inputData)
        return;

    d3->inputView =  dynamic_cast<medAbstractView*> (dtkAbstractViewFactory::instance()->create("v3dView"));
    d3->inputView->setData(d3->inputData,0);
    d3->inputView->reset();
    d3->inputView->update();

    d3->inputViewContainer->clear();
//     d3->inputViewContainer->close();
    d3->inputViewContainer = dynamic_cast<medViewContainerCustom*> (this->childContainers()[0]);
    d3->inputViewContainer->setInfo(tr("Input to be filtered"));
    d3->inputViewContainer->update();
    d3->inputViewContainer->setView(d3->inputView);
    d3->inputViewContainer->update();

    d3->outputViewContainer->clear();
//     d3->outputViewContainer->close();
    d3->outputViewContainer = dynamic_cast<medViewContainerCustom*> (this->childContainers()[1]);
    d3->outputViewContainer->setInfo(tr("Result of filtering"));
    d3->outputViewContainer->update();
}

void medViewContainerFiltering::updateOutput(dtkAbstractData *data)
{
    if(!data)
        return;

    d3->outputView =  dynamic_cast<medAbstractView*> (dtkAbstractViewFactory::instance()->create("v3dView"));
    d3->outputView->setData(data,0);
    d3->outputView->reset();
    d3->outputView->update();

    d3->outputViewContainer->setView(d3->outputView);
    d3->outputViewContainer->update();
}
