/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTestWorkspace.h>


#include <medDatabaseNonPersistentController.h>
#include <medMetaDataKeys.h>
#include <medStorage.h>
#include <medToolBoxFactory.h>
#include <medViewContainer.h>
#include <medTabbedViewContainers.h>
#include <medAbstractData.h>
#include <medDataManager.h>
#include <medAbstractView.h>
#include <medAbstractLayeredView.h>
#include <medViewParameterGroup.h>
#include <medLayerParameterGroup.h>

#include <medAbstractSubstractImageProcess.h>
#include <medAbstractSubstractImageProcessPresenter.h>
#include <medProcessLayer.h>

class medTestWorkspacePrivate
{
public:
    medViewContainer *inputContainer1;
    medViewContainer *inputContainer2;
    medViewContainer *outputContainer;

};

medTestWorkspace::medTestWorkspace(QWidget *parent): medAbstractWorkspace (parent), d(new medTestWorkspacePrivate)
{

}

medTestWorkspace::~medTestWorkspace()
{
    delete d;
    d = NULL;
}

/**
 * @brief sets up all the signal/slot connections when Viewer is switched to this workspace
 */
void medTestWorkspace::setupViewContainerStack()
{
    if ( !this->stackedViewContainers()->count() )
    {
        d->inputContainer1 = this->stackedViewContainers()->addContainerInTab(this->name());
        QLabel *label1 = new QLabel("Input 1");
        label1->setAlignment(Qt::AlignCenter);
        d->inputContainer1->setDefaultWidget(label1);

        d->inputContainer1->setClosingMode(medViewContainer::CLOSE_VIEW);
        d->inputContainer1->setUserSplittable(false);
        d->inputContainer1->setMultiLayered(false);

        d->outputContainer = d->inputContainer1->splitVertically();
        QLabel *outputLabel = new QLabel("Result");
        label1->setAlignment(Qt::AlignCenter);
        d->outputContainer->setDefaultWidget(outputLabel);

        d->outputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
        d->outputContainer->setUserSplittable(false);
        d->outputContainer->setMultiLayered(false);


        d->inputContainer2 = d->inputContainer1->splitHorizontally();
        QLabel *label2 = new QLabel("Input 2");
        label1->setAlignment(Qt::AlignCenter);

        medAbstractSubstractImageProcessPresenter *p = medProcessLayer::substractImage::pluginFactory().create("medItkSubstractImageProcess");
        d->inputContainer2->setDefaultWidget(p->toolbox());

        d->inputContainer2->setClosingMode(medViewContainer::CLOSE_VIEW);
        d->inputContainer2->setUserSplittable(false);
        d->inputContainer2->setMultiLayered(false);

        this->stackedViewContainers()->lockTabs();
        this->stackedViewContainers()->hideTabBar();

        d->inputContainer1->setSelected(true);
        d->inputContainer2->setSelected(false);
        d->outputContainer->setSelected(false);

        this->stackedViewContainers()->lockTabs();
        this->stackedViewContainers()->hideTabBar();
    }
}

bool medTestWorkspace::isUsable()
{
    return true;
}

