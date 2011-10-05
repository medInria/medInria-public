#include <medDatabaseController.h>
#include <medDatabaseNonPersistentController.h>
#include <medDatabaseModel.h>
#include <medDatabaseItem.h>
#include <medDatabaseNonPersistentItem.h>
#include <medDataIndex.h>

#include <QtGui>

#include "medSaveModifiedDialog.h"

class medSaveModifiedDialogPrivate
{
public:

    QTreeWidget *treeWidget;
    QTreeWidgetItem *image;
    QTreeWidgetItem *root;
    QPushButton *saveButton;
    QPushButton *doneButton;
    unsigned int counter;
//QPushButton *selectAllButton;
};

/*********************************/

class medSaveModifiedDialogCheckListItem : public QTreeWidgetItem
{
    public :
        medSaveModifiedDialogCheckListItem(QTreeWidgetItem *root, const medDataIndex &index, QString &name, QString &studyName, QString &seriesName, QString &file)
        : QTreeWidgetItem(), medIndex(index)
        {
            root->addChild(this);

            setFlags(flags() | Qt::ItemIsUserCheckable);
            setText(1, name);
            setText(2, studyName);
            setText(3, seriesName);
            setText(4, file);
            setCheckState(0, Qt::Unchecked);
        }

        const medDataIndex& getIndex() const
        {
            return medIndex;
        }

        ~medSaveModifiedDialogCheckListItem()
        {}

    private:
        medDataIndex medIndex;
};

/***********************************************/

//TODO : when importing non-persistent data to the database medDataIndex changes  !
medSaveModifiedDialog::medSaveModifiedDialog(QWidget *parent) : QDialog(parent), d (new medSaveModifiedDialogPrivate)
{
    d->counter = 0;

    QLabel *label = new QLabel(this);
    label->setText(tr("The following data has been created. Do you want save them ?"));

    d->saveButton = new QPushButton(tr("Save"),this);
    d->doneButton = new QPushButton(tr("Done"),this);
//    d->selectAllButton = new QPushButton(tr("Select All"),this);

    d->treeWidget = new QTreeWidget(this);
    d->treeWidget->setColumnCount(5);

    QStringList headers;
    headers << tr("Thumbnail") << tr("Name") << tr("Study") << tr("Series") << tr("File");

    d->treeWidget->setHeaderLabels(headers);

//    d->root = d->treeWidget->invisibleRootItem();

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(d->saveButton);
    hlayout->addWidget(d->doneButton);
//    hlayout->addWidget(d->selectAllButton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addWidget(d->treeWidget);
    layout->addLayout(hlayout);

    foreach(medDatabaseNonPersistentItem *item, medDatabaseNonPersistentController::instance()->items())
    {
        QString name(item->name());
        QString studyName(item->studyName());
        QString seriesName(item->seriesName());
        QString file(item->file());

        medSaveModifiedDialogCheckListItem * itemtmp = new medSaveModifiedDialogCheckListItem(d->treeWidget->invisibleRootItem(), item->index(), name, studyName, seriesName, file);

//        d->treeWidget->addTopLevelItem(new medSaveModifiedDialogCheckListItem(d->root, item->index(), name, studyName, seriesName, file));
    }

    d->treeWidget->resizeColumnToContents(0);

    connect (d->saveButton, SIGNAL(clicked()), this, SLOT(Save()));
//connect (d->selectAllButton, SIGNAL(clicked()), this, SLOT(onSelectAll()));
    connect (d->doneButton,SIGNAL(clicked()), this, SLOT(close()));
    connect (medDataManager::instance(), SIGNAL(dataAdded(const medDataIndex &)),this, SLOT(updateCounter()) );
    connect (this, SIGNAL(updateTree()), this, SLOT(onUpdateTree()));

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
    QList<medDataIndex> list;

    for (int i = 0; i < d->treeWidget->topLevelItemCount(); ++i)
    {
        medSaveModifiedDialogCheckListItem * checkListItem = dynamic_cast<medSaveModifiedDialogCheckListItem*> (d->treeWidget->topLevelItem(i));

        if (checkListItem->checkState(0) == Qt::Checked)
        {
            d->counter++;
            list.append(checkListItem->getIndex());
        }
    }

    foreach(medDataIndex index, list)
        {
            //qDebug() << "DEBUG : entering method Save()" << " index = " << checkListItem->getIndex() << "counter = " << d->counter;
            medDataManager::instance()->storeNonPersistentSingleDataToDatabase(index);
        }
    //this->close();
}

void medSaveModifiedDialog::updateCounter()
{
        d->counter--;

        if(d->counter == 0)
            emit updateTree();
}

void medSaveModifiedDialog::onUpdateTree()
{
        qDebug() << "size db :" << medDatabaseNonPersistentController::instance()->items().size();
        d->treeWidget->clear();
        qDebug() << "size db :" << medDatabaseNonPersistentController::instance()->items().size();

        foreach(medDatabaseNonPersistentItem *item, medDatabaseNonPersistentController::instance()->items())
        {
            QString name(item->name());
            QString studyName(item->studyName());
            QString seriesName(item->seriesName());
            QString file(item->file());

            //d->treeWidget->addTopLevelItem(new medSaveModifiedDialogCheckListItem(item->index(), name, studyName, seriesName, file));
            d->treeWidget->insertTopLevelItem(0,new medSaveModifiedDialogCheckListItem(d->root, item->index(), name, studyName, seriesName, file));

            qDebug() << "DEBUG : onUpdateTree(), after addTopLevelItem";
        }
        d->treeWidget->update();
        d->treeWidget->resizeColumnToContents(0);
        d->treeWidget->showMaximized();
}


//void medSaveModifiedDialog::updateTree(const medDataIndex & index)
//{
//    qDebug() << "DEBUG : entering method updateTree(..)";
//
//    for (int i = 0; i < d->treeWidget->topLevelItemCount(); ++i)
//    {
//        qDebug() << "i" << i;
//
//        medSaveModifiedDialogCheckListItem * checkListItem = dynamic_cast<medSaveModifiedDialogCheckListItem*> (d->treeWidget->topLevelItem(i));
//
//        qDebug() << "checkListItem = " << checkListItem;
//
//        qDebug() << "checkListItem->getIndex() : " << checkListItem->getIndex() << " index : " << index;
//
//        if (checkListItem->getIndex() == index)
//        {
//            qDebug() << "IF ?";
//            QTreeWidgetItem *parent = checkListItem->parent();
//
//            int index = d->treeWidget->indexOfTopLevelItem(checkListItem);
//            delete d->treeWidget->takeTopLevelItem(index);
//
//            //d->treeWidget->removeItemWidget(d->treeWidget->topLevelItem(i),0);
//        }
//    }
//
//    qDebug() << "DEBUG : finish";
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
