/* medToolBoxFiltering.cpp ---
 *
 * @author Clément Philipot <clement.philipot@inria.fr>
 *
 */

#include "medToolBoxFiltering.h"
#include "medToolBoxFilteringCustom.h"

#include <medToolBoxFactory.h>
#include <medDropSite.h>

#include <medCore/medDataManager.h>
#include <medViewManager.h>
#include <medAbstractView.h>

#include <dtkCore/dtkAbstractData.h>



class medToolBoxFilteringPrivate
{
public:
    QComboBox    *chooseFilter;
    QPushButton *saveInDatabaseButton;
//    QPushButton *saveToDiskButton;
    medToolBoxFilteringCustom *customToolBox;
    medAbstractView *inputView;
    dtkAbstractData *inputData;
    medDataIndex index;
};

medToolBoxFiltering::medToolBoxFiltering ( QWidget *parent ) : medToolBox ( parent ), d ( new medToolBoxFilteringPrivate )
{
    d->inputView = NULL;

    QWidget *displayWidget = new QWidget ( this );

    d->saveInDatabaseButton = new QPushButton ( tr ( "Store in Database" ),this );
    d->saveInDatabaseButton->setFocusPolicy(Qt::NoFocus);

//    d->saveToDiskButton = new QPushButton(tr("Save to Disk"),this);

    d->chooseFilter = new QComboBox ( this );
    d->chooseFilter->addItem ( tr ( "Choose filter" ) );

    QVBoxLayout *filterLayout = new QVBoxLayout ( displayWidget );
    filterLayout->addWidget ( d->saveInDatabaseButton );
//    filterLayout->addWidget(d->saveToDiskButton);
    filterLayout->addWidget ( d->chooseFilter );

    medToolBoxFactory* tbFactory = medToolBoxFactory::instance();
    int i = 1; //account for the choose Filter item
    foreach ( QString toolbox, tbFactory->filteringToolBoxes() )
    {
        QPair<QString, QString> pair =
                tbFactory->filteringToolBoxDetailsFromId( toolbox );
        QString name = pair.first;
        QString description = pair.second;
//        qDebug() << "Added registration toolbox" << name;
        d->chooseFilter->addItem( name, toolbox );
        d->chooseFilter->setItemData( i,
                                  description,
                                  Qt::ToolTipRole);
        i++;
    }

    connect ( d->chooseFilter, SIGNAL ( activated ( int ) ), this, SLOT ( onToolBoxChosen ( int ) ) );
    connect ( d->saveInDatabaseButton,SIGNAL ( clicked() ), this, SLOT ( onSavedImage() ) );


    this->setTitle ( tr ( "Filtering View" ) );
    this->addWidget ( displayWidget );

    d->customToolBox = NULL;
    d->inputData = NULL;
}

medToolBoxFiltering::~medToolBoxFiltering()
{
    delete d;
    d = NULL;
}

medToolBoxFilteringCustom* medToolBoxFiltering::customToolbox ( void )
{

    return d->customToolBox;
}

dtkAbstractData*  medToolBoxFiltering::data()
{
    return d->inputData;
}

void medToolBoxFiltering::onToolBoxChosen ( int index )
{
    //get identifier for toolbox.
    QString id = d->chooseFilter->itemData( index ).toString();

    medToolBoxFilteringCustom *toolbox = medToolBoxFactory::instance()->createCustomFilteringToolBox ( id );

    if ( !toolbox )
    {
        qWarning() << "Unable to instantiate" << id << "toolbox";
        return;
    }

    toolbox->setFilteringToolBox ( this );
    //get rid of old toolBox
    if ( d->customToolBox )
    {
        emit removeToolBox ( d->customToolBox );
        delete d->customToolBox;
    }
    d->customToolBox = toolbox;
    toolbox->show();
    emit addToolBox ( toolbox );

    if ( d->inputView )
        d->customToolBox->update ( d->inputView );

    connect ( d->customToolBox,SIGNAL ( success() ),this,SIGNAL ( processFinished() ) );
}


void medToolBoxFiltering::onInputSelected ( const medDataIndex& index )
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
        if ( d->customToolBox )
            d->customToolBox->update ( d->inputView );
    }
}

void medToolBoxFiltering::clear ( void )
{
    if ( d->customToolBox )
        d->customToolBox->clear();

    d->inputData = NULL;
    d->inputView = NULL;
    d->index = medDataIndex();
}


void medToolBoxFiltering::setDataIndex ( medDataIndex index )
{
    d->index = index;
}

void medToolBoxFiltering::onSavedImage ( void )
{
    medDataManager::instance()->storeNonPersistentSingleDataToDatabase ( d->index );
}
