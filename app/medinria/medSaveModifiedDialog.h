#ifndef MEDSAVEMODIFIEDDIALOG_H
#define MEDSAVEMODIFIEDDIALOG_H

#include <QFileDialog>
#include <QTreeWidget>
#include <QTreeView>

#include <medCore/medDataIndex.h>
#include <medCore/medDataManager.h>

class QTreeWidget;
class QTreeWidgetItem;
class medSaveModifiedDialogPrivate;

class medSaveModifiedDialog: public QDialog
{
    Q_OBJECT
    
public:
  
    medSaveModifiedDialog(QWidget *parent);
    virtual ~medSaveModifiedDialog();

    bool Save();    
    bool Delete();
    
public slots:

    void onSelectAll();    
    void onSaveSelected();    
    void onDeleteSelected();

private:
  
    medSaveModifiedDialogPrivate *d;
};







































#endif // MEDSAVEMODIFIEDDIALOG_H






