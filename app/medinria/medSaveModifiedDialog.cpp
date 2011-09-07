#include <medDatabaseController.h>
#include <medDatabaseNonPersistentController.h>
#include <medDatabaseModel.h>
#include <medDatabaseItem.h>
#include <medDatabaseNonPersistentItem.h>

#include <QtGui>

#include "medSaveModifiedDialog.h"

class medSaveModifiedDialogPrivate
{   
public:  

    QTreeWidget *treeWidget;
    QTreeWidgetItem *image;    
    QPushButton *saveButton;
    QPushButton *deleteButton;
    QPushButton *selectAllButton;
};

/*********************************/

//TODO : add thumbnails
class medSaveModifiedDialogCheckListItem : public QTreeWidgetItem
{
    public :
        medSaveModifiedDialogCheckListItem(medDatabaseNonPersistentItem *item) : QTreeWidgetItem(), medItem(item)
        {
            setFlags(flags() | Qt::ItemIsUserCheckable);
            setText(0, item->name());
            setText(1, item->studyName());
            setText(2, item->seriesName());
            setText(3, item->file());
            setCheckState(0, Qt::Checked);
        }

        medDatabaseNonPersistentItem* getMedItem() const
        {
            return medItem;
        }

        ~medSaveModifiedDialogCheckListItem()
        {}

    private:
        medDatabaseNonPersistentItem *medItem;
};

/***********************************************/

medSaveModifiedDialog::medSaveModifiedDialog(QWidget *parent) : QDialog(parent), d (new medSaveModifiedDialogPrivate)
{
    QWidget *widget = new QWidget(this);

    d->saveButton = new QPushButton(tr("Save"),this);
    d->deleteButton = new QPushButton(tr("Delete"),this);
    d->selectAllButton = new QPushButton(tr("Select All"),this);
    
    d->treeWidget = new QTreeWidget(this);
    d->treeWidget->setColumnCount(4); //add thumbnail ?

    QStringList headers;
    headers << tr("Name") << tr("Study") << tr("Series") << tr("File");

    d->treeWidget->setHeaderLabels(headers);

    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->addWidget(d->treeWidget);
    layout->addWidget(d->saveButton);
    layout->addWidget(d->deleteButton);
    layout->addWidget(d->selectAllButton);

    foreach(medDatabaseNonPersistentItem *item, medDatabaseNonPersistentController::instance()->items())
        d->treeWidget->addTopLevelItem(new medSaveModifiedDialogCheckListItem(item));

    d->treeWidget->resizeColumnToContents(0);

    connect (d->saveButton, SIGNAL(clicked()), this, SLOT(onSaveSelected()));
    connect (d->deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteSelected()));
    connect (d->selectAllButton, SIGNAL(clicked()), this, SLOT(onSelectAll()));

    connect (d->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(onItemActivated(QTreeWidgetItem *,int)));
}

medSaveModifiedDialog::~medSaveModifiedDialog()
{
    delete d;
    d = NULL;  
}

bool medSaveModifiedDialog::Save( )
{
    for (int i = 0; i < d->treeWidget->topLevelItemCount(); ++i)
    {
        medSaveModifiedDialogCheckListItem * checkListItem = (medSaveModifiedDialogCheckListItem*)d->treeWidget->topLevelItem(i);

        if (checkListItem->checkState(0) == Qt::Checked)
        {
            medDataManager::instance()->storeNonPersistentSingleDataToDatabase(checkListItem->getMedItem()->index());
        }
    }

    //TODO : refresh treeWidget (remove handled items)
    return true;
}

bool medSaveModifiedDialog::Delete( )
{
    for (int i = 0; i < d->treeWidget->topLevelItemCount(); ++i)
    {
        medSaveModifiedDialogCheckListItem * checkListItem = (medSaveModifiedDialogCheckListItem*)d->treeWidget->topLevelItem(i);

        if (checkListItem->checkState(0) == Qt::Checked)
        {
            medDataManager::instance()->removeData(checkListItem->getMedItem()->index());
        }
    }
    //TODO : refresh treeWidget (remove handled items)
    return true;
}


//        QTreeWidgetItem *parent = currentItem->parent();
//        int index;
//
//        if (parent) {
//            index = parent->indexOfChild(treeWidget->currentItem());
//            delete parent->takeChild(index);
//        } else {
//            index = treeWidget->indexOfTopLevelItem(treeWidget->currentItem());
//            delete treeWidget->takeTopLevelItem(index);
//        }
//
//   Inserting the item somewhere else in the tree widget follows the same pattern:
//
//        QTreeWidgetItem *parent = currentItem->parent();
//        QTreeWidgetItem *newItem;
//        if (parent)
//            newItem = new QTreeWidgetItem(parent, treeWidget->currentItem());
//        else
//            newItem = new QTreeWidgetItem(treeWidget, treeWidget->currentItem());

void medSaveModifiedDialog::onSelectAll()
{
  
}

void medSaveModifiedDialog::onSaveSelected( )
{
  
}


void medSaveModifiedDialog::onDeleteSelected()
{
  
}


// QList<medDatabaseNonPersistentItem *> itemList;
// dtkSmartPointer<dtkAbstractData> data;
// QStringList seriesDescriptions;    
// QTreeWidgetItem *series = new QTreeWidgetItem(d->treeWidget);
// series->setText(0, tr("Series"));



