/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medFilteringSelectorToolBox.h"
#include "medFilteringAbstractToolBox.h"

#include <medToolBoxFactory.h>
#include <medDropSite.h>
#include <medToolBoxHeader.h>

#include <medCore/medDataManager.h>
#include <medViewManager.h>
#include <medAbstractView.h>

#include <dtkCore/dtkAbstractData.h>



class medFilteringSelectorToolBoxPrivate
{
public:

    QComboBox    *chooseFilter;
    medAbstractView *inputView;
    dtkAbstractData *inputData;
    medDataIndex index;
    QHash<QString, medFilteringAbstractToolBox*> toolBoxes;
    QVBoxLayout *filterLayout;
    medFilteringAbstractToolBox *currentToolBox;

};

medFilteringSelectorToolBox::medFilteringSelectorToolBox ( QWidget *parent ) :
    medToolBox ( parent ),
    d ( new medFilteringSelectorToolBoxPrivate )
{
    d->inputView = NULL;


    d->chooseFilter = new QComboBox;
    d->chooseFilter->addItem ( tr ( "Choose filter" ) );
	d->chooseFilter->setToolTip(tr("Browse through the list of available filters"));

    medToolBoxFactory* tbFactory = medToolBoxFactory::instance();
    int i = 1; //account for the choose Filter item
    foreach ( QString toolbox, tbFactory->toolBoxesFromCategory("filtering") )
    {
        medToolBoxDetails* details = tbFactory->toolBoxDetailsFromId(toolbox);

        d->chooseFilter->addItem( details->name, toolbox );
        d->chooseFilter->setItemData( i,
                                  details->description,
                                  Qt::ToolTipRole);
        i++;
    }

    connect ( d->chooseFilter, SIGNAL ( activated ( int ) ), this, SLOT ( onToolBoxChosen ( int ) ) );

    QWidget *displayWidget = new QWidget;
    d->filterLayout = new QVBoxLayout;
    d->filterLayout->addWidget ( d->chooseFilter );
    displayWidget->setLayout(d->filterLayout);

    this->setTitle ( tr ( "Filtering View" ) );
    this->addWidget ( displayWidget );
    displayWidget->setStyleSheet("QListView {border:none}");

    d->inputData = NULL;
    d->currentToolBox = NULL;
}

medFilteringSelectorToolBox::~medFilteringSelectorToolBox()
{
    delete d;
    d = NULL;
}

medFilteringAbstractToolBox* medFilteringSelectorToolBox::currentToolBox()
{

    return d->currentToolBox;
}

dtkAbstractData*  medFilteringSelectorToolBox::data()
{
    return d->inputData;
}

void medFilteringSelectorToolBox::onToolBoxChosen ( int index )
{    
    medFilteringAbstractToolBox *toolbox = NULL;
    //get identifier for toolbox.
    QString identifier = d->chooseFilter->itemData(index).toString();
    if (d->toolBoxes.contains (identifier))
        toolbox = d->toolBoxes[identifier];
    else {
        medToolBox* tb = medToolBoxFactory::instance()->createToolBox(
                    identifier, this);
        toolbox = qobject_cast<medFilteringAbstractToolBox *>(tb);
        if (toolbox) {
            toolbox->setStyleSheet("medToolBoxBody {border:none}");
            d->toolBoxes[identifier] = toolbox;
        }
    }

    if(!toolbox) {
        if (d->currentToolBox) {
            d->currentToolBox->hide();
            d->filterLayout->removeWidget ( d->currentToolBox );
            d->currentToolBox = 0;
            this->setAboutPluginVisibility(false);
        }
        return;
    }

    toolbox->setFilteringToolBox(this);

    //get rid of old toolBox
    if (d->currentToolBox)
    {
        d->currentToolBox->hide();
        emit removeToolBox ( d->currentToolBox );
        d->filterLayout->removeWidget ( d->currentToolBox );
        d->currentToolBox = NULL;

    }

    d->currentToolBox = toolbox;
    d->currentToolBox->header()->hide();

    dtkPlugin* plugin = d->currentToolBox->plugin();
    this->setAboutPluginButton(plugin);
    this->setAboutPluginVisibility(true);


    d->currentToolBox->show();

    d->filterLayout->addWidget ( toolbox );

    if ( d->inputView )
        d->currentToolBox->update ( d->inputView );

    connect ( d->currentToolBox,SIGNAL ( success() ),this,SIGNAL ( processFinished() ) );
}


void medFilteringSelectorToolBox::onInputSelected ( const medDataIndex& index )
{
    if ( !index.isValid() )
        return;

    d->inputData = medDataManager::instance()->data ( index ).data();

    if ( !d->inputData )
        return;

    d->inputView = dynamic_cast<medAbstractView*> ( medViewManager::instance()->views ( index ).first() );

    if ( !d->inputView )
    {
        qDebug() << "Unable to retrieve input view";
        return;
    }
    else
    {
        if ( d->currentToolBox )
            d->currentToolBox->update ( d->inputView );
    }
}

void medFilteringSelectorToolBox::clear()
{
    foreach (medFilteringAbstractToolBox *tb, d->toolBoxes)
        tb->deleteLater();

    d->toolBoxes.clear();

    d->currentToolBox = NULL;

    d->chooseFilter->blockSignals (true);
    d->chooseFilter->setCurrentIndex (0);
    d->chooseFilter->blockSignals (false);

    d->inputData = NULL;
    d->inputView = NULL;
    d->index = medDataIndex();
}


void medFilteringSelectorToolBox::setDataIndex ( medDataIndex index )
{
    d->index = index;
}

