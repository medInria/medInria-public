/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseView.h>
#include <medDataManager.h>
#include <medAbstractDatabaseItem.h>
#include <medAbstractDbController.h>
#include <medDatabaseEditItemDialog.h>

#include <medAbstractDataFactory.h>

#include <QSortFilterProxyModel>
#include <QAbstractItemModel>
#include <QFontMetrics>

class NoFocusDelegate : public QStyledItemDelegate
{
public:
    NoFocusDelegate(medDatabaseView *view, QList<medDataIndex> indexes) : QStyledItemDelegate(), m_view(view), m_indexes(indexes) {}

    void append(medDataIndex);

protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    medDatabaseView *m_view;
    QList<medDataIndex> m_indexes;
};

void NoFocusDelegate::append(medDataIndex index)
{
    m_indexes.append(index);
}

void NoFocusDelegate::paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex &index) const
{
    QStyleOptionViewItem itemOption(option);
    if (itemOption.state & QStyle::State_HasFocus)
        itemOption.state = itemOption.state ^ QStyle::State_HasFocus;

    if(index.isValid()) {
        medAbstractDatabaseItem *item = NULL;

        if(QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(m_view->model()))
            item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());
        else if (dynamic_cast<QAbstractItemModel *>(m_view->model()))
            item = static_cast<medAbstractDatabaseItem *>(index.internalPointer());

        if(item)
        {               
            if(index.column()>0)
            {
                QPen pen;
                pen.setColor(QColor("#505050"));
                painter->setPen(pen);
                painter->drawLine(option.rect.x(), option.rect.y(), option.rect.x(), (option.rect.y()+option.rect.height()));
            }
            if (m_indexes.contains(item->dataIndex()))
                // items that failed to open will have a pinkish background               
                painter->fillRect(option.rect, QColor("#FF3333"));

            medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(item->dataIndex().dataSourceId());
            if ( dbc ) {
                if(!dbc->isPersistent())
                {
                    itemOption.font.setItalic(true);
                }
            }
        }
    }

    QStyledItemDelegate::paint(painter, itemOption, index);
}

class medDatabaseViewPrivate
{
public:
    QAction *viewAction;
    QAction *exportAction;
    QAction *saveAction;
    QAction *removeAction;
    QAction *addPatientAction;
    QAction *addStudyAction;
    QAction *editAction;
    QMenu *contextMenu;
};

medDatabaseView::medDatabaseView(QWidget *parent) : QTreeView(parent), d(new medDatabaseViewPrivate)
{
    this->setDragEnabled(true);
    this->setDropIndicatorShown(true);

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
    this->header()->setDefaultAlignment(Qt::AlignCenter);
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    this->setEditTriggers(QAbstractItemView:: SelectedClicked);

    connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(onItemDoubleClicked(const QModelIndex&)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(updateContextMenu(const QPoint&)));

    NoFocusDelegate* delegate = new NoFocusDelegate(this, QList<medDataIndex>());
    this->setItemDelegate(delegate);

    d->contextMenu = new QMenu(this);

    //Init the QActions
    d->viewAction = new QAction(tr("View"), this);
    d->viewAction->setIconVisibleInMenu(true);
    d->viewAction->setIcon(QIcon(":icons/eye.png"));
    connect(d->viewAction, SIGNAL(triggered()), this, SLOT(onViewSelectedItemRequested()));

    d->exportAction = new QAction(tr("Export"), this);
    d->exportAction->setIconVisibleInMenu(true);
    d->exportAction->setIcon(QIcon(":icons/export.png"));
    connect(d->exportAction, SIGNAL(triggered()), this, SLOT(onExportSelectedItemRequested()));

    d->saveAction = new QAction(tr("Save"), this);
    d->saveAction->setIconVisibleInMenu(true);
    d->saveAction->setIcon(QIcon(":icons/save.png"));
    connect(d->saveAction, SIGNAL(triggered()), this, SLOT(onSaveSelectedItemRequested()));

    d->removeAction = new QAction(tr("Remove"), this);
    d->removeAction->setIconVisibleInMenu(true);
    d->removeAction->setIcon(QIcon(":icons/cross.svg"));
    connect(d->removeAction, SIGNAL(triggered()), this, SLOT(onRemoveSelectedItemRequested()));

    d->addPatientAction = new QAction(tr("New Patient"), this);
    d->addPatientAction->setIconVisibleInMenu(true);
    d->addPatientAction->setIcon(QIcon(":icons/user_add.png"));
    connect(d->addPatientAction, SIGNAL(triggered()), this, SLOT(onCreatePatientRequested()));
    
    d->addStudyAction = new QAction(tr("New Study"), this);
    d->addStudyAction->setIconVisibleInMenu(true);
    d->addStudyAction->setIcon(QIcon(":icons/page_add.png"));
    connect(d->addStudyAction, SIGNAL(triggered()), this, SLOT(onCreateStudyRequested()));

    d->editAction = new QAction(tr("Edit..."), this);
    d->editAction->setIconVisibleInMenu(true);
    d->editAction->setIcon(QIcon(":icons/page_edit.png"));
    connect(d->editAction, SIGNAL(triggered()), this, SLOT(onEditRequested()));

}

