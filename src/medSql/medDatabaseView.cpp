/* medDatabaseView.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Mar 31 13:18:20 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Sat Oct 10 00:57:20 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 83
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

medDatabaseModel *deproxy(QAbstractItemModel *model)
{
    if(QAbstractProxyModel *proxy = dynamic_cast<QAbstractProxyModel *>(model)) {
        return deproxy(proxy->sourceModel());
    } else if(medDatabaseModel *db = dynamic_cast<medDatabaseModel *>(model)) {
        return db;
    } else {
        return NULL;
    }
}

medDatabaseItem *deproxy(QAbstractItemModel *model, QModelIndex index)
{
    if(QAbstractProxyModel *proxy = dynamic_cast<QAbstractProxyModel *>(model)) {
        return deproxy(proxy->sourceModel(), proxy->mapToSource(index));
    } else if(medDatabaseModel *db = dynamic_cast<medDatabaseModel *>(model)) {
        return static_cast<medDatabaseItem *>(index.internalPointer());
    } else {
        return NULL;
    }
}

medDatabaseView::medDatabaseView(QWidget *parent) : QTreeView(parent)
{
    this->setAcceptDrops(true);
    this->setFrameStyle(QFrame::NoFrame);
    this->setAttribute(Qt::WA_MacShowFocusRect, false);
    this->setUniformRowHeights(true);
    this->setAlternatingRowColors(true);

    this->header()->setStretchLastSection(true);

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
    medDatabaseItem *item = deproxy(this->model(), index);

    if(item)
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
    medDatabaseItem *item = deproxy(this->model(), index);

    if(item)
        if(item->table() == "patient") {
            emit patientDoubleClicked(item->value(0).toInt());
            emit patientDoubleClicked(index);
        }
        else if(item->table() == "study") {
            emit studyDoubleClicked(item->value(0).toInt());
            emit studyDoubleClicked(index);
        }
        else if(item->table() == "series") {
            emit seriesDoubleClicked(item->value(0).toInt());
            emit seriesDoubleClicked(index);
        }
        else {
            emit imageDoubleClicked(item->value(0).toInt());
            emit imageDoubleClicked(index);
        }
}
