/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseNavigator.h"

#include <dtkCore/dtkGlobal.h>

#include <QtCore>
#include <QtGui>

#include <medCore/medAbstractDbController.h>
#include <medCore/medDataManager.h>
#include <medCore/medMetaDataKeys.h>
#include <medCore/medStorage.h>

#include "medDatabaseNavigatorController.h"
#include "medDatabaseNavigatorItem.h"
#include "medDatabaseNavigatorItemGroup.h"
#include "medDatabaseNavigatorScene.h"
#include "medDatabaseNavigatorView.h"


namespace  {

    // These small classes are used to determine if patients from different DBs are the same.
    // At present it is just based on the name.
    struct PatientDataKey{
        QString name;
        bool operator==(const PatientDataKey & other) const { return this->name == other.name; }
        bool operator!=(const PatientDataKey & other) const { return !this->operator==(other); }
    };

    struct StudyDataKey{
        QString name;
        bool operator==(const StudyDataKey & other) const { return this->name == other.name; }
        bool operator!=(const StudyDataKey & other) const { return !this->operator==(other); }
        bool operator<(const StudyDataKey & other) const { return this->name < other.name; }
    };
} // namespace

class medDatabaseNavigatorPrivate
{
public:
    medDatabaseNavigatorScene *scene;
    medDatabaseNavigatorView *view;
    int currentPatient ;
    Qt::Orientation orientation;
};

medDatabaseNavigator::medDatabaseNavigator(QWidget *parent) : QFrame(parent), d(new medDatabaseNavigatorPrivate)
{
    d->currentPatient = -1;
    d->orientation = medDatabaseNavigatorController::instance()->orientation();

    d->scene = new medDatabaseNavigatorScene(this);
    d->scene->setOrientation (d->orientation);

    d->view = new medDatabaseNavigatorView(this);
    d->view->setOrientation (d->orientation);
    d->view->setScene(d->scene);
    d->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->view);

    // medDatabaseNavigatorController::instance()->setOrientation(Qt::Vertical);
    medDatabaseNavigatorController::instance()->setItemSize(128, 128);
    d->orientation == Qt::Horizontal
        ? this->setFixedHeight(medDatabaseNavigatorController::instance()->groupHeight() + medDatabaseNavigatorController::instance()->itemSpacing() + 36) // 26 pixels for the scroller
        : this->setFixedWidth(medDatabaseNavigatorController::instance()->groupWidth() + medDatabaseNavigatorController::instance()->itemSpacing() + 20); // 26 pixels for the scroller
}

medDatabaseNavigator::~medDatabaseNavigator(void)
{
    delete d;

    d = NULL;
}

void medDatabaseNavigator::reset(void)
{
    d->scene->reset();
}

void medDatabaseNavigator::onItemClicked(const medDataIndex& index)
{
    if ( index.isValidForPatient() )
        this->onPatientClicked(index);
}

void medDatabaseNavigator::updateNavigator(const medDataIndex& index)
{
    if (index.isValidForPatient() && index.patientId() == d->currentPatient)
        onPatientClicked(index);
}

