/* medDatabaseNavigator.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:38:39 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Jun 28 14:21:37 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 59
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabaseController.h"
#include "medDatabaseNavigator.h"
#include "medDatabaseNavigatorController.h"
#include "medDatabaseNavigatorItem.h"
#include "medDatabaseNavigatorItemGroup.h"
#include "medDatabaseNavigatorScene.h"
#include "medDatabaseNavigatorView.h"
#include "medSql/medDatabaseNonPersistentItem.h"
#include "medSql/medDatabaseNonPersistentController.h"

#include <medCore/medStorage.h>

#include <QtCore>
#include <QtGui>

#include <dtkCore/dtkGlobal.h>

class medDatabaseNavigatorPrivate
{
public:
    medDatabaseNavigatorScene *scene;
    medDatabaseNavigatorView *view;

    Qt::Orientation orientation;
};

medDatabaseNavigator::medDatabaseNavigator(QWidget *parent) : QFrame(parent), d(new medDatabaseNavigatorPrivate)
{
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
        : this->setFixedWidth(medDatabaseNavigatorController::instance()->groupWidth() + medDatabaseNavigatorController::instance()->itemSpacing() + 36); // 26 pixels for the scroller
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

void medDatabaseNavigator::onPatientClicked(const medDataIndex& index)
{
    this->reset();

    // Query persistent data

    QSqlQuery patientQuery(*(medDatabaseController::instance()->database())); QVariant id;

    // Query persistent data -- Retrieve patient information

    QString patientName;
    QString patientThumbnail;

    patientQuery.prepare("SELECT name, thumbnail FROM patient WHERE id = :id");
    patientQuery.bindValue(":id", index.patientId());
    if(!patientQuery.exec())
        qDebug() << DTK_COLOR_FG_RED << patientQuery.lastError() << DTK_NO_COLOR;

    if(patientQuery.first())
        patientName = patientQuery.value(0).toString();

    // Query persistent data -- Retrieve studies information

    QSqlQuery studyQuery(*(medDatabaseController::instance()->database()));

    QVariant studyId;
    QVariant studyName;
    QVariant studyThumbnail;

    studyQuery.prepare("SELECT id, name, thumbnail FROM study WHERE patient = :patient");
    studyQuery.bindValue(":patient", index.patientId());
    if(!studyQuery.exec())
        qDebug() << DTK_COLOR_FG_RED << studyQuery.lastError() << DTK_NO_COLOR;

    while(studyQuery.next()) {
        studyId = studyQuery.value(0);
        studyName = studyQuery.value(1);
        studyThumbnail = studyQuery.value(2);

        medDatabaseNavigatorItemGroup *group = new medDatabaseNavigatorItemGroup;
	group->setOrientation (d->orientation);
	
        group->setName(studyName.toString());

        // Retrieve series information

        QSqlQuery seriesQuery(*(medDatabaseController::instance()->database()));

        QVariant seriesId;
        QVariant seriesName;
        QVariant seriesThumbnail;

        seriesQuery.prepare("SELECT id, name, thumbnail FROM series WHERE study = :study");
        seriesQuery.bindValue(":study", studyId);
        if(!seriesQuery.exec())
            qDebug() << DTK_COLOR_FG_RED << seriesQuery.lastError() << DTK_NO_COLOR;

        while(seriesQuery.next()) {
            seriesId = seriesQuery.value(0);
            seriesName = seriesQuery.value(1);
            seriesThumbnail = seriesQuery.value(2);

            QString thumbPath = medStorage::dataLocation() + seriesThumbnail.toString();
            qDebug() << thumbPath;
            medDatabaseNavigatorItem *item = new medDatabaseNavigatorItem(index.patientId(), studyId.toInt(), seriesId.toInt(), -1, thumbPath);

            connect(item, SIGNAL(patientClicked(int)), this, SIGNAL(patientClicked(int)));
            connect(item, SIGNAL(studyClicked(int)), this, SIGNAL(studyClicked(int)));
            connect(item, SIGNAL(seriesClicked(int)), this, SIGNAL(seriesClicked(int)));
            connect(item, SIGNAL(imageClicked(int)), this, SIGNAL(imageClicked(int)));

            group->addItem(item);
        }

        d->scene->addGroup(group);
    }

    // Query non persistent data

    QMap<QString, medDatabaseNavigatorItemGroup*> groupMap;

    foreach(medDatabaseNonPersistentItem *item, medDatabaseNonPersistentController::instance()->items()) {

        if(item->index().patientId() == index.patientId()) {

            medDatabaseNavigatorItem *nitem = new medDatabaseNavigatorItem(
                item->index().patientId(), 
                item->index().studyId(), 
                item->index().seriesId(), 
                item->index().imageId(), 
                item->thumb());
            
            connect(nitem, SIGNAL(patientClicked(int)), this, SIGNAL(patientClicked(int)));
            connect(nitem, SIGNAL(studyClicked(int)), this, SIGNAL(studyClicked(int)));
            connect(nitem, SIGNAL(seriesClicked(int)), this, SIGNAL(seriesClicked(int)));
            connect(nitem, SIGNAL(imageClicked(int)), this, SIGNAL(imageClicked(int)));

	    medDatabaseNavigatorItemGroup *group = NULL;
	    if (groupMap.contains(item->studyName()))
	        group = groupMap.value(item->studyName());
	    else {
		group = new medDatabaseNavigatorItemGroup;
		group->setName (item->studyName());
		groupMap.insert(item->studyName(), group);		
	    }
	                
            group->addItem(nitem);
        }
    }

    foreach(medDatabaseNavigatorItemGroup *group, groupMap)
        d->scene->addGroup(group);
}

void medDatabaseNavigator::onStudyClicked(int id)
{
    qDebug() << DTK_PRETTY_FUNCTION << id;
}

void medDatabaseNavigator::onSeriesClicked(int id)
{
    qDebug() << DTK_PRETTY_FUNCTION << id;
}

void medDatabaseNavigator::onImageClicked(int id)
{
    qDebug() << DTK_PRETTY_FUNCTION << id;
}

void medDatabaseNavigator::addThumbnail(const QImage& thumbnail)
{
    medDatabaseNavigatorItemGroup *group = new medDatabaseNavigatorItemGroup;
    medDatabaseNavigatorItem *item = new medDatabaseNavigatorItem(-1, -1, -1, -1, thumbnail);

    group->addItem(item);
    d->scene->addGroup(group);    
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

Qt::Orientation medDatabaseNavigator::orientation (void) const
{
  return d->orientation;
}
