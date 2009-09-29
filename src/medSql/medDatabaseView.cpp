/* medDatabaseView.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Mar 31 13:18:20 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Sep 25 12:13:57 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 64
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <medSql/medDatabaseModel.h>
#include <medSql/medDatabaseView.h>
#include <medSql/medDatabaseItem.h>

medDatabaseView::medDatabaseView(QWidget *parent) : QTreeView(parent)
{
    this->setFrameStyle(QFrame::NoFrame);
    this->setAttribute(Qt::WA_MacShowFocusRect, false);
    this->setUniformRowHeights(true);
    this->setAcceptDrops(true);

    connect(this, SIGNAL(      clicked(const QModelIndex&)), this, SLOT(onItemClicked(const QModelIndex&)));
    connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(onItemDoubleClicked(const QModelIndex&)));
}

medDatabaseView::~medDatabaseView(void)
{

}

void medDatabaseView::setModel(medDatabaseModel *model)
{
    QTreeView::setModel(model);
}

void medDatabaseView::onItemClicked(const QModelIndex& index)
{
    if(qobject_cast<medDatabaseModel *>(this->model()))
        if(medDatabaseItem *item = static_cast<medDatabaseItem *>(index.internalPointer()))
            if(item->table() == "patient")
                emit patientClicked(item->value(0).toInt());
            else if(item->table() == "study")
                emit studyClicked(item->value(0).toInt());
            else if(item->table() == "series")
                emit seriesClicked(item->value(0).toInt());
            else
                emit imageClicked(item->value(0).toInt());
}

void medDatabaseView::onItemDoubleClicked(const QModelIndex& index)
{
    if(qobject_cast<medDatabaseModel *>(this->model()))
        if(medDatabaseItem *item = static_cast<medDatabaseItem *>(index.internalPointer()))
            if(item->table() == "patient")
                emit patientDoubleClicked(item->value(0).toInt());
            else if(item->table() == "study")
                emit studyDoubleClicked(item->value(0).toInt());
            else if(item->table() == "series")
                emit seriesDoubleClicked(item->value(0).toInt());
            else
                emit imageDoubleClicked(item->value(0).toInt());
}
