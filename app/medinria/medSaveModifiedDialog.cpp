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
        medDatabaseNonPersistentItem *medItem;
};

/***********************************************/

medSaveModifiedDialog::medSaveModifiedDialog(QWidget *parent) : QDialog(parent), d (new medSaveModifiedDialogPrivate)
{
    QWidget *widget = new QWidget(this);

    QLabel *label = new QLabel(this);
    label->setText("The following data has been created. Do you want save them before closing?");

    d->saveButton = new QPushButton(tr("Save"),this);
    d->deleteButton = new QPushButton(tr("Delete"),this);
    d->selectAllButton = new QPushButton(tr("Select All"),this);
    
    d->treeWidget = new QTreeWidget(this);
    d->treeWidget->setColumnCount(4);

    QStringList headers;
    headers << tr("Name") << tr("Study") << tr("Series") << tr("File");

    d->treeWidget->setHeaderLabels(headers);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(d->saveButton);
    hlayout->addWidget(d->deleteButton);
    hlayout->addWidget(d->selectAllButton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addWidget(d->treeWidget);
    layout->addLayout(hlayout);

    foreach(medDatabaseNonPersistentItem *item, medDatabaseNonPersistentController::instance()->items())
        d->treeWidget->addTopLevelItem(new medSaveModifiedDialogCheckListItem(item));

    d->treeWidget->resizeColumnToContents(0);

    connect (d->saveButton, SIGNAL(clicked()), this, SLOT(Save()));
    connect (d->deleteButton, SIGNAL(clicked()), this, SLOT(Delete()));
    connect (d->selectAllButton, SIGNAL(clicked()), this, SLOT(onSelectAll()));
    connect (d->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(onItemActivated(QTreeWidgetItem *,int)));

    this->setLayout(layout);
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
            medDataManager::instance()->storeNonPersistentSingleDataToDatabase(checkListItem->getMedItem()->index());
        }
    }

    //TODO : refresh treeWidget (remove handled items)
}

void medSaveModifiedDialog::Delete( )
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
}

void medSaveModifiedDialog::onSelectAll()
{
    for (int i = 0; i < d->treeWidget->topLevelItemCount(); ++i)
        {
            medSaveModifiedDialogCheckListItem * checkListItem = (medSaveModifiedDialogCheckListItem*)d->treeWidget->topLevelItem(i);

            checkListItem->setItemState( Qt::Checked );
        }
}
