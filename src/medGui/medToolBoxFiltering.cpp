/* medToolBoxFiltering.cpp --- 
 * 
 */

//TODO save result in a file or database
//TODO launch process in a thread to prevent freezing the user interface

#include "medToolBoxFiltering.h"
#include "medToolBoxFilteringCustom.h"

#include <medGui/medToolBoxFactory.h>
#include <medGui/medDropSite.h>

#include <medCore/medDataManager.h>
#include <medSql/medDatabaseController.h>

#include <dtkCore/dtkAbstractData.h>



class medToolBoxFilteringPrivate
{
public:
	QComboBox    *chooseFilter;
	QPushButton * saveResultButton;
	medToolBoxFilteringCustom *customToolBox;
	medDropSite *dropSite;
	dtkAbstractData *data;
};


medToolBoxFiltering::medToolBoxFiltering(QWidget *parent) : medToolBox(parent), d(new medToolBoxFilteringPrivate)
{

    d->dropSite = new medDropSite(this);

    QWidget *displayWidget = new QWidget(this);
    
    d->saveResultButton = new QPushButton(tr("Save Result"),this);
//    connect (d->saveResultButton, SIGNAL(clicked()), this, SLOT(onSaveImage()));

    d->chooseFilter = new QComboBox(this);
    d->chooseFilter->addItem("Choose filter");
    
    QVBoxLayout *filterLayout = new QVBoxLayout(displayWidget);
    filterLayout->addWidget(d->dropSite);
    filterLayout->addWidget(d->saveResultButton);
    filterLayout->addWidget(d->chooseFilter);
    filterLayout->setAlignment(d->dropSite,Qt::AlignHCenter);
    
    foreach(QString toolbox, medToolBoxFactory::instance()->filteringToolBoxes())
        d->chooseFilter->addItem(toolbox, toolbox);

    connect(d->chooseFilter, SIGNAL(activated(const QString&)), this, SLOT(onToolBoxChosen(const QString&)));
    connect(d->dropSite,SIGNAL(objectDropped()),this,SLOT(onObjectDropped()));
//    connect(this, SIGNAL(dataSelected(dtkAbstractData *)),d->customToolBox,SLOT(setInputData(dtkAbstractData *)));
    
    // Layout section :

    QWidget * layoutSection = new QWidget(this);
    layoutSection->setLayout(filterLayout);

    this->setTitle("Filtering View");
    this->addWidget(layoutSection);
    
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

    connect(d->customToolBox,SIGNAL(success()),this,SIGNAL(processFinished()));
}


void medToolBoxFiltering::onObjectDropped(void)
{
  medDataIndex index = d->dropSite->index();
  
  if (!index.isValid())
    return;
  
  d->data = medDataManager::instance()->data (index).data();
  
  qDebug() << "onObjectDropped(), d->data->data() = " << d->data->data();

//  if (d->data)
//    medToolBoxFilteringCustom* customToolbox(void);
  
  if (!d->data)
    return;
  
	emit dataSelected(d->data);	
}

// TO DO : to complete
void medToolBoxFiltering::clear(void)
{
    //maybe clear the customtoolbox?
    if (d->customToolBox)
        d->customToolBox->clear();
}




















medToolBoxFilteringCustom* customToolbox(void);











