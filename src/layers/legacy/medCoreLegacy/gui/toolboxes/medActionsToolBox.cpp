/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medActionsToolBox.h>

#include <QtGui>

#include <medDataManager.h>
#include <medAbstractDbController.h>
#include <medToolBoxBody.h>

class medActionsToolBoxPrivate
{
public:

    QWidget* buttonsWidget;
    QWidget* noButtonsSelectedWidget;

    QPushButton* removeBt;
    QPushButton* viewBt;
    QPushButton* exportBt;
    QPushButton* bookmarkBt;
    QPushButton* importBt;
    QPushButton* loadBt;
    QPushButton* indexBt;
    QPushButton* saveBt;
    QPushButton* newPatientBt;
    QPushButton* newStudyBt;
    QPushButton* editBt;

    QList<QAbstractButton*> buttonsList;
    QMultiMap<QString, QString> itemToActions;
};

medActionsToolBox::medActionsToolBox( QWidget *parent /*= 0*/, bool FILE_SYSTEM ) : medToolBox(parent), d(new medActionsToolBoxPrivate)
{
    /**
     * This toolbox will show possible action buttons depending on the
     * items that is currently selected in the file system or db browser.
     *
     * Which actions are appropriate to which item is specified in the itemToActions map.
     *
     * It consists of two widgets: one containing the buttons and another one
     * with an explanatory label telling the user that an item must be selected.
     * Both widgets' visibilities are set depending on the selection.
     */

    d->buttonsWidget = new QWidget(this);
    d->noButtonsSelectedWidget = new QWidget(this);

    initializeItemToActionsMap();


    d->viewBt = new QPushButton(d->buttonsWidget);
    d->viewBt->setAccessibleName("View");
    d->viewBt->setText(tr("View"));
    d->viewBt->setToolTip(tr("Load and visualize the currently selected item."));
    d->viewBt->setIcon(QIcon(":/icons/eye.png"));
    connect(d->viewBt, SIGNAL(clicked()), this, SIGNAL(viewClicked()));


    if (FILE_SYSTEM)
    {
        d->loadBt = new QPushButton(d->buttonsWidget);
        d->loadBt->setAccessibleName("Load");
        d->loadBt->setText(tr("Load"));
        d->loadBt->setToolTip(tr("Temporary load the item(s) so as they can be used inside medInria,\nbut do not include them in the database."));
        d->loadBt->setIcon(QIcon(":/icons/document-open.png"));

        d->importBt = new QPushButton(d->buttonsWidget);
        d->importBt->setAccessibleName("Import");
        d->importBt->setText(tr("Import"));
        d->importBt->setToolTip(tr("Import (copy) item(s) into medInria's database."));
        d->importBt->setIcon(QIcon(":/icons/import.png"));

        d->indexBt = new QPushButton(d->buttonsWidget);
        d->indexBt->setAccessibleName("Index");
        d->indexBt->setText(tr("Index"));
        d->indexBt->setToolTip(tr("Include the item(s) into medInria's database but do not import (copy) them."));
        d->indexBt->setIcon(QIcon(":/icons/finger.png"));

        d->bookmarkBt = new QPushButton(d->buttonsWidget);
        d->bookmarkBt->setAccessibleName("Bookmark");
        d->bookmarkBt->setText(tr("Bookmark"));
        d->bookmarkBt->setToolTip(tr("Bookmark selected folder/resource."));
        d->bookmarkBt->setIcon(QIcon(":/icons/star.svg"));
        
        connect(d->bookmarkBt, SIGNAL(clicked()), this, SIGNAL(bookmarkClicked()));
        connect(d->importBt, SIGNAL(clicked()), this, SIGNAL(importClicked()));
        connect(d->loadBt, SIGNAL(clicked()), this, SIGNAL(loadClicked()));
        connect(d->indexBt, SIGNAL(clicked()), this, SIGNAL(indexClicked()));

        // the order of the buttons in this list determines the order used to place them in the grid layout
        d->buttonsList << d->viewBt << d->loadBt << d->importBt << d->indexBt <<d->bookmarkBt;}

    else //IF DATABASE
    {
        d->saveBt = new QPushButton(d->buttonsWidget);
        d->saveBt->setAccessibleName("Save");
        d->saveBt->setText(tr("Save"));
        d->saveBt->setToolTip(tr("Save selected item into the database."));
        d->saveBt->setIcon(QIcon(":/icons/save.png"));
    
        d->newPatientBt = new QPushButton(d->buttonsWidget);
        d->newPatientBt->setAccessibleName("New Patient");
        d->newPatientBt->setText("Patient");
        d->newPatientBt->setToolTip(tr("Create a new patient."));
        d->newPatientBt->setIcon(QIcon(":/icons/user_add.png"));
    
        d->newStudyBt = new QPushButton(d->buttonsWidget);
        d->newStudyBt->setAccessibleName("New Study");
        d->newStudyBt->setText("Study");
        d->newStudyBt->setToolTip(tr("Create a new study."));
        d->newStudyBt->setIcon(QIcon(":/icons/page_add.png"));
    
        d->editBt = new QPushButton(d->buttonsWidget);
        d->editBt->setAccessibleName("Edit");
        d->editBt->setText("Edit");
        d->editBt->setToolTip(tr("Edit item."));
        d->editBt->setIcon(QIcon(":/icons/page_edit.png"));

        d->exportBt = new QPushButton(d->buttonsWidget);
        d->exportBt->setAccessibleName("Export");
        d->exportBt->setText(tr("Export"));
        d->exportBt->setToolTip(tr("Export the series."));
        d->exportBt->setIcon(QIcon(":/icons/export.png"));

        d->removeBt = new QPushButton(d->buttonsWidget);
        d->removeBt->setAccessibleName("Remove");
        d->removeBt->setText(tr("Remove"));
        d->removeBt->setToolTip(tr("Remove selected item from the database."));
        d->removeBt->setIcon(QIcon(":/icons/cross.svg"));

        connect(d->removeBt, SIGNAL(clicked()), this, SIGNAL(removeClicked()));
        connect(d->exportBt, SIGNAL(clicked()), this, SIGNAL(exportClicked()));
        connect(d->saveBt, SIGNAL(clicked()), this, SIGNAL(saveClicked()));
        connect(d->newPatientBt, SIGNAL(clicked()), this, SIGNAL(newPatientClicked()));
        connect(d->newStudyBt, SIGNAL(clicked()), this, SIGNAL(newStudyClicked()));
        connect(d->editBt, SIGNAL(clicked()), this, SIGNAL(editClicked()));
        
        d->buttonsList << d->viewBt << d->saveBt << d->exportBt << d->removeBt;
        d->buttonsList << d->newPatientBt << d->newStudyBt << d->editBt;
    }

    int COLUMNS = 4; // we will use 3 rows of 4 buttons each
    int i = 0;
    QGridLayout *gridLayout = new QGridLayout(d->buttonsWidget);
    gridLayout->setHorizontalSpacing(4);

    foreach(QAbstractButton* bt, d->buttonsList)
    {
        bt->setAutoFillBackground(true);
        bt->setObjectName("actionToolBoxButton"); // set for style sheet medInria.qss
        gridLayout->addWidget(bt, (int)i/COLUMNS, (int)i%COLUMNS);
        i++;
    }

    this->addWidget(d->buttonsWidget);
    d->buttonsWidget->setVisible(false);

    QLabel* noButtonsSelectedLabel = new QLabel(
                tr("Select any item to see possible actions."),
                d->noButtonsSelectedWidget);
    noButtonsSelectedLabel->setObjectName("actionToolBoxLabel");
    // we use a layout to center the label
    QHBoxLayout* noButtonsSelectedLayout = new QHBoxLayout(d->noButtonsSelectedWidget);
    noButtonsSelectedLayout->addWidget(noButtonsSelectedLabel, 0, Qt::AlignCenter);
    this->addWidget(d->noButtonsSelectedWidget);

    this->setTitle(tr("Actions"));
}