medDatabaseView::~medDatabaseView(void)
{
    //TreeViews don't take ownership of delegates
    delete this->itemDelegate();
    delete d;
}


int medDatabaseView::sizeHintForColumn(int column) const
{
    if (column < 0 || column > this->model()->columnCount()) return 100;

    return this->fontMetrics().width(this->model()->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString()) + 40;
}

void medDatabaseView::setModel(QAbstractItemModel *model)
{
    QTreeView::setModel(model);

    this->expandAll();
    
    
    this->header()->setMinimumSectionSize(60);
    this->header()->resizeSections(QHeaderView::ResizeToContents);
    this->collapseAll();

    // we stopped using this signal as it is not being emitted after removing or saving an item (and the selection does change)
    //connect( this->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), SLOT(onSelectionChanged(const QModelIndex&, const QModelIndex&)));

    connect( this->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(onSelectionChanged(const QItemSelection&, const QItemSelection&)) );
    
    connect( model, SIGNAL(dataChanged ( const QModelIndex &, const QModelIndex & )),
             this, SLOT( setCurrentIndex(QModelIndex)));
}

void medDatabaseView::updateContextMenu(const QPoint& point)
{
    QModelIndex index = this->indexAt(point);

    d->contextMenu->clear();
    d->contextMenu->addAction(d->addPatientAction);

    medAbstractDatabaseItem *item = NULL;

    if(index.isValid())
    {
        if(QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
            item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());
        else if (dynamic_cast<QAbstractItemModel *>(this->model()))
            item = static_cast<medAbstractDatabaseItem *>(index.internalPointer());
    }

    if (item)
    {
        if( item->dataIndex().isValidForSeries())
        {
            d->contextMenu->addAction(d->editAction);
            d->editAction->setIcon(QIcon(":icons/page_edit.png"));
            d->contextMenu->addAction(d->viewAction);
            d->contextMenu->addAction(d->exportAction);
            d->contextMenu->addAction(d->removeAction);
            if( !(medDataManager::instance()->controllerForDataSource(item->dataIndex().dataSourceId())->isPersistent()) )
                d->contextMenu->addAction(d->saveAction);
        }
        else if (item->dataIndex().isValidForStudy())
        {
            d->contextMenu->addAction(d->editAction);
            d->editAction->setIcon(QIcon(":icons/page_edit.png"));
            d->contextMenu->addAction(d->removeAction);
            if( !(medDataManager::instance()->controllerForDataSource(item->dataIndex().dataSourceId())->isPersistent()) )
                d->contextMenu->addAction(d->saveAction);
        }
        else if (item->dataIndex().isValidForPatient())
        {
            d->contextMenu->addAction(d->addStudyAction);
            d->contextMenu->addAction(d->editAction);
            d->editAction->setIcon(QIcon(":icons/user_edit.png"));
            d->contextMenu->addAction(d->removeAction);
            if( !(medDataManager::instance()->controllerForDataSource(item->dataIndex().dataSourceId())->isPersistent()) )
                d->contextMenu->addAction(d->saveAction);
        }
    }

    d->contextMenu->exec(mapToGlobal(point));
}

void medDatabaseView::onItemDoubleClicked(const QModelIndex& index)
{
    medAbstractDatabaseItem *item = 0;

    if (QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel*>(this->model()))
        item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());
    else if (dynamic_cast<QAbstractItemModel*>(this->model()))
        item = static_cast<medAbstractDatabaseItem *>(index.internalPointer());

    if (item && item->dataIndex().isValidForSeries())
        emit (openRequest(item->dataIndex()));
}

