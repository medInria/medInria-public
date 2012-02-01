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

/**
 * @brief Dialog window displaying non-persistent database items (i.e. unsaved data) when the user is about to close the application
 */
class medSaveModifiedDialog: public QDialog
{
    Q_OBJECT
    
public:
  
    medSaveModifiedDialog(QWidget *parent);
    virtual ~medSaveModifiedDialog();

public slots:

    /**
     * Method to store non-persistent data (according to the tree widget items that are checked) into the database.
     */
    void Save();

    /**
     * Method to decrement a counter (i.e. the number of checked items that remain to be saved).
     */
    void updateCounter();

    /**
     * This method rebuilds the treewidget.
     */
    void onUpdateTree();

private:
  
    medSaveModifiedDialogPrivate *d;
};







































#endif // MEDSAVEMODIFIEDDIALOG_H






