#ifndef MEDSAVEMODIFIEDDIALOG_H
#define MEDSAVEMODIFIEDDIALOG_H

#include <QFileDialog>
#include <QTreeWidget>
#include <QTreeView>

#include <medCore/medDataIndex.h>

class QTreeWidget;
class QTreeWidgetItem;
class medSaveModifiedDialogPrivate;

class medSaveModifiedDialog: public QFileDialog
{
    Q_OBJECT
    
public:
  
    medSaveModifiedDialog(QWidget *parent);
    virtual ~medSaveModifiedDialog();
    
//    static bool queryClose(QWidget *parent, QList<medDataIndex *indexNonPersistent);
    
    void RetrieveItems();    
    bool Save();    
    bool Delete();    
    
public slots:    
    
    void ItemActivated(QTreeWidgetItem*, int);
    
    void onSelectAll();    
    void onSaveSelected();    
    void onDeleteSelected();

private:
  
    medSaveModifiedDialogPrivate *d;
};







































#endif // MEDSAVEMODIFIEDDIALOG_H