/** Opens the currently selected item. */
void medDatabaseView::onViewSelectedItemRequested(void)
{
    if(!this->selectedIndexes().count())
        return;

    QModelIndex index = this->selectedIndexes().first();

    if(!index.isValid())
        return;

    medAbstractDatabaseItem *item = NULL;

    if(QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
        item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());

    if (item && (item->dataIndex().isValidForSeries()))
    {
        emit openRequest(item->dataIndex ());
    }
}

/** Exports the currently selected item. */
void medDatabaseView::onExportSelectedItemRequested(void)
{
    if(!this->selectedIndexes().count())
        return;

    QModelIndex index = this->selectedIndexes().first();

    if(!index.isValid())
        return;

    medAbstractDatabaseItem *item = NULL;

    if(QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
        item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());

    if(item)
        emit exportDataRequest(item->dataIndex());
}

void medDatabaseView::onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    if (selected.count() == 0)
    {
        emit noPatientOrSeriesSelected();
        return;
    }
        
    // so far we only allow single selection
    QModelIndex index = selected.indexes()[0];

    medAbstractDatabaseItem *item = NULL;

    if(QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
        item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());
    else if (dynamic_cast<QAbstractItemModel *>(this->model()))
        item = static_cast<medAbstractDatabaseItem *>(index.internalPointer());

    if (!item)
        return;

    if (item->dataIndex().isValidForSeries())
    {
        this->setExpanded(index.parent().parent(), true);
        this->setExpanded(index.parent(), true);
        this->setExpanded(index, true);
        emit seriesClicked(item->dataIndex());
    }
    else if (item->dataIndex().isValidForStudy())
    {
        this->setExpanded(index, true);
        emit studyClicked(item->dataIndex());
    }
    else if (item->dataIndex().isValidForPatient())
    {
        this->setExpanded(index, true);
        emit patientClicked(item->dataIndex());
    }
}

/** Removes the currently selected item. */
void medDatabaseView::onRemoveSelectedItemRequested( void )
{
    int reply = QMessageBox::question(this, tr("Remove item"),
            tr("Are you sure you want to continue?\n""This cannot be undone."),
            QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);

    if( reply == QMessageBox::Yes )
    {
        QModelIndexList indexes = this->selectedIndexes();
        if(!indexes.count())
            return;

        QModelIndex index = indexes.at(0);

        medAbstractDatabaseItem *item = NULL;

        if(QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
            item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());

        if (item)
        {
            // Copy the data index, because the data item may cease to be valid.
            medDataIndex index = item->dataIndex();
            medDataManager::instance()->removeData(index);
        }
    }
}

/** Saves the currently selected item. */
void medDatabaseView::onSaveSelectedItemRequested(void)
{
    QModelIndexList indexes = this->selectedIndexes();
    if(!indexes.count())
        return;

    QModelIndex index = indexes.at(0);

    medAbstractDatabaseItem *item = NULL;

    if(QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
        item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());

    if (item)
    {
        // Copy the data index, because the data item may cease to be valid.
        medDataIndex index = item->dataIndex();
        medDataManager::instance()->makePersistent(medDataManager::instance()->retrieveData(index));
        qDebug() << "DEBUG : onMenuSaveClicked() after storeNonPersistentSingleDataToDatabase";
        qDebug() << "DEBUG : index" << index;
    }
}

/** Creates a new patient */
void medDatabaseView::onCreatePatientRequested(void)
{
    QModelIndexList indexes = this->selectedIndexes();

    bool isPersistent = true;

    if(indexes.count() > 0)
    {
        QModelIndex index = indexes.at(0);

        medAbstractDatabaseItem *item = NULL;

        if(QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
            item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());

        if (item)
        {
            isPersistent = medDataManager::instance()->controllerForDataSource(
                               item->dataIndex().dataSourceId() )->isPersistent();
        }
    }

    QString patientName = "new patient";
    QString birthdate = "";
    QString gender = "";

    QList<QString> ptAttributes;
    ptAttributes << medMetaDataKeys::PatientName.label();
    ptAttributes << medMetaDataKeys::BirthDate.label();
    ptAttributes << medMetaDataKeys::Gender.label();

    QList<QVariant> ptValues;
    ptValues << patientName;
    ptValues << QString("");
    ptValues << QChar();

    medDatabaseEditItemDialog editDialog(ptAttributes, ptValues, this, true, isPersistent);

    int res =  editDialog.exec();

    if(res == QDialog::Accepted)
    {
        patientName = editDialog.value(medMetaDataKeys::PatientName.label()).toString();
        birthdate = editDialog.value(medMetaDataKeys::BirthDate.label()).toString();
        gender = editDialog.value(medMetaDataKeys::Gender.label()).toString();

        // TODO: Hack to be able to create patient using medAbstractData
        // Need to be rethought
        medAbstractData* medData = new medAbstractData();

        QString generatedPatientID = QUuid::createUuid().toString().replace ( "{","" ).replace ( "}","" );

        medData->addMetaData ( medMetaDataKeys::PatientName.key(), QStringList() << patientName );
        medData->addMetaData ( medMetaDataKeys::PatientID.key(), QStringList() << generatedPatientID );
        medData->addMetaData ( medMetaDataKeys::BirthDate.key(), QStringList() << birthdate );
        medData->addMetaData ( medMetaDataKeys::Gender.key(), QStringList() << gender );

        medDataManager::instance()->importData(medData, editDialog.isPersistent());
    }
}