medActionsToolBox::~medActionsToolBox()
{
//    delete d->itemToActions;
//    d->itemToActions = NULL;
    delete d;
    d = NULL;
}

/**
* Slot to call when an item representing a patient has been selected.
* The appropriate buttons will appear in the toolbox.
* @param index – the medDataIndex of the db item
**/
void medActionsToolBox::patientSelected(const medDataIndex& index)
{
    if( !(medDataManager::instance()->controllerForDataSource(index.dataSourceId())->isPersistent()) )
        updateButtons("Unsaved Patient");
    else
        updateButtons("Patient");
}

/**
* Slot to call when an item representing a series has been selected.
* The appropriate buttons will appear in the toolbox.
* @param index – the medDataIndex of the db item
**/
void medActionsToolBox::seriesSelected(const medDataIndex& index)
{
    if( !(medDataManager::instance()->controllerForDataSource(index.dataSourceId())->isPersistent()) )
        updateButtons("Unsaved Series");
    else
        updateButtons("Series");
}

/**
* Slot to call when the items selected in the db browser change
* and no item is selected. Hence no buttons are displayed.
**/
void medActionsToolBox::noPatientOrSeriesSelected()
{
    updateButtons("None");
}

void medActionsToolBox::multipleEntriesSelected(const QVector<medDataIndex> &selectedIndexes)
{
    updateButtons("Multiple Select");
}

