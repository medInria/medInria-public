/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseMetadataItemDialog.h>
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
        medAbstractDatabaseItem *item = nullptr;

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

            medAbstractDbController * dbc = medDataManager::instance().controllerForDataSource(item->dataIndex().dataSourceId());
            if ( dbc ) {
                if(!dbc->isPersistent())
                {
                    itemOption.font.setItalic(true);
                    QString name = item->data(0).toString();
                    if (!dbc->isDataLoaded(item->dataIndex()))
                    {
                        if (!name.endsWith(" (*)"))
                        {
                            name = name.append(" (*)");
                            item->setData(0, QVariant(name));
                        }
                    }
                    else
                    {
                        if (name.endsWith(" (*)"))
                        {
                            name.remove(name.length() - 4, 4);
                            item->setData(0, QVariant(name));
                        }
                    }
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
    QAction *metadataAction;
    QAction *retrieveAction;
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
    this->sortByColumn(0, Qt::AscendingOrder);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->header()->setStretchLastSection(true);
    this->header()->setDefaultAlignment(Qt::AlignCenter);
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    // Selected Clicked: "Editing starts when clicking on an already selected item."
    // Edit Key: F2 for instance on Linux/Windows.
    this->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);

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

    d->metadataAction = new QAction(tr("Metadata"), this);
    d->metadataAction->setIconVisibleInMenu(true);
    d->metadataAction->setIcon(QIcon(":icons/information.png"));
    connect(d->metadataAction, SIGNAL(triggered()), this, SLOT(onMetadataRequested()));

    d->retrieveAction = new QAction(tr("Retrieve"), this);
    d->retrieveAction->setIconVisibleInMenu(true);
    d->retrieveAction->setIcon(QIcon(":icons/import.png"));
    connect(d->retrieveAction, SIGNAL(triggered()), this, SLOT(onRetrieveDataRequested()));
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
    // If we have selected multiple rows, we'll only permit *remove* for now, as that was what was requested in Issue 198
    // Further options can be added, based upon discussion - MJB 08/01/15
    if (this->selectionModel()->selectedRows().count() > 1)
    {
        d->contextMenu->clear();
        d->contextMenu->addAction(d->removeAction);
    }
    else // single row behaviour
    {
        QModelIndex index = this->indexAt(point);

        d->contextMenu->clear();
        d->contextMenu->addAction(d->addPatientAction);

        medAbstractDatabaseItem *item = nullptr;

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
                d->contextMenu->addAction(d->metadataAction);
                d->contextMenu->addAction(d->removeAction);
                if (!(medDataManager::instance().controllerForDataSource(item->dataIndex().dataSourceId())->isPersistent()))
                {
                    d->contextMenu->addAction(d->saveAction);
                }
                if (item->dataIndex().dataSourceId() == 3)
                {
                    d->contextMenu->addAction(d->retrieveAction);
                }
            }
            else if (item->dataIndex().isValidForStudy())
            {
                d->contextMenu->addAction(d->editAction);
                d->editAction->setIcon(QIcon(":icons/page_edit.png"));
                d->contextMenu->addAction(d->removeAction);
                if (!(medDataManager::instance().controllerForDataSource(item->dataIndex().dataSourceId())->isPersistent()))
                {
                    d->contextMenu->addAction(d->saveAction);
                }
                if (item->dataIndex().dataSourceId() == 3)
                {
                    d->contextMenu->addAction(d->retrieveAction);
                }
            }
            else if (item->dataIndex().isValidForPatient())
            {
                d->contextMenu->addAction(d->addStudyAction);
                d->contextMenu->addAction(d->editAction);
                d->editAction->setIcon(QIcon(":icons/user_edit.png"));
                d->contextMenu->addAction(d->removeAction);
                if (!(medDataManager::instance().controllerForDataSource(item->dataIndex().dataSourceId())->isPersistent()))
                {
                    d->contextMenu->addAction(d->saveAction);
                }
                if (item->dataIndex().dataSourceId() == 3)
                {
                    d->contextMenu->addAction(d->retrieveAction);
                }
            }
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
        emit (open(item->dataIndex()));
}

