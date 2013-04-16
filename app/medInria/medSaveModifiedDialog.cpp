/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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
    QMutex mutex;
};

/*********************************/

class medSaveModifiedDialogCheckListItem : public QTreeWidgetItem
{
    public :
        medSaveModifiedDialogCheckListItem(QTreeWidgetItem *root, const medDataIndex &index, const QString &name, const QString &studyName, const QString &seriesName, const QString &file, const QImage &thumb)
        : QTreeWidgetItem(), medIndex(index)
        {
            root->addChild(this);

            setFlags(flags() | Qt::ItemIsUserCheckable);
            setIcon(1, QIcon(QPixmap::fromImage(thumb)));
            setText(2, name);
            setText(3, studyName);
            setText(4, seriesName);
            setText(5, file);
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

medSaveModifiedDialog::medSaveModifiedDialog(QWidget *parent) : QDialog(parent), d (new medSaveModifiedDialogPrivate)
{
    QLabel *label = new QLabel(this);
    label->setText(tr("The following data has been created. Do you want save them ?"));

    d->saveButton = new QPushButton(tr("Save"),this);
    d->doneButton = new QPushButton(tr("Done"),this);

    d->treeWidget = new QTreeWidget(this);
    d->treeWidget->setColumnCount(6);

    QStringList headers;
    headers << tr("Select") <<tr("Thumbnail") << tr("Name") << tr("Study") << tr("Series") << tr("File");

    d->treeWidget->setHeaderLabels(headers);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(d->saveButton);
    hlayout->addWidget(d->doneButton);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(d->treeWidget);
    layout->addLayout(hlayout);

    //TODO a little bit ugly the way medSaveModifiedDialogCheckListItem are parented no ?
    foreach(medDatabaseNonPersistentItem *item, medDatabaseNonPersistentController::instance()->items())
        new medSaveModifiedDialogCheckListItem(d->treeWidget->invisibleRootItem(), item->index(), item->name(), item->studyName(), item->seriesName(), item->file(), item->thumb());

    d->treeWidget->resizeColumnToContents(0);

    connect (d->saveButton, SIGNAL(clicked()), this, SLOT(Save()));
    connect (d->doneButton,SIGNAL(clicked()), this, SLOT(close()));
    connect (medDataManager::instance(), SIGNAL(dataAdded(const medDataIndex &)),this, SLOT(updateCounter()) );

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
    d->counter = 0;

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
        qDebug() << "DEUBG  d->counter" << d->counter;
        d->mutex.lock();
        d->counter--;
        d->mutex.unlock();

        if(d->counter == 0)
            onUpdateTree();
}

void medSaveModifiedDialog::onUpdateTree()
{
        d->treeWidget->clear();

        foreach(medDatabaseNonPersistentItem *item, medDatabaseNonPersistentController::instance()->items())
            d->treeWidget->insertTopLevelItem(0,new medSaveModifiedDialogCheckListItem(d->treeWidget->invisibleRootItem(), item->index(), item->name(), item->studyName(), item->seriesName(), item->file(), item->thumb()));

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
