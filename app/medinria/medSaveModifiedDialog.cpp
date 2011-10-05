#include <medDatabaseNonPersistentController.h>
#include <medDatabaseNonPersistentItem.h>
#include <medDataIndex.h>

#include <QtGui>

#include "medSaveModifiedDialog.h"

class medSaveModifiedDialogPrivate
{
public:

    QTreeWidget *treeWidget;
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

//TODO : warning ! when importing non-persistent data to the database medDataIndex changes  !
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
    }

    d->treeWidget->resizeColumnToContents(0);

    connect (d->saveButton, SIGNAL(clicked()), this, SLOT(Save()));
//    connect (d->selectAllButton, SIGNAL(clicked()), this, SLOT(onSelectAll()));
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
            medDataManager::instance()->storeNonPersistentSingleDataToDatabase(index);
}

void medSaveModifiedDialog::updateCounter()
{
        d->counter--;

        if(d->counter == 0)
            emit updateTree();
}

void medSaveModifiedDialog::onUpdateTree()
{
        d->treeWidget->clear();

        foreach(medDatabaseNonPersistentItem *item, medDatabaseNonPersistentController::instance()->items())
        {
            qDebug() << "DEBUG updateTree() name" << item->name() << "studyName" << item->studyName() << "seriesName" << item->seriesName() << "file" << item->file();

            QString name(item->name());
            QString studyName(item->studyName());
            QString seriesName(item->seriesName());
            QString file(item->file());

            d->treeWidget->insertTopLevelItem(0,new medSaveModifiedDialogCheckListItem(d->treeWidget->invisibleRootItem(), item->index(), name, studyName, seriesName, file));
        }
        d->treeWidget->update();
        d->treeWidget->resizeColumnToContents(0);
        d->treeWidget->showMaximized();
}

//void medSaveModifiedDialog::onSelectAll()
//{
//    for (int i = 0; i < d->treeWidget->topLevelItemCount(); ++i)
//        {
//            medSaveModifiedDialogCheckListItem * checkListItem = (medSaveModifiedDialogCheckListItem*)d->treeWidget->topLevelItem(i);
//
//            checkListItem->setItemState( Qt::Checked );
//        }
//}
