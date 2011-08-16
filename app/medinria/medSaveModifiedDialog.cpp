#include <medDatabaseController.h>
#include <medDatabaseNonPersistentController.h>
#include <medDatabaseModel.h>
#include <medDatabaseItem.h>

#include <QtGui>

class medSaveModifiedDialogPrivate
{   
public:  

    QTreeWidget *list;
    QTreeWidgetItem *image;    
    QPushButton *saveButton;
    QPushButton *deleteButton;
    QPushButton *selectAllButton;     
};

medSaveModifiedDialog::medSaveModifiedDialog(QWidget *parent) : QFileDialog(parent), d (medSaveModifiedDialogPrivate)
{
    // getting non-persistent database items
    // foreach(medDatabaseNonPersistentItem *item, medDatabaseNonPersistentController::instance()->items())  
  
    d->saveButton = new QPushButton(tr("Save"),this);
    connect (d->saveButton, SIGNAL(clicked()), this, SLOT(onSaveSelected()));
    
    d->deleteButton = new QPushButton(tr("Delete"),this);
    connect (d->deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteSelected()));
    
    d->selectAllButton = new QPushButton(tr("Select All"),this);
    connect (d->selectAllButton, SIGNAL(clicked()), this, SLOT(onSelectAll()));    
    
    
    
    
  
  
  
}

~medSaveModifiedDialog::medSaveModifiedDialog(void)
{
  delete d;
  d = NULL;  
}


void medSaveModifiedDialog::RetrieveItems()
{
  QList<metaDataIndex *index> list;
  
  foreach(medDatabaseNonPersistentItem *item, medDatabaseNonPersistentController::instance()->items())
      list.append(item->index());
  
  for (int i = 0; i < list.size(); ++i) 
  {
      list.at(i)->
      
      

  }
  
  
  
  
  
  
  int dataSourceId(void) const { return m_dataSourceId; }
    int patientId(void) const { return m_patientId; }
    int   studyId(void) const { return m_studyId; }
    int  seriesId(void) const { return m_seriesId; }
    int   imageId(void) const { return m_imageId; }
  
  
  
  
  QTreeWidgetItem *cities = new QTreeWidgetItem(treeWidget);
     cities->setText(0, tr("Cities"));
     QTreeWidgetItem *osloItem = new QTreeWidgetItem(cities);
     osloItem->setText(0, tr("Oslo"));
     osloItem->setText(1, tr("Yes"));  
  
  
  
  
  
  
  
  
  
  
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


