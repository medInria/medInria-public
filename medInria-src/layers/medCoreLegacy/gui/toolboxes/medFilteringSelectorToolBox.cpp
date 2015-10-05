/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medFilteringSelectorToolBox.h>
#include <medFilteringAbstractToolBox.h>

#include <medToolBoxFactory.h>
#include <medDropSite.h>
#include <medToolBoxHeader.h>

#include <medDataManager.h>
#include <medAbstractView.h>

#include <medAbstractData.h>



class medFilteringSelectorToolBoxPrivate
{
public:

    QComboBox    *chooseFilter;
    medAbstractData *inputData;
    QHash<QString, medFilteringAbstractToolBox*> toolBoxes;
    QVBoxLayout *filterLayout;
    medFilteringAbstractToolBox *currentToolBox;

};

medFilteringSelectorToolBox::medFilteringSelectorToolBox ( QWidget *parent ) :
    medToolBox ( parent ),
    d ( new medFilteringSelectorToolBoxPrivate )
{

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

    connect (d->chooseFilter, SIGNAL(activated(int)), this, SLOT(changeCurrentToolBox(int)));

    QWidget *displayWidget = new QWidget;
    d->filterLayout = new QVBoxLayout;

    d->chooseFilter->adjustSize();
    d->filterLayout->addWidget ( d->chooseFilter );
    displayWidget->setLayout(d->filterLayout);

    this->setTitle (tr("Filtering View"));
    this->addWidget(displayWidget);

    d->inputData = NULL;
    d->currentToolBox = NULL;
}

medFilteringSelectorToolBox::~medFilteringSelectorToolBox()
{
    delete d;
    d = NULL;
}

/**
 * @brief returns current selected toolbox
 */
medFilteringAbstractToolBox* medFilteringSelectorToolBox::currentToolBox()
{
    return d->currentToolBox;
}

/**
 * @brief returns input data
 */
medAbstractData*  medFilteringSelectorToolBox::data()
{
    return d->inputData;
}

/**
 * @brief instantiates the right process toolbox according to its description
 */
void medFilteringSelectorToolBox::changeCurrentToolBox ( int index )
{    
    medFilteringAbstractToolBox *toolbox = NULL;
    //get identifier for toolbox.
    QString identifier = d->chooseFilter->itemData(index).toString();
    if (d->toolBoxes.contains (identifier))
        toolbox = d->toolBoxes[identifier];
    else
    {
        medToolBox* tb = medToolBoxFactory::instance()->createToolBox(
                    identifier, this);
        toolbox = qobject_cast<medFilteringAbstractToolBox *>(tb);
        if(toolbox)
        {
            toolbox->setStyleSheet("medToolBoxBody {border:none}");
            d->toolBoxes[identifier] = toolbox;
        }
    }

    if(!toolbox)
    {
        if (d->currentToolBox)
        {
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
        d->filterLayout->removeWidget ( d->currentToolBox );
        d->currentToolBox = NULL;
    }

    d->currentToolBox = toolbox;
    d->currentToolBox->header()->hide();

    dtkPlugin* plugin = d->currentToolBox->plugin();
    this->setAboutPluginButton(plugin);
    this->setAboutPluginVisibility(true);


    d->currentToolBox->show();
    d->filterLayout->addWidget(toolbox);
    d->currentToolBox->update(d->inputData);
    connect ( d->currentToolBox, SIGNAL(success()), this, SIGNAL(processFinished()));
}

/**
 * @brief retrieve data from the selected input image index
 * @param index
 */
void medFilteringSelectorToolBox::onInputSelected(medAbstractData *data)
{   
    d->inputData = data;

    if (d->inputData != NULL && !d->inputData->dataIndex().isValidForSeries())
        d->inputData = NULL;

    if(d->currentToolBox)
        d->currentToolBox->update(d->inputData);
}

/**
 * @brief clear input data and the current process toolbox
 */
void medFilteringSelectorToolBox::clear()
{
    d->inputData = NULL;

    if(d->currentToolBox)
        d->currentToolBox->update(d->inputData);

}



