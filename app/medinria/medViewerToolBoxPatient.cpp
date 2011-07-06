/* medViewerToolBoxPatient.cpp ---
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:06:02 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Sun Jun 27 18:16:44 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 64
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewerToolBoxPatient.h"

#include <QtGui>

#include <dtkCore/dtkGlobal.h>
#include <dtkGui/dtkQSignalBlocker.h>

#include <medCore/medDataIndex.h>
#include <medCore/medDataManager.h>
#include <medCore/medMetaDataHelper.h>
#include <medCore/medAbstractDbController.h>
#include <medCore/medDbControllerFactory.h>

class medViewerToolBoxPatientPrivate
{
public:
    QHash< int, QSet<medDataIndex> > itemMap;
    QHash< medDataIndex, int >       indexMap;

    int nextItemId;
    QComboBox *combo;
    enum{ ChooserItemId = -1 };
};

medViewerToolBoxPatient::medViewerToolBoxPatient(QWidget *parent) : medToolBox(parent), d(new medViewerToolBoxPatientPrivate)
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

    connect(medDbControllerFactory::instance(), SIGNAL(dbControllerRegistered(const QString&)), this, SLOT(setupDatabase()));

    // Setting up database
    this->setupDatabase();

}

medViewerToolBoxPatient::~medViewerToolBoxPatient(void)
{
    delete d;

    d = NULL;
}

//! Add the chooser text item
int medViewerToolBoxPatient::addChooseItem()
{
    int itemId = d->ChooserItemId;
    d->combo->addItem(tr("Choose patient"), itemId);
    return itemId;
}

//! Remove the chooser text item
void medViewerToolBoxPatient::removeChooseItem()
{
    int itemId = d->ChooserItemId;
    d->combo->removeItem(d->combo->findData(itemId));
}

//! Add a patient item.
/*! 
 *  The patient is appended to the toolbox' combobox with text \param
 *  item. \param data corresponds to the patient's database id.
 */

int medViewerToolBoxPatient::addItem(const QString& item, const medDataIndex& data)
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

void medViewerToolBoxPatient::clear(void)
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

QSet<medDataIndex> medViewerToolBoxPatient::patientIndex(void) const
{
    int itemId = d->combo->itemData(d->combo->currentIndex()).toInt();

    return patientIndex(itemId);
}

//! Returns the database patient index of the currently displayed patient. 
/*! 
 *  Beware not to confuse the returned index with the one of the item
 *  in the combo box, that is not necessarily the same.
 */

QSet<medDataIndex> medViewerToolBoxPatient::patientIndex(QString patient) const
{
    int itemId = d->combo->itemData(d->combo->findText(patient)).toInt();
    return patientIndex(itemId);
}

QSet<medDataIndex> medViewerToolBoxPatient::patientIndex(int itemId) const
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

void medViewerToolBoxPatient::setPatientIndex(const medDataIndex &index)
{
    typedef QHash< medDataIndex, int  > IndexHashType;
    IndexHashType::const_iterator it = d->indexMap.find( index );
    if ( it != d->indexMap.end() ) {
        int itemId =it.value();
        d->combo->setCurrentIndex(d->combo->findData(itemId));
    }
}

//! Combo box index changed slot.
/*! 
 *  Emits the patientIndexChanged signal with argument the index of
 *  the newly displayed patient in the database.
 */

void medViewerToolBoxPatient::onCurrentIndexChanged(int index)
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

void medViewerToolBoxPatient::setupDatabase(void)
{
    dtkQSignalBlocker( d->combo );
    
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

        QList<int> patientIds = dbc->patients();
        for (QList<int>::const_iterator patientIt( patientIds.begin()); patientIt != patientIds.end(); ++patientIt ) {

            medDataIndex patientIndex( *dataSourceIt, *patientIt );
            QString patientName = dbc->metaData(patientIndex, medMetaDataHelper::KEY_PatientName() );

            bool isFirstSource = dataSourceIt == dataSourceIds.begin();
            bool matchesExisting(false);
            if ( !isFirstSource ) {
                matchesExisting = patientList.contains(patientName);
            }
            if ( matchesExisting ) {
                int itemId = patientList[patientName];
                d->itemMap.find(itemId).value().insert(patientIndex);
                d->indexMap[patientIndex] = itemId;
            } else {
                int itemId = this->addItem(patientName, patientIndex );
                patientList[patientName] = itemId;
            }
        }
    }

    for (QList<int>::const_iterator dataSourceIt( dataSourceIds.begin()); dataSourceIt != dataSourceIds.end(); ++dataSourceIt ) {
        medAbstractDbController *dbc = dataManager->controllerForDataSource(*dataSourceIt);

        if ( !dbc ) 
            continue;

        connect(dbc, SIGNAL(updated(medDataIndex)), this, SLOT(setupDatabase()));
    }
    // d->combo->blockSignals (false);  // automatic.
}
