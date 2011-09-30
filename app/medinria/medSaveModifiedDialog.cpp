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
    QPushButton *doneButton;
//    QPushButton *deleteButton;
//    QPushButton *selectAllButton;
};

/*********************************/

class medSaveModifiedDialogCheckListItem : public QTreeWidgetItem
{
    public :
        medSaveModifiedDialogCheckListItem(medDatabaseNonPersistentItem *item) : QTreeWidgetItem(), medItem(item)
        {
            setFlags(flags() | Qt::ItemIsUserCheckable);
            setIcon(1, QIcon( medMetaDataKeys::SeriesThumbnail.getFirstValue(item->data(),":icons/layer.png")) );
            setText(2, item->name());
            setText(3, item->studyName());
            setText(4, item->seriesName());
            setText(5, item->file());
            setCheckState(0, Qt::Unchecked);
        }

        medDatabaseNonPersistentItem* getMedItem() const
        {
            return medItem;
        }

        void setItemState(Qt::CheckState state)
        {
             setCheckState(0, state);
        }

        ~medSaveModifiedDialogCheckListItem()
        {}

    private:
        medDatabaseNonPersistentItem * medItem;
};

/***********************************************/

medSaveModifiedDialog::medSaveModifiedDialog(QWidget *parent) : QDialog(parent), d (new medSaveModifiedDialogPrivate)
{
    QLabel *label = new QLabel(this);
    label->setText(tr("The following data has been created. Do you want save them ?"));

    d->saveButton = new QPushButton(tr("Save"),this);
    d->doneButton = new QPushButton(tr("Done"),this);
//    d->deleteButton = new QPushButton(tr("Delete"),this);
//    d->selectAllButton = new QPushButton(tr("Select All"),this);

    d->treeWidget = new QTreeWidget(this);
    d->treeWidget->setColumnCount(5);

    QStringList headers;
    headers << tr("Thumbnail") << tr("Name") << tr("Study") << tr("Series") << tr("File");

    d->treeWidget->setHeaderLabels(headers);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(d->saveButton);
    hlayout->addWidget(d->doneButton);
//    hlayout->addWidget(d->deleteButton);
//    hlayout->addWidget(d->selectAllButton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addWidget(d->treeWidget);
    layout->addLayout(hlayout);

    foreach(medDatabaseNonPersistentItem *item, medDatabaseNonPersistentController::instance()->items())
        d->treeWidget->addTopLevelItem(new medSaveModifiedDialogCheckListItem(item));

    d->treeWidget->resizeColumnToContents(0);

    connect (d->saveButton, SIGNAL(clicked()), this, SLOT(Save()));
    //connect (d->deleteButton, SIGNAL(clicked()), this, SLOT(Delete()));
    //connect (d->selectAllButton, SIGNAL(clicked()), this, SLOT(onSelectAll()));
    connect (d->doneButton,SIGNAL(clicked()), this, SLOT(close()));
    connect (medDataManager::instance(), SIGNAL(dataAdded(const medDataIndex &)),this, SLOT(updateTree(const medDataIndex &)) );

    this->setLayout(layout);
    setModal(true);
}

medSaveModifiedDialog::~medSaveModifiedDialog()
{
    delete d;
    d = NULL;
}

void medSaveModifiedDialog::Save( )
{
    for (int i = 0; i < d->treeWidget->topLevelItemCount(); ++i)
    {
        medSaveModifiedDialogCheckListItem * checkListItem = (medSaveModifiedDialogCheckListItem*)d->treeWidget->topLevelItem(i);

        if (checkListItem->checkState(0) == Qt::Checked)
        {
            qDebug() << "entering method Save()" << " index = " << checkListItem->getMedItem()->index();
            medDataManager::instance()->storeNonPersistentSingleDataToDatabase(checkListItem->getMedItem()->index());
        }
    }
    //this->close();
}

void medSaveModifiedDialog::updateTree(const medDataIndex & index)
{
    qDebug() << "entering method updateTree(..)";
    for (int i = 0; i < d->treeWidget->topLevelItemCount(); ++i)
    {
        medSaveModifiedDialogCheckListItem * checkListItem = (medSaveModifiedDialogCheckListItem*)d->treeWidget->topLevelItem(i);

        if (checkListItem->getMedItem()->index() == index)
            d->treeWidget->removeItemWidget(d->treeWidget->topLevelItem(i),0);
    }
}

//void medSaveModifiedDialog::Delete( )
//{
//    for (int i = 0; i < d->treeWidget->topLevelItemCount(); ++i)
//    {
//        medSaveModifiedDialogCheckListItem * checkListItem = (medSaveModifiedDialogCheckListItem*)d->treeWidget->topLevelItem(i);
//
//        if (checkListItem->checkState(0) == Qt::Checked)
//        {
//            medDataManager::instance()->removeData(checkListItem->getMedItem()->index());
//        }
//    }
//    this->close();
//}

//void medSaveModifiedDialog::onSelectAll()
//{
//    for (int i = 0; i < d->treeWidget->topLevelItemCount(); ++i)
//        {
//            medSaveModifiedDialogCheckListItem * checkListItem = (medSaveModifiedDialogCheckListItem*)d->treeWidget->topLevelItem(i);
//
//            checkListItem->setItemState( Qt::Checked );
//        }
//}
