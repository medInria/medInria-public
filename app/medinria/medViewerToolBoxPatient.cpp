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
#include <QSqlQuery>

#include <dtkCore/dtkGlobal.h>

#include <medCore/medDataIndex.h>
#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseNonPersistentController.h>
#include <medSql/medDatabaseNonPersistentItem.h>

class medViewerToolBoxPatientPrivate
{
public:
    QComboBox *combo;
};

medViewerToolBoxPatient::medViewerToolBoxPatient(QWidget *parent) : medToolBox(parent), d(new medViewerToolBoxPatientPrivate)
{
    QWidget *central = new QWidget(this);

    d->combo = new QComboBox(central);
    d->combo->setFocusPolicy(Qt::NoFocus);

    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->addWidget(d->combo);

    this->setTitle("Patient");
    this->addWidget(central);

    connect(d->combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));

    // Setting up database

    this->setupDatabase();

    connect(medDatabaseController::instance(), SIGNAL(updated(medDataIndex)), this, SLOT(setupDatabase()));
    connect(medDatabaseNonPersistentController::instance(), SIGNAL(updated(medDataIndex)), this, SLOT(setupDatabase()));


}

medViewerToolBoxPatient::~medViewerToolBoxPatient(void)
{
    delete d;

    d = NULL;
}

//! Add a patient item.
/*! 
 *  The patient is appended to the toolbox' combobox with text \param
 *  item. A non persistent id is assigned to the generate item.
 */

void medViewerToolBoxPatient::addItem(const QString& item)
{
    static int data = 99999;

    d->combo->addItem(item, data++);
}

//! Add a patient item.
/*! 
 *  The patient is appended to the toolbox' combobox with text \param
 *  item. \param data corresponds to the patient's database id.
 */

void medViewerToolBoxPatient::addItem(const QString& item, const QVariant& data)
{
    d->combo->addItem(item, data);
}

//! Clears the list of patient.
/*! 
 * 
 */

void medViewerToolBoxPatient::clear(void)
{
    d->combo->clear();
}

//! Returns the database patient index of the currently displayed patient. 
/*! 
 *  Beware not to confuse the returned index with the one of the item
 *  in the combo box, that is not necessarily the same.
 */

int medViewerToolBoxPatient::patientIndex(void)
{
    return d->combo->itemData(d->combo->currentIndex()).toInt();
}

//! Returns the database patient index of the currently displayed patient. 
/*! 
 *  Beware not to confuse the returned index with the one of the item
 *  in the combo box, that is not necessarily the same.
 */

int medViewerToolBoxPatient::patientIndex(QString patient)
{
    return d->combo->itemData(d->combo->findText(patient)).toInt();
}

//! Sets the currently displayed patient. 
/*! 
 *  \param patientId is the index of a patient in the database.
 */

void medViewerToolBoxPatient::setPatientIndex(int patientId)
{
    d->combo->setCurrentIndex(d->combo->findData(patientId));
}

//! Sets the currently displayed patient.
/*! 
 * 
 * \param index is the index of a data in the database.
 */

void medViewerToolBoxPatient::setPatientIndex(const medDataIndex& index)
{
    d->combo->setCurrentIndex(d->combo->findData(index.patientId()));
}

//! Combo box index changed slot.
/*! 
 *  Emits the patientIndexChanged signal with argument the index of
 *  the newly displayed patient in the database.
 */

void medViewerToolBoxPatient::onCurrentIndexChanged(int index)
{
    emit patientIndexChanged(d->combo->itemData(index).toInt());
}


//! Query the db for patients.
/*!
 *  This methods sets the patient toolbox up by retreiving the list of
 *  partients from the database, add items using their names, and the
 *  database patient id as a user data for each item. Beware, the
 *  index of the patient in the combo box does not necessarily
 *  corresponds to the one of the patient in the database.
 */

void medViewerToolBoxPatient::setupDatabase(void)
{
    d->combo->blockSignals (true);
    
    this->clear();
    this->addItem("Choose patient", -1);

    QList<QString> patientList;
    
    // Setting up persitent data

    QSqlQuery query(*(medDatabaseController::instance()->database()));

    query.prepare("SELECT name, id FROM patient");
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    while(query.next()) {
        this->addItem(query.value(0).toString(), query.value(1));
	patientList.append (query.value(0).toString());
    }

    // Setting up non persitent data

    foreach(medDatabaseNonPersistentItem *item, medDatabaseNonPersistentController::instance()->items()) {
        if (!patientList.contains (item->name())) {
            this->addItem(item->name(), item->index().patientId());
            patientList.append (item->name());
        }
    }

    d->combo->blockSignals (false);
}
