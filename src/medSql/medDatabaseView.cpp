/* medDatabaseView.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Mar 31 13:18:20 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Jun 14 13:42:21 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 122
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseModel.h>
#include <medSql/medDatabaseView.h>
#include <medSql/medDatabaseItem.h>

medDatabaseView::medDatabaseView(QWidget *parent) : QTreeView(parent)
{
    this->setAcceptDrops(true);
    this->setFrameStyle(QFrame::NoFrame);
    this->setAttribute(Qt::WA_MacShowFocusRect, false);
    this->setUniformRowHeights(true);
    this->setAlternatingRowColors(true);
    this->setAnimated(false);
    this->setSortingEnabled(true);
    // this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->header()->setStretchLastSection(true);
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(      clicked(const QModelIndex&)), this, SLOT(onItemClicked(const QModelIndex&)));
    connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(onItemDoubleClicked(const QModelIndex&)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(updateContextMenu(const QPoint&)));
}

medDatabaseView::~medDatabaseView(void)
{

}

int medDatabaseView::sizeHintForColumn(int column) const
{
    Q_UNUSED(column);

    return 150;
}

void medDatabaseView::setModel(medDatabaseModel *model)
{
    QTreeView::setModel(model);

    for(int i = 0; i < model->columnCount(); this->resizeColumnToContents(i), i++);

    this->hideColumn(20);
}

void medDatabaseView::onPatientClicked(int id)
{
    if(medDatabaseModel *model = dynamic_cast<medDatabaseModel *>(this->model())) {
        QModelIndex index = model->indexForPatient(id);
        this->collapseAll();
        this->setExpanded(index, true);
        selectionModel()->clearSelection();
        selectionModel()->select(index, QItemSelectionModel::Select);
    }
}

void medDatabaseView::onStudyClicked(int id)
{
    if(medDatabaseModel *model = dynamic_cast<medDatabaseModel *>(this->model())) {
        QModelIndex index = model->indexForStudy(id);
        this->collapseAll();
        this->setExpanded(index.parent(), true);
        this->setExpanded(index, true);
        selectionModel()->clearSelection();
        selectionModel()->select(index, QItemSelectionModel::Select);
    }
}

void medDatabaseView::onSeriesClicked(int id)
{
    if(medDatabaseModel *model = dynamic_cast<medDatabaseModel *>(this->model())) {
        QModelIndex index = model->indexForSeries(id);
        this->collapseAll();
        this->setExpanded(index.parent().parent(), true);
        this->setExpanded(index.parent(), true);
        this->setExpanded(index, true);
        selectionModel()->clearSelection();
        selectionModel()->select(index, QItemSelectionModel::Select);
    }
}

void medDatabaseView::onImageClicked(int id)
{
    if(medDatabaseModel *model = dynamic_cast<medDatabaseModel *>(this->model())) {
        QModelIndex index = model->indexForImage(id);
        this->collapseAll();
        this->setExpanded(index.parent().parent().parent(), true);
        this->setExpanded(index.parent().parent(), true);
        this->setExpanded(index.parent(), true);
        this->setExpanded(index, true);
        selectionModel()->clearSelection();
        selectionModel()->select(index, QItemSelectionModel::Select);
    }
}

void medDatabaseView::updateContextMenu(const QPoint& point)
{
    QModelIndex index = this->indexAt(point);

    if(!index.isValid())
        return;

    medDatabaseItem *item = NULL;

    if(medDatabaseModel *model = dynamic_cast<medDatabaseModel *>(this->model()))
        item = static_cast<medDatabaseItem *>(index.internalPointer());

    if(item && item->table() == "series") {    
        QMenu menu(this);
        menu.addAction("View", this, SLOT(onMenuViewClicked()));
        menu.exec(mapToGlobal(point));
    }
}

void medDatabaseView::onItemClicked(const QModelIndex& index)
{
    medDatabaseItem *item = NULL;

    if(medDatabaseModel *model = dynamic_cast<medDatabaseModel *>(this->model()))
        item = static_cast<medDatabaseItem *>(index.internalPointer());

    if(item)
        if(item->table() == "patient"){
            emit patientClicked(item->value(20).toInt());
        }
        else if(item->table() == "study")
            emit studyClicked(item->value(20).toInt());
        else if(item->table() == "series")
            emit seriesClicked(item->value(20).toInt());
        else
            emit imageClicked(item->value(20).toInt());
}

void medDatabaseView::onItemDoubleClicked(const QModelIndex& index)
{
    medDatabaseItem *item = NULL;

    if(medDatabaseModel *model = dynamic_cast<medDatabaseModel *>(this->model()))
        item = static_cast<medDatabaseItem *>(index.internalPointer());

    if(item)
        if(item->table() == "patient")
            ;
        else if(item->table() == "study")
            ;
        else if(item->table() == "series")
            ;
        else
            ;
}

void medDatabaseView::onMenuViewClicked(void)
{
    if(!this->selectedIndexes().count())
        return;

    QModelIndex index = this->selectedIndexes().first();

    if(!index.isValid())
        return;

    medDatabaseItem *item = NULL;

    if(medDatabaseModel *model = dynamic_cast<medDatabaseModel *>(this->model()))
        item = static_cast<medDatabaseItem *>(index.internalPointer());

    if(item)
        if(item->table() == "patient")
            ;
        else if(item->table() == "study")
            ;
        else if(item->table() == "series")
            emit open(medDatabaseController::instance()->indexForSeries(item->value(20).toInt()));
        else
            ;
}
