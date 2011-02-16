/* medToolBoxFiltering.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 16:36:24 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 47
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medToolBoxFiltering.h"
#include "medToolBoxFilteringCustom.h"

#include <medGui/medToolBoxFactory.h>
#include <medGui/medDropSite.h>
#include <medGui/medViewContainerStack.h>
#include <medGui/medViewerConfigurationFiltering.h>

#include <medCore/medDataManager.h>
#include <medSql/medDatabaseController.h>

#include <dtkCore/dtkAbstractData.h>



class medToolBoxFilteringPrivate
{
public:
	QComboBox    *chooseFilter;
	medToolBoxFilteringCustom *customToolBox;
	medDropSite *dropSite;
	dtkAbstractData *data;
};


medToolBoxFiltering::medToolBoxFiltering(QWidget *parent) : medToolBox(parent), d(new medToolBoxFilteringPrivate)
{

    d->dropSite = new medDropSite(this);

    QWidget *displayWidget = new QWidget(this);
    
    d->chooseFilter = new QComboBox(this);
    d->chooseFilter->addItem("Choose...");  
    
    QLabel *filterLabel = new QLabel("Filters :", displayWidget);    
    
    QVBoxLayout *filterLayout = new QVBoxLayout(displayWidget);
    filterLayout->addWidget(d->dropSite);
    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(d->chooseFilter);
    filterLayout->setAlignment(d->dropSite,Qt::AlignHCenter);
    
    foreach(QString toolbox, medToolBoxFactory::instance()->filteringToolBoxes())
        d->chooseFilter->addItem(toolbox, toolbox);

    connect(d->chooseFilter, SIGNAL(activated(const QString&)), this, SLOT(onToolBoxChosen(const QString&)));
    connect(d->dropSite,SIGNAL(objectDropped()),this,SLOT(onObjectDropped()));
    
    this->setTitle("Filtering View");
    this->addWidget(displayWidget);
    
    d->customToolBox = NULL;
    d->data = NULL;
}

medToolBoxFiltering::~medToolBoxFiltering()
{
    delete d;
    d = NULL;
}

medToolBoxFilteringCustom* medToolBoxFiltering::customToolbox(void)
{
        return d->customToolBox;
}

dtkAbstractData*  medToolBoxFiltering::data()
{
        return d->data;
}


// void medToolBoxFiltering::update (dtkAbstractView *view)
// {
// }


void medToolBoxFiltering::onToolBoxChosen(const QString& id)
{
    medToolBoxFilteringCustom *toolbox = medToolBoxFactory::instance()->createCustomFilteringToolBox(id);

    if(!toolbox) {
        qDebug() << "Unable to instanciate" << id << "toolbox";
        return;
    }

    toolbox->setFilteringToolBox(this);
    //get rid of old toolBox
    if (d->customToolBox)
    {
        emit removeToolBox(d->customToolBox);
        delete d->customToolBox;
    }
    d->customToolBox = toolbox;
    emit addToolBox(toolbox);

    connect(toolbox,SIGNAL(success()),this,SIGNAL(processFinished()));
}


void medToolBoxFiltering::onObjectDropped(void)
{
    medDataIndex index = d->dropSite->index();

    if (!index.isValid())
        return;

    d->data = medDataManager::instance()->data (index);
    if (!d->data) {
        d->data = medDatabaseController::instance()->read(index);
        if (d->data)    medToolBoxFilteringCustom* customToolbox(void);
            medDataManager::instance()->insert(index, d->data);
    }

    if (!d->data)
        return;

}

// TO DO : to complete
void medToolBoxFiltering::clear(void)
{
    //maybe clear the customtoolbox?
    if (d->customToolBox)
        d->customToolBox->clear();
}




















medToolBoxFilteringCustom* customToolbox(void);











