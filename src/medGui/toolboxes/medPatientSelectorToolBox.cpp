/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medPatientSelectorToolBox.h"

#include <QtGui>

#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkSignalBlocker.h>

#include <medDataIndex.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <medAbstractDbController.h>
#include <medDbControllerFactory.h>

class medPatientSelectorToolBoxPrivate
{
public:
    QHash< int, QSet<medDataIndex> > itemMap;
    QHash< medDataIndex, int >       indexMap;

    int nextItemId;
    QComboBox *combo;
    enum{ ChooserItemId = -1 };
};

medPatientSelectorToolBox::medPatientSelectorToolBox(QWidget *parent) : medToolBox(parent), d(new medPatientSelectorToolBoxPrivate)
{
    QWidget *central = new QWidget(this);
    d->nextItemId = 0;

    d->combo = new QComboBox(central);
    d->combo->setFocusPolicy(Qt::NoFocus);

    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->addWidget(d->combo);

    this->setTitle("Patient");
    this->addWidget(central);

    connect(d->combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));

    medDbControllerFactory* dbFactory = medDbControllerFactory::instance();
    connect(dbFactory, SIGNAL(dbControllerRegistered(const QString&)), this, SLOT(onDbControllerRegistered(const QString&)));

    //connect all existing db controllers, also calls setupDatabase if any dbController
    onDbControllerRegistered(QString());

}

medPatientSelectorToolBox::~medPatientSelectorToolBox(void)
{
    delete d;

    d = NULL;
}

//! Add the chooser text item
int medPatientSelectorToolBox::addChooseItem()
{
    int itemId = d->ChooserItemId;
    d->combo->addItem(tr("Choose patient"), itemId);
	d->combo->setToolTip(tr("Choose a patient from the database"));
    return itemId;
}

//! Remove the chooser text item
void medPatientSelectorToolBox::removeChooseItem()
{
    int itemId = d->ChooserItemId;
    d->combo->removeItem(d->combo->findData(itemId));
}

//! Add a patient item.
/*!
 *  The patient is appended to the toolbox' combobox with text \param
 *  item. \param data corresponds to the patient's database id.
 */

int medPatientSelectorToolBox::addItem(const QString& item, const medDataIndex& data)
{
    if ( d->indexMap.contains(data) )
        return d->indexMap.find(data).value();

    int itemId = d->nextItemId++;
    d->itemMap[itemId].insert( data );
    d->indexMap[data] = itemId;

    QString itemText = item;
    if ( itemText.isEmpty() ) {
        itemText = QString(tr("Patient %1.%2")).arg( data.dataSourceId() ).arg( data.patientId() );
    }
    d->combo->addItem(itemText, itemId);
    return itemId;
}

//! Clears the list of patient.
/*!
 *
 */

void medPatientSelectorToolBox::clear(void)
{
    d->nextItemId = 0;
    d->itemMap.clear();
    d->indexMap.clear();
    d->combo->clear();
}

//! Returns the database patient index of the currently displayed patient.
/*!
 *  Beware not to confuse the returned index with the one of the item
 *  in the combo box, that is not necessarily the same.
 */

QSet<medDataIndex> medPatientSelectorToolBox::patientIndex(void) const
{
    int itemId = d->combo->itemData(d->combo->currentIndex()).toInt();

    return patientIndex(itemId);
}

//! Returns the database patient index of the currently displayed patient.
/*!
 *  Beware not to confuse the returned index with the one of the item
 *  in the combo box, that is not necessarily the same.
 */

QSet<medDataIndex> medPatientSelectorToolBox::patientIndex(QString patient) const
{
    int itemId = d->combo->itemData(d->combo->findText(patient)).toInt();
    return patientIndex(itemId);
}

QSet<medDataIndex> medPatientSelectorToolBox::patientIndex(int itemId) const
{
    typedef QHash< int, QSet<medDataIndex> > HashType;
    HashType::const_iterator it = d->itemMap.find(itemId);
    if ( it == d->itemMap.end() ) {
        return QSet<medDataIndex>();
    }
    return d->itemMap.find(itemId).value();
}

//! Sets the currently displayed patient.
/*!
 *  \param index is the index of a patient in the database.
 */

void medPatientSelectorToolBox::setPatientIndex(const medDataIndex &index)
{
    medDataIndex baseIndex = index;
    baseIndex.setStudyId(-1);
    baseIndex.setSeriesId(-1);
    baseIndex.setImageId(-1);

    typedef QHash< medDataIndex, int  > IndexHashType;
    QList<int> dataSources = medDataManager::instance()->dataSourceIds();
    
    foreach (const int dataSourceId, dataSources )
    {
        baseIndex.setDataSourceId(dataSourceId);
        IndexHashType::const_iterator it = d->indexMap.find( baseIndex );
        if ( it != d->indexMap.end() )
        {
            int itemId =it.value();
            d->combo->setCurrentIndex(d->combo->findData(itemId));
            break;
        }
    }
}

//! Combo box index changed slot.
/*!
 *  Emits the patientIndexChanged signal with argument the index of
 *  the newly displayed patient in the database.
 */

void medPatientSelectorToolBox::onCurrentIndexChanged(int index)
{
    int itemId = d->combo->itemData(index).toInt();
    if ( itemId != d->ChooserItemId ) {
        this->removeChooseItem();
    }
    const QSet<medDataIndex> indexes = this->patientIndex(itemId);
    medDataIndex emittedIndex;
    if ( !indexes.empty() ) {
        emittedIndex = *(indexes.begin());
    }
    emit patientIndexChanged(  emittedIndex );
}


//! Query the db for patients.
/*!
 *  This methods sets the patient toolbox up by retrieving the list of
 *  patients from the database, add items using their names, and the
 *  database patient id as a user data for each item. Beware, the
 *  index of the patient in the combo box does not necessarily
 *  corresponds to the one of the patient in the database.
 */

void medPatientSelectorToolBox::setupDatabase(void)
{
    dtkSignalBlocker( d->combo );

    this->clear();
    this->addChooseItem();

    QHash<QString, int> patientList;

    // Setting up persistent data
    medDataManager * dataManager = medDataManager::instance();
    QList<int> dataSourceIds = dataManager->dataSourceIds();


    for (QList<int>::const_iterator dataSourceIt( dataSourceIds.begin()); dataSourceIt != dataSourceIds.end(); ++dataSourceIt ) {

        medAbstractDbController *dbc = dataManager->controllerForDataSource(*dataSourceIt);

        if ( !dbc )
            continue;

        QList<medDataIndex> patientsForSource = dbc->patients();
        for (QList<medDataIndex>::const_iterator patientIt( patientsForSource.begin()); patientIt != patientsForSource.end(); ++patientIt ) {

            QString patientName = dbc->metaData((*patientIt),medMetaDataKeys::PatientName);

            bool isFirstSource = dataSourceIt == dataSourceIds.begin();
            bool matchesExisting(false);
            if ( !isFirstSource ) {
                matchesExisting = patientList.contains(patientName);
            }
            if ( matchesExisting ) {
                int itemId = patientList[patientName];
                d->itemMap.find(itemId).value().insert((*patientIt));
                d->indexMap[(*patientIt)] = itemId;
            } else {
                int itemId = this->addItem(patientName, (*patientIt) );
                patientList[patientName] = itemId;
            }
        }
    }

    // d->combo->blockSignals (false);  // automatic.
}

void medPatientSelectorToolBox::onDbControllerRegistered( const QString& )
{
    // Connections are moved to medWorkspaceArea for now, the reason is that otherwise setupDatabase is called too late
    // This method stays only to call the first setupDatabase method

    this->setupDatabase();
}
