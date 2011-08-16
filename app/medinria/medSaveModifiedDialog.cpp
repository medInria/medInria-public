#include <medDatabaseController.h>
#include <medDatabaseNonPersistentController.h>
#include <medDatabaseModel.h>
#include <medDatabaseItem.h>

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

medSaveModifiedDialog::medSaveModifiedDialog(QWidget *parent) : QFileDialog(parent), d (new medSaveModifiedDialogPrivate)
{  
    d->saveButton = new QPushButton(tr("Save"),this);
    connect (d->saveButton, SIGNAL(clicked()), this, SLOT(onSaveSelected()));
    
    d->deleteButton = new QPushButton(tr("Delete"),this);
    connect (d->deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteSelected()));
    
    d->selectAllButton = new QPushButton(tr("Select All"),this);
    connect (d->selectAllButton, SIGNAL(clicked()), this, SLOT(onSelectAll()));        
    
  
  
  
}

medSaveModifiedDialog::~medSaveModifiedDialog()
{
    delete d;
    d = NULL;  
}


void medSaveModifiedDialog::RetrieveItems()
{
    QList<medDataIndex *> indexes;
    dtkSmartPointer<dtkAbstractData> data;
    QStringList seriesDescriptions;    
    QTreeWidgetItem *series = new QTreeWidgetItem(d->treeWidget);
    series->setText(0, tr("Series"));
    
    QTreeWidgetItem *seriesItem = new QTreeWidgetItem(series);    
      
    foreach(medDatabaseNonPersistentItem *item, medDatabaseNonPersistentController::instance()->items())
        indexes.append(item->index());
  
    for (int i = 0; i < indexes.size(); ++i) 
    {
	seriesDescriptions.append(data->data(indexes.at(i))->metadata(tr("SeriesDescription")));
	seriesItem->setText(i, seriesDescriptions.at(i));
    }      
}

void medSaveModifiedDialog::ItemActivated(QTreeWidgetItem*, int)
{
  
}

bool medSaveModifiedDialog::Save( )
{
  
}

bool medSaveModifiedDialog::Delete( )
{
  
}


void medSaveModifiedDialog::onSelectAll()
{
  
}

void medSaveModifiedDialog::onSaveSelected( )
{
  
}


void medSaveModifiedDialog::onDeleteSelected()
{
  
}