/** Creates a new study */
void medDatabaseView::onCreateStudyRequested(void)
{
    QModelIndexList indexes = this->selectedIndexes();
    if(!indexes.count())
        return;

    QModelIndex index = indexes.at(0);

    medAbstractDatabaseItem *item = NULL;

    if(QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
        item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());

    if (item)
    {
        bool isPersistent = medDataManager::instance()->controllerForDataSource(
                                item->dataIndex().dataSourceId() )->isPersistent();

        int patientNameIndex = item->attributes().indexOf(medMetaDataKeys::PatientName.key());
        int birthdateIndex = item->attributes().indexOf(medMetaDataKeys::BirthDate.key());
        QString patientName = item->data( patientNameIndex ).toString();
        QString birthdate = item->data( birthdateIndex ).toString();
  
        if(birthdate.isNull()) 
            birthdate="";

        QList<QString> stAttributes;
        stAttributes << medMetaDataKeys::StudyDescription.label();

        QList<QVariant> stValues;
        stValues << "new study";

        medDatabaseEditItemDialog editDialog(stAttributes, stValues, this, true, isPersistent);

        int res =  editDialog.exec();

        if(res == QDialog::Accepted)
        {
            QString studyName = editDialog.value(medMetaDataKeys::StudyDescription.label()).toString();

            // TODO: Hack to be able to create study using medAbstractData
            // Need to be rethought
            medAbstractData* medData = new medAbstractData();

            medData->addMetaData ( medMetaDataKeys::PatientName.key(), QStringList() << patientName );
            medData->addMetaData ( medMetaDataKeys::BirthDate.key(), QStringList() << birthdate );
            medData->addMetaData ( medMetaDataKeys::StudyDescription.key(), QStringList() << studyName );
            
            medData->addMetaData ( medMetaDataKeys::StudyID.key(), QStringList() << "0" );
            medData->addMetaData ( medMetaDataKeys::StudyDicomID.key(), QStringList() << "" );

            medDataManager::instance()->importData(medData, editDialog.isPersistent());
        }
    }
}

/** Edits selected item */
void medDatabaseView::onEditRequested(void)
{
    QModelIndexList indexes = this->selectedIndexes();
    if(!indexes.count())
        return;

    QModelIndex index = indexes.at(0);

    medAbstractDatabaseItem *item = NULL;

    if(QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
        item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());

    if(item)
    {        
        QList<QVariant> attributes = item->attributes();
        QList<QVariant> values = item->values();
        QList<QString> labels;
        
        foreach(QVariant attrib, attributes)
        {
            const medMetaDataKeys::Key* key =  medMetaDataKeys::Key::fromKeyName(attrib.toString().toStdString().c_str());
            if(key)
                labels << key->label();
            else labels << "";
        }
        
        medDatabaseEditItemDialog editDialog(labels,values,this);
        
        int res =  editDialog.exec();
        medDataIndex index = item->dataIndex();

        if(res == QDialog::Accepted)
        {
            int i=0;
            foreach(QString label, labels)
            {
                QVariant data = editDialog.value(label);
                QVariant variant = item->attribute(i);
                medDataManager::instance()->setMetadata(index, variant.toString(), data.toString());
                i++;    
            }
        } 
    }

}

/** Called after having failed to open a file. Will add a visual indicator of the failed file. */
void medDatabaseView::onOpeningFailed(const medDataIndex& index)
{
    if (NoFocusDelegate* delegate =
            dynamic_cast<NoFocusDelegate*>(this->itemDelegate()))
    {

        delegate->append(index);
    }
}
