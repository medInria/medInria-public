/* medDatabaseView.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Mar 31 13:18:20 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct  6 19:08:51 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 126
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabaseController.h"
#include "medDatabaseModel.h"
#include "medDatabaseView.h"
#include "medDatabaseItem.h"
#include "medDatabaseProxyModel.h"
#include "medCore/medDataManager.h"

class NoFocusDelegate : public QStyledItemDelegate
{
protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

void NoFocusDelegate::paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex &index) const
{
    QStyleOptionViewItem itemOption(option);
    if (itemOption.state & QStyle::State_HasFocus)
        itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
    QStyledItemDelegate::paint(painter, itemOption, index);
}

medDatabaseView::medDatabaseView(QWidget *parent) : QTreeView(parent)
{
    this->setAcceptDrops(true);
    this->setFrameStyle(QFrame::NoFrame);
    this->setAttribute(Qt::WA_MacShowFocusRect, false);
    this->setUniformRowHeights(true);
    this->setAlternatingRowColors(true);
    this->setAnimated(false);
    this->setSortingEnabled(true);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->header()->setStretchLastSection(true);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    //connect(this, SIGNAL(      clicked(const QModelIndex&)), this, SLOT(onItemClicked(const QModelIndex&))); // obsolete with selectionModel changed signal
    connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(onItemDoubleClicked(const QModelIndex&)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(updateContextMenu(const QPoint&)));

    NoFocusDelegate* delegate = new NoFocusDelegate();
    this->setItemDelegate(delegate);
}

medDatabaseView::~medDatabaseView(void)
{
}

int medDatabaseView::sizeHintForColumn(int column) const
{
    if (column<3)
        return 150;
    
    return 50;
}

void medDatabaseView::setModel(QAbstractItemModel *model)
{
    QTreeView::setModel(model);

    for(int i = 0; i < model->columnCount(); this->resizeColumnToContents(i), i++);
    connect( this->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), SLOT(selectionChanged(const QModelIndex&, const QModelIndex&)));

}


void medDatabaseView::updateContextMenu(const QPoint& point)
{
    QModelIndex index = this->indexAt(point);

    if(!index.isValid())
        return;

    medDatabaseItem *item = NULL;

    if(medDatabaseProxyModel *proxy = dynamic_cast<medDatabaseProxyModel *>(this->model()))
        item = static_cast<medDatabaseItem *>(proxy->mapToSource(index).internalPointer());
    else if (medDatabaseModel *model = dynamic_cast<medDatabaseModel *>(this->model()))
        item = static_cast<medDatabaseItem *>(index.internalPointer());


    if (item) {
        QMenu menu(this);
        if( item->dataIndex().isValidForSeries())
        {
            menu.addAction(tr("View"), this, SLOT(onMenuViewClicked()));
            menu.addAction(tr("Export"), this, SLOT(onMenuExportClicked()));
            menu.addAction(tr("Remove"), this, SLOT(onMenuRemoveClicked()));
            menu.exec(mapToGlobal(point));
        }
        else if (item->dataIndex().isValidForPatient())
        {
            menu.addAction(tr("Remove"), this, SLOT(onMenuRemoveClicked()));
            menu.exec(mapToGlobal(point));
        }
    }

}

void medDatabaseView::onItemClicked(const QModelIndex& index)
{

    medDatabaseItem *item = NULL;

    if(medDatabaseProxyModel *proxy = dynamic_cast<medDatabaseProxyModel *>(this->model()))
        item = static_cast<medDatabaseItem *>(proxy->mapToSource(index).internalPointer());
    else if (medDatabaseModel *model = dynamic_cast<medDatabaseModel *>(this->model()))
        item = static_cast<medDatabaseItem *>(index.internalPointer());

    if (!item)
        return;

    if (item->dataIndex().isValidForSeries())
    {
        this->collapseAll();
        this->setExpanded(index.parent().parent(), true);
        this->setExpanded(index.parent(), true);
        this->setExpanded(index, true);
        emit seriesClicked(item->dataIndex ().seriesId ());
    }
    else if (item->dataIndex().isValidForPatient())
    {
        this->collapseAll();
        this->setExpanded(index, true);
        emit patientClicked(item->dataIndex ().patientId ());
    }


}

void medDatabaseView::onItemDoubleClicked(const QModelIndex& index)
{
    medDatabaseItem *item = NULL;

    if(medDatabaseProxyModel *proxy = dynamic_cast<medDatabaseProxyModel *>(this->model()))
        item = static_cast<medDatabaseItem *>(proxy->mapToSource(index).internalPointer());
    else if (medDatabaseModel *model = dynamic_cast<medDatabaseModel *>(this->model()))
        item = static_cast<medDatabaseItem *>(index.internalPointer());

    if (item)
        emit (open(item->dataIndex()));
}

void medDatabaseView::onMenuViewClicked(void)
{
    if(!this->selectedIndexes().count())
        return;

    QModelIndex index = this->selectedIndexes().first();

    if(!index.isValid())
        return;

    medDatabaseItem *item = NULL;

    if(medDatabaseProxyModel *proxy = dynamic_cast<medDatabaseProxyModel *>(this->model()))
        item = static_cast<medDatabaseItem *>(proxy->mapToSource(index).internalPointer());

    if (item && (item->dataIndex().isValidForSeries()))
    {        
        emit open(medDatabaseController::instance()->indexForSeries(item->dataIndex ().seriesId ()));
    }
}

void medDatabaseView::onMenuExportClicked(void)
{
    if(!this->selectedIndexes().count())
        return;

    QModelIndex index = this->selectedIndexes().first();

    if(!index.isValid())
        return;

    medDatabaseItem *item = NULL;

    if(medDatabaseProxyModel *proxy = dynamic_cast<medDatabaseProxyModel *>(this->model()))
        item = static_cast<medDatabaseItem *>(proxy->mapToSource(index).internalPointer());

    if(item)
        if(item->table() == "patient")
            ;
        else if(item->table() == "study")
            ;
        else if(item->table() == "series")
            emit exportData(medDatabaseController::instance()->indexForSeries(item->value(20).toInt()));
        else
            ;
}

void medDatabaseView::selectionChanged( const QModelIndex& current, const QModelIndex& previous)
{
    emit onItemClicked(current);
}

void medDatabaseView::onMenuRemoveClicked( void )
{
    QModelIndexList indexes = this->selectedIndexes();
    if(!indexes.count())
        return;

    QModelIndex index = indexes.at(0);

    medDatabaseItem *item = NULL;

    if(medDatabaseProxyModel *proxy = dynamic_cast<medDatabaseProxyModel *>(this->model()))
        item = static_cast<medDatabaseItem *>(proxy->mapToSource(index).internalPointer());

    if (item)
    {
        // Copy the data index, because the data item may cease to be valid.
        medDataIndex index = item->dataIndex();
        medDataManager::instance()->removeData(index);
    }

}
