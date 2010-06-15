/* medDatabaseNavigator.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:38:39 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Jun 14 21:33:21 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 13
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

#include <QtCore>
#include <QtGui>

#include <dtkCore/dtkGlobal.h>

class medDatabaseNavigatorPrivate
{
public:
    medDatabaseNavigatorScene *scene;
    medDatabaseNavigatorView *view;
};

medDatabaseNavigator::medDatabaseNavigator(QWidget *parent) : QFrame(parent), d(new medDatabaseNavigatorPrivate)
{
    d->scene = new medDatabaseNavigatorScene(this);

    d->view = new medDatabaseNavigatorView(this);
    d->view->setScene(d->scene);
    d->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->view);

    medDatabaseNavigatorController::instance()->setOrientation(Qt::Vertical);
    medDatabaseNavigatorController::instance()->setItemSize(128, 128);
    medDatabaseNavigatorController::instance()->orientation() == Qt::Horizontal
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

void medDatabaseNavigator::onPatientClicked(int patientId)
{
    this->reset();

    QSqlQuery patientQuery(*(medDatabaseController::instance()->database())); QVariant id;

    // Retrieve patient information

    QString patientName;
    QString patientThumbnail;

    patientQuery.prepare("SELECT name, thumbnail FROM patient WHERE id = :id");
    patientQuery.bindValue(":id", patientId);
    if(!patientQuery.exec())
        qDebug() << DTK_COLOR_FG_RED << patientQuery.lastError() << DTK_NO_COLOR;

    if(patientQuery.first())
        patientName = patientQuery.value(0).toString();

    // Retrieve studies information

    QSqlQuery studyQuery(*(medDatabaseController::instance()->database()));

    QVariant studyId;
    QVariant studyName;
    QVariant studyThumbnail;

    studyQuery.prepare("SELECT id, name, thumbnail FROM study WHERE patient = :patient");
    studyQuery.bindValue(":patient", patientId);
    if(!studyQuery.exec())
        qDebug() << DTK_COLOR_FG_RED << studyQuery.lastError() << DTK_NO_COLOR;

    while(studyQuery.next()) {
        studyId = studyQuery.value(0);
        studyName = studyQuery.value(1);
        studyThumbnail = studyQuery.value(2);

        medDatabaseNavigatorItemGroup *group = new medDatabaseNavigatorItemGroup;

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

            qDebug() << "Creating navigator item with id" << patientId;

            medDatabaseNavigatorItem *item = new medDatabaseNavigatorItem(patientId, studyId.toInt(), seriesId.toInt(), -1, seriesThumbnail.toString());

            connect(item, SIGNAL(patientClicked(int)), this, SIGNAL(patientClicked(int)));
            connect(item, SIGNAL(studyClicked(int)), this, SIGNAL(studyClicked(int)));
            connect(item, SIGNAL(seriesClicked(int)), this, SIGNAL(seriesClicked(int)));
            connect(item, SIGNAL(imageClicked(int)), this, SIGNAL(imageClicked(int)));

            group->addItem(item);
        }

        d->scene->addGroup(group);
    }
}

void medDatabaseNavigator::onStudyClicked(int id)
{
#ifndef WIN32
	qDebug() << __func__ << id;
#endif
}

void medDatabaseNavigator::onSeriesClicked(int id)
{
#ifndef WIN32
    qDebug() << __func__ << id;
#endif
}

void medDatabaseNavigator::onImageClicked(int id)
{
#ifndef WIN32
    qDebug() << __func__ << id;
#endif
}

void medDatabaseNavigator::addThumbnail(const QImage& thumbnail)
{
    medDatabaseNavigatorItemGroup *group = new medDatabaseNavigatorItemGroup;
    medDatabaseNavigatorItem *item = new medDatabaseNavigatorItem(-1, -1, -1, -1, thumbnail);

    group->addItem(item);
    d->scene->addGroup(group);    
}