/** Opens the currently selected item. */
void medDatabaseView::onViewSelectedItemRequested()
{
    // Called when the user right click->View in DB on a series/study

    if(!this->selectedIndexes().count())
        return;

    QModelIndex index = this->selectedIndexes().first();

    if(!index.isValid())
        return;

    medAbstractDatabaseItem *item = nullptr;

    if(QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
        item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());

    if (item && (item->dataIndex().isValidForSeries() || item->dataIndex().isValidForStudy()))
    {
        emit open(item->dataIndex ());
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

    medAbstractDatabaseItem *item = nullptr;

    if(QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
        item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());

    if(item)
        emit exportData(item->dataIndex());
}

void medDatabaseView::onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(deselected);
    if (selected.count() == 0)
    {
        emit noPatientOrSeriesSelected();
        return;
    }

    // If we're already holding down CTRL or SHIFT, change behaviour to not open but rather just select, so we can select multiple elements
    Qt::KeyboardModifiers activeKeyboardModifiers = QApplication::keyboardModifiers();

    if (activeKeyboardModifiers & (Qt::ControlModifier | Qt::ShiftModifier))
    {
        QVector<medDataIndex> test;
        for (int i = 0; i < selected.size(); i++)
        {
            QModelIndex index = selected.indexes()[i];
            medAbstractDatabaseItem* item = getItemFromIndex(index);

            if (item)
                test.push_back(item->dataIndex());

        }
        emit multipleEntriesSelected(test);
        return;
    }
        
    // If we don't have the modifer keys pressed, expand the selected element.
    QModelIndex index = selected.indexes()[0];
    medAbstractDatabaseItem *item = getItemFromIndex(index);

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

medAbstractDatabaseItem* medDatabaseView::getItemFromIndex(const QModelIndex& index)
{
    medAbstractDatabaseItem *item = nullptr;

    if(QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
        item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());
    else if (dynamic_cast<QAbstractItemModel *>(this->model()))
        item = static_cast<medAbstractDatabaseItem *>(index.internalPointer());

    return item;
}

/** Removes the currently selected item. */
void medDatabaseView::onRemoveSelectedItemRequested( void )
{
    const QString dialogueTitle = this->selectionModel()->selectedRows().count() > 1 ? tr("Remove items") : tr("Remove item");

    int reply = QMessageBox::question(this, dialogueTitle,
            tr("Are you sure you want to continue?\n""This cannot be undone."),
            QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);

    if (reply == QMessageBox::Yes)
    {
        for(const QModelIndex& index : this->selectionModel()->selectedRows())
        {
            medAbstractDatabaseItem *item = nullptr;
            if(QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
                item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());

            if (item)
            {
                // Copy the data index, because the data item may cease to be valid.
                medDataIndex index = item->dataIndex();
                medDataManager::instance().removeData(index);
            }
        }
    }
}

/** Saves the currently selected item. */
void medDatabaseView::onSaveSelectedItemRequested(void)
{
    QModelIndexList indexes = this->selectedIndexes();
    if (!indexes.count())
        return;

    QModelIndex index = indexes.at(0);

    medAbstractDatabaseItem *item = nullptr;

    if (QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
        item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());

    if (item)
    {
        // Copy the data index, because the data item may cease to be valid.
        medDataIndex index = item->dataIndex();
        medDataManager::instance().makePersistent(index);
        qDebug() << "onMenuSaveClicked() after storeNonPersistentSingleDataToDatabase";
        qDebug() << "index" << index;
    }
}

void medDatabaseView::onRetrieveDataRequested()
{
    QModelIndexList indexes = this->selectedIndexes();
    if (!indexes.count())
        return;

    QModelIndex index = indexes.at(0);

    medAbstractDatabaseItem *item = nullptr;

    if (QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
        item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());

    if (item)
    {
        // Copy the data index, because the data item may cease to be valid.
        medDataIndex index = item->dataIndex();
        medDataManager::instance().loadData(index);
        qDebug() << "onRetrieveDataRequested()";
        qDebug() << "index" << index;
    }
}

/** Creates a new patient */
void medDatabaseView::onCreatePatientRequested(void)
{
    QModelIndexList indexes = this->selectedIndexes();

    bool isPersistent = true;
    int dataSourceId = 0;
    if (indexes.count() > 0)
    {
        QModelIndex index = indexes.at(0);

        medAbstractDatabaseItem *item = nullptr;

        if (QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
            item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());

        if (item)
        {
            dataSourceId = item->dataIndex().dataSourceId();
            isPersistent = medDataManager::instance().controllerForDataSource(dataSourceId)->isPersistent();
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
    if (dataSourceId == 3)
    {
        editDialog.setEnabled(false);
        return;
    }

    int res = editDialog.exec();

    if (res == QDialog::Accepted)
    {
        patientName = editDialog.value(medMetaDataKeys::PatientName.label()).toString();
        birthdate = editDialog.value(medMetaDataKeys::BirthDate.label()).toString();
        gender = editDialog.value(medMetaDataKeys::Gender.label()).toString();

        // TODO: Hack to be able to create patient using medAbstractData
        // Need to be rethought
        medAbstractData* medData = new medAbstractData();

        QString generatedPatientID = QUuid::createUuid().toString().replace ( "{","" ).replace ( "}","" );

        medData->setMetaData ( medMetaDataKeys::PatientName.key(), QStringList() << patientName );
        medData->setMetaData ( medMetaDataKeys::PatientID.key(), QStringList() << generatedPatientID );
        medData->setMetaData ( medMetaDataKeys::BirthDate.key(), QStringList() << birthdate );
        medData->setMetaData ( medMetaDataKeys::Gender.key(), QStringList() << gender );

        medDataManager::instance().importData(medData, editDialog.isPersistent());
    }
}

/** Creates a new study */
void medDatabaseView::onCreateStudyRequested()
{
    QModelIndexList indexes = this->selectedIndexes();
    if(!indexes.count())
        return;

    QModelIndex index = indexes.at(0);

    medAbstractDatabaseItem *item = nullptr;

    if(QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
        item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());

    if (item)
    {
        bool isPersistent = medDataManager::instance().controllerForDataSource(
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
        if (item->dataIndex().dataSourceId() == 3)
        {
            editDialog.setEnabled(false);
            return;
        }

        int res = editDialog.exec();

        if (res == QDialog::Accepted)
        {
            QString studyName = editDialog.value(medMetaDataKeys::StudyDescription.label()).toString();

            // TODO: Hack to be able to create study using medAbstractData
            // Need to be rethought
            medAbstractData *medData = new medAbstractData();

            medData->setMetaData(medMetaDataKeys::PatientName.key(), QStringList() << patientName);
            medData->setMetaData(medMetaDataKeys::BirthDate.key(), QStringList() << birthdate);
            medData->setMetaData(medMetaDataKeys::StudyDescription.key(), QStringList() << studyName);

            medData->setMetaData(medMetaDataKeys::StudyID.key(), QStringList() << "0");
            medData->setMetaData(medMetaDataKeys::StudyInstanceUID.key(), QStringList() << "");

            medDataManager::instance().importData(medData, editDialog.isPersistent());
        }
    }
}

/** Edits selected item */
void medDatabaseView::onEditRequested()
{
    QModelIndexList indexes = this->selectedIndexes();
    if(!indexes.count())
        return;

    QModelIndex index = indexes.at(0);

    medAbstractDatabaseItem *item = nullptr;

    if(QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
        item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());

    if(item)
    {        
        QList<QVariant> attributes = item->attributes();
        QList<QVariant> values = item->values();
        QList<QString> labels;
        
        // Users are not allowed to change ThumbnailPath attribute.
        for (int i = 0; i<attributes.count(); i++)
        {
            if (attributes.at(i).toString() == "ThumbnailPath")
            {
                attributes.removeAt(i);
                values.removeAt(i);
            }
        }

        for(QVariant attrib : attributes)
        {
            const medMetaDataKeys::Key* key =  medMetaDataKeys::Key::fromKeyName(attrib.toString());
            if(key)
                labels << key->label();
            else labels << "";
        }
        
        medDatabaseEditItemDialog editDialog(labels,values,this);
        
        int res =  editDialog.exec();
        medDataIndex index = item->dataIndex();

        if (index.dataSourceId() == 3)
        {
            editDialog.setEnabled(false);
            return;
        }

        if (res == QDialog::Accepted)
        {
            int i = 0;
            for (QString label : labels)
            {
                QVariant data = editDialog.value(label);
                QVariant variant = item->attribute(i);
                medDataManager::instance().setMetadata(index, variant.toString(), data.toString());
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

/** Display metadata of selected item */
void medDatabaseView::onMetadataRequested(void)
{
    QModelIndexList indexes = this->selectedIndexes();
    if(indexes.count())
    {
        QModelIndex index = indexes.at(0);

        medAbstractDatabaseItem *item = nullptr;

        if(QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(this->model()))
        {
            item = static_cast<medAbstractDatabaseItem *>(proxy->mapToSource(index).internalPointer());
        }

        if (item)
        {
            QVariant metadata;
            QList<QString> keyList;
            QList<QVariant> metadataList;

            // Get back keys and metadata from the selected data
            for(const medMetaDataKeys::Key* key : medMetaDataKeys::Key::all())
            {
                metadata = medDataManager::instance().getMetaData(item->dataIndex(), key->key());

                keyList.push_back(key->key());
                metadataList.push_back(metadata);
            }

            // Create the information dialog
            medDatabaseMetadataItemDialog metadataDialog(keyList, metadataList, this);
            metadataDialog.exec();
        }
    }
}