/**
* Slot to call when items representing files and/or folders have been selected.
* The appropriate buttons will appear in the toolbox.
* @param paths – a QStringList with all the selected paths
**/
void medActionsToolBox::selectedPathsChanged(const QStringList& paths)
{
    bool containsFolders = false;
    bool containsFiles = false;

    foreach(QString path, paths)
    {
        QFileInfo fi(path);

        if (fi.isDir())
            containsFolders = true;
        else
            containsFiles = true;
    }

    if (containsFolders && containsFiles)
        updateButtons("Files & Folders");
    else if (containsFolders)
        updateButtons("Folders");
    else if (containsFiles)
        updateButtons("Files");
    else
        updateButtons("None");
}

void medActionsToolBox::updateButtons(QString selectedItem)
{
    QList<QString> actions = d->itemToActions.values(selectedItem);

    foreach(QAbstractButton* bt, d->buttonsList)
    {
        bt->setVisible(true); 
        bool showButton = actions.contains( bt->accessibleName() );
        bt->setEnabled(showButton); // Not accessible buttons are disabled
    }

    // insert an explanatory label if no button is being displayed
    if(actions.size() == 0)
    {
        d->noButtonsSelectedWidget->setVisible(true);
        d->buttonsWidget->setVisible(false);
    }
    else
    {
        d->noButtonsSelectedWidget->setVisible(false);
        d->buttonsWidget->setVisible(true);
    }
}

void medActionsToolBox::initializeItemToActionsMap()
{
    d->itemToActions = QMultiMap<QString, QString>();

    d->itemToActions.insert("Patient", "Remove");
    d->itemToActions.insert("Patient", "New Patient");
    d->itemToActions.insert("Patient", "New Study");
    d->itemToActions.insert("Patient", "Edit");

    d->itemToActions.insert("Unsaved Patient", "Remove");
    d->itemToActions.insert("Unsaved Patient", "Save");
    d->itemToActions.insert("Unsaved Patient", "New Patient");
    d->itemToActions.insert("Unsaved Patient", "New Study");
    d->itemToActions.insert("Unsaved Patient", "Edit");

    d->itemToActions.insert("Series", "View");
    d->itemToActions.insert("Series", "Export");
    d->itemToActions.insert("Series", "Remove");
    d->itemToActions.insert("Series", "New Patient");
    d->itemToActions.insert("Series", "Edit");

    d->itemToActions.insert("Unsaved Series", "Remove");
    d->itemToActions.insert("Unsaved Series", "Save");
    d->itemToActions.insert("Unsaved Series", "View");
    d->itemToActions.insert("Unsaved Series", "Export");
    d->itemToActions.insert("Unsaved Series", "New Patient");
    d->itemToActions.insert("Unsaved Series", "Edit");

    d->itemToActions.insert("Folders", "Bookmark");
    d->itemToActions.insert("Folders", "Import");
    d->itemToActions.insert("Folders", "Index");
    d->itemToActions.insert("Folders", "Load");
    d->itemToActions.insert("Folders", "View");

    d->itemToActions.insert("Files", "Import");
    d->itemToActions.insert("Files", "Index");
    d->itemToActions.insert("Files", "Load");
    d->itemToActions.insert("Files", "View");

    d->itemToActions.insert("Files & Folders", "Import");
    d->itemToActions.insert("Files & Folders", "Index");
    d->itemToActions.insert("Files & Folders", "Load");
    d->itemToActions.insert("Files & Folders", "View");

    d->itemToActions.insert("Multiple Select", "Remove");
}
