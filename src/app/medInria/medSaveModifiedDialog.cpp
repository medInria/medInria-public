/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseNonPersistentController.h>
#include <medDatabaseNonPersistentItem.h>
#include <medDataIndex.h>

#include <QtGui>
#include <QtWidgets>

#include <medSaveModifiedDialog.h>

class medSaveModifiedDialogPrivate
{
public:

    QTreeWidget *treeWidget;
    QPushButton *quitButton;
    QPushButton *saveQuitButton;
    QPushButton *cancelButton;
    
    bool quitRequested;

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
            setCheckState(0, Qt::Checked);
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
    label->setText(tr("The following datasets have been created. Do you want to save them ?"));

    d->saveQuitButton = new QPushButton(tr("Save and Quit"),this);
    d->cancelButton = new QPushButton(tr("Cancel"),this);
    d->quitButton = new QPushButton(tr("Quit without saving"),this);

    d->treeWidget = new QTreeWidget(this);
    d->treeWidget->setColumnCount(6);
    
    d->quitRequested = false;

    QStringList headers;
    headers << tr("Select") <<tr("Thumbnail") << tr("Name") << tr("Study") << tr("Series") << tr("File");

    d->treeWidget->setHeaderLabels(headers);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(d->saveQuitButton);
    hlayout->addWidget(d->cancelButton);
    hlayout->addWidget(d->quitButton);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(d->treeWidget);
    layout->addLayout(hlayout);

    for(medDatabaseNonPersistentItem *item : medDatabaseNonPersistentController::instance().items())
    {
        if ((item->studyName() != "") && (item->seriesName() != ""))
            new medSaveModifiedDialogCheckListItem(d->treeWidget->invisibleRootItem(), item->index(), item->name(), item->studyName(), item->seriesName(), item->file(), item->thumb());
    }
    
    d->treeWidget->resizeColumnToContents(0);

    connect (d->saveQuitButton, SIGNAL(clicked()), this, SLOT(saveAndQuit()));
    connect (d->cancelButton,SIGNAL(clicked()), this, SLOT(reject()));
    connect (d->quitButton,SIGNAL(clicked()), this, SLOT(accept()));

    connect (&medDataManager::instance(), SIGNAL(dataImported(medDataIndex,QUuid)),this, SLOT(updateCounter()));

    setModal(true);
}

medSaveModifiedDialog::~medSaveModifiedDialog()
{
    delete d;
    d = nullptr;
}

void medSaveModifiedDialog::saveAndQuit()
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

    medDataManager &mdm = medDataManager::instance();
    for(medDataIndex index : list)
    {
        mdm.makePersistent(index);
    }

    if (d->counter != 0)
        d->quitRequested = true;
    else
        this->accept();
}

void medSaveModifiedDialog::updateCounter()
{
    d->mutex.lock();
    d->counter--;
    d->mutex.unlock();
    
    if(d->counter == 0)
    {
        if (d->quitRequested)
            this->accept();
        else
            this->onUpdateTree();
    }
}

void medSaveModifiedDialog::onUpdateTree()
{
    d->treeWidget->clear();
    
    for(medDatabaseNonPersistentItem *item : medDatabaseNonPersistentController::instance().items())
    {
        d->treeWidget->insertTopLevelItem(0,new medSaveModifiedDialogCheckListItem(d->treeWidget->invisibleRootItem(),
                                                                                   item->index(), item->name(), item->studyName(),
                                                                                   item->seriesName(), item->file(), item->thumb()));
    }
    
    d->treeWidget->update();
    d->treeWidget->resizeColumnToContents(0);
    d->treeWidget->showMaximized();
}