void medDatabaseNavigator::onPatientClicked(const medDataIndex& index)
{
    // Small trick so that when a patient image gets deleted, we're still able to find all other images of that patient
    medDataIndex baseIndex = index;
    baseIndex.setStudyId(-1);
    baseIndex.setSeriesId(-1);
    baseIndex.setImageId(-1);

    //qDebug()<< "resetting Navigator" << index << baseIndex;

    this->reset();

    if  (!baseIndex.isValidForPatient()) {
        return;
    }
    d->currentPatient = baseIndex.patientId();

    typedef QSet<medDataIndex> IndexSet;
    typedef QList<int> IntList;
    typedef QList<medDataIndex> IndexList;

    medDataManager *dataManager = medDataManager::instance();

    IntList dataSources = dataManager->dataSourceIds();

    QMap<StudyDataKey, medDatabaseNavigatorItemGroup*> groupMap;

    PatientDataKey referencePatientKey;
    referencePatientKey.name = "";
    
    foreach (const int dataSourceId, dataSources )
    {
        medAbstractDbController *dbc = dataManager->controllerForDataSource(dataSourceId);
        if ( !dbc )
            continue;
        
        if (dbc->metaData(baseIndex,medMetaDataKeys::PatientName) != "")
        {
            referencePatientKey.name = dbc->metaData(baseIndex,medMetaDataKeys::PatientName);
            break;
        }
    }

    foreach (const int dataSourceId, dataSources ) {
        //qDebug() << "dataSource:" << dataSourceId;
        medAbstractDbController *dbc = dataManager->controllerForDataSource(dataSourceId);
        if ( !dbc )
            continue;

        IndexList patientsForSource;
        if ( dataSourceId == baseIndex.dataSourceId() ) {
            patientsForSource.push_back(baseIndex);
        } else {
            patientsForSource = dbc->patients();
        }

        //qDebug() << "patients for source" << patientsForSource;

        foreach (const medDataIndex& patient, patientsForSource ) {
            //qDebug() << "patient:" << patient;
            IndexList studiesForSource = dbc->studies(patient);
            QString patientName = dbc->metaData(patient,medMetaDataKeys::PatientName);
            PatientDataKey patientKey;
            patientKey.name = patientName;
            if ( patientKey != referencePatientKey ) {
                continue;
            }

            foreach (const medDataIndex& study, studiesForSource ) {
                //qDebug() << "study:" << study;
                QString studyName = dbc->metaData(study,medMetaDataKeys::StudyDescription);
                StudyDataKey studyKey;
                studyKey.name = studyName;

                medDatabaseNavigatorItemGroup *group = NULL;
                //qDebug() << "groups";
                if ( groupMap.contains(studyKey) ) {
                    //qDebug() << "group contains" << studyKey.name;
                    group = groupMap.find(studyKey).value();
                } else {
                    //qDebug() << "new group";
                    group = new medDatabaseNavigatorItemGroup;
                    group->setOrientation (d->orientation);
                    group->setName(studyName);
                    groupMap[studyKey] = group;
                }

                IndexList seriesForSource = dbc->series(study);

                foreach (const medDataIndex& series, seriesForSource )
                {
                    //qDebug() << "Creating new item for series:" << series;
                    medDatabaseNavigatorItem *item = new medDatabaseNavigatorItem( medDataIndex(series) );

                    connect(item, SIGNAL(itemClicked(const medDataIndex&)),
                            this, SIGNAL(itemClicked(const medDataIndex&)));
                    group->addItem(item);
                }
            }
        }
    }

    foreach(medDatabaseNavigatorItemGroup *group, groupMap)
    {
//        qDebug() << "add group to groupMap";
        d->scene->addGroup(group);
    }
}

void medDatabaseNavigator::onStudyClicked(const medDataIndex& id)
{
    qDebug() << DTK_PRETTY_FUNCTION << id;
}

void medDatabaseNavigator::onSeriesClicked(const medDataIndex& id)
{
    qDebug() << DTK_PRETTY_FUNCTION << id;
}

void medDatabaseNavigator::onImageClicked(const medDataIndex& id)
{
    qDebug() << DTK_PRETTY_FUNCTION << id;
}

void medDatabaseNavigator::setOrientation (Qt::Orientation orientation)
{
    d->orientation = orientation;
    if (d->orientation == Qt::Horizontal) {
        this->setFixedHeight(medDatabaseNavigatorController::instance()->groupHeight() + medDatabaseNavigatorController::instance()->itemSpacing() + 36); // 26 pixels for the scroller
	this->setFixedWidth(QWIDGETSIZE_MAX);
    }
    else {
        this->setFixedWidth(medDatabaseNavigatorController::instance()->groupWidth() + medDatabaseNavigatorController::instance()->itemSpacing() + 36); // 26 pixels for the scroller
	this->setFixedHeight(QWIDGETSIZE_MAX);
    }

    d->view->setOrientation (d->orientation);
    d->scene->setOrientation (d->orientation);
}

Qt::Orientation medDatabaseNavigator::orientation() const
{
  return d->orientation;
}
