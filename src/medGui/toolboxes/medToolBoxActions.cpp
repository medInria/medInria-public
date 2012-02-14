#include "medToolBoxActions.h"

#include <QtGui>

#include <medDataManager.h>
#include <medAbstractDbController.h>

class medToolBoxActionsPrivate
{
public:

    QWidget *page;

    QPushButton* removeBt;
    QPushButton* viewBt;
    QPushButton* exportBt;
    QPushButton* bookmarkBt;
    QPushButton* importBt;
    QPushButton* loadBt;
    QPushButton* indexBt;
    QPushButton* saveBt;
    QLabel* placeholder;

    QList<QPushButton*> btList;
    QMultiMap<QString, QString> itemToActions;
};

medToolBoxActions::medToolBoxActions( QWidget *parent /*= 0*/ ) : medToolBox(parent), d(new medToolBoxActionsPrivate)
{
    d->page = new QWidget(this);

    /* Begin Initialization of itemsToActions map */

    d->itemToActions = *(new QMultiMap<QString, QString>());

    d->itemToActions.insert("Patient", "Remove");

    d->itemToActions.insert("Unsaved Patient", "Remove");
    d->itemToActions.insert("Unsaved Patient", "Save");

    d->itemToActions.insert("Series", "View");
    d->itemToActions.insert("Series", "Export");
    d->itemToActions.insert("Series", "Remove");

    d->itemToActions.insert("Unsaved Series", "Remove");
    d->itemToActions.insert("Unsaved Series", "Save");
    d->itemToActions.insert("Unsaved Series", "View");
    d->itemToActions.insert("Unsaved Series", "Export");

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

    /* End Initialization of itemsToActions map */

    d->removeBt = new QPushButton(tr("Remove"));
    d->removeBt->setAccessibleName("Remove");
    d->removeBt->setToolTip(tr("Remove selected item from the database."));
    d->viewBt = new QPushButton(tr("View"));
    d->viewBt->setAccessibleName("View");
    d->viewBt->setToolTip(tr("Load and visualize the currently selected item."));
    d->exportBt = new QPushButton(tr("Export"));
    d->exportBt->setAccessibleName("Export");
    d->exportBt->setToolTip(tr("Export the series."));
    d->importBt = new QPushButton(tr("Import"));
    d->importBt->setAccessibleName("Import");
    d->importBt->setToolTip(tr("Import (copy) item(s) into medInria's database."));
    d->loadBt = new QPushButton(tr("Load"));
    d->loadBt->setAccessibleName("Load");
    d->loadBt->setToolTip(tr("Temporary load the item(s) so as they can be used inside medInria,\nbut do not include them in the database."));
    d->indexBt = new QPushButton(tr("Index"));
    d->indexBt->setAccessibleName("Index");
    d->indexBt->setToolTip(tr("Include the item(s) into medInria's database but do not import (copy) them."));
    d->bookmarkBt = new QPushButton(tr("Bookmark"));
    d->bookmarkBt->setAccessibleName("Bookmark");
    d->bookmarkBt->setToolTip(tr("Bookmark selected folder/resource."));
    d->saveBt = new QPushButton(tr("Save"));
    d->saveBt->setAccessibleName("Save");
    d->saveBt->setToolTip(tr("Save selected item into the database."));

    d->btList = *(new QList<QPushButton*>());
    d->btList << d->removeBt << d->viewBt << d->exportBt << d->bookmarkBt << d->importBt << d->loadBt << d->indexBt << d->saveBt;

    foreach(QPushButton* bt, d->btList)
    {
        bt->setVisible(false);
        bt->setObjectName("actionToolBoxButton"); // set for style sheet medInria.qss
    }

    d->placeholder = new QLabel(tr("Select an item to see possible actions."));
    d->placeholder->setObjectName("actionToolBoxLabel");

    QGridLayout *glayout = new QGridLayout();
    glayout->addWidget(d->removeBt, 0, 0, Qt::AlignCenter);
    glayout->addWidget(d->viewBt, 0, 1, Qt::AlignCenter);
    glayout->addWidget(d->exportBt, 0, 2, Qt::AlignCenter);
    glayout->addWidget(d->importBt, 1, 0, Qt::AlignCenter);
    glayout->addWidget(d->loadBt, 1, 1, Qt::AlignCenter);
    glayout->addWidget(d->indexBt, 1, 2, Qt::AlignCenter);
    glayout->addWidget(d->saveBt, 2, 0, Qt::AlignCenter);
    glayout->addWidget(d->bookmarkBt, 2, 1, Qt::AlignCenter);

    glayout->addWidget(d->placeholder, 3, 1, 1, 3, Qt::AlignCenter);

    d->page->setLayout(glayout);
    this->addWidget(d->page);

    connect(d->removeBt, SIGNAL(clicked()), this, SIGNAL(removeClicked()));
    connect(d->viewBt, SIGNAL(clicked()), this, SIGNAL(viewClicked()));
    connect(d->exportBt, SIGNAL(clicked()), this, SIGNAL(exportClicked()));
    connect(d->bookmarkBt, SIGNAL(clicked()), this, SIGNAL(bookmarkClicked()));
    connect(d->importBt, SIGNAL(clicked()), this, SIGNAL(importClicked()));
    connect(d->loadBt, SIGNAL(clicked()), this, SIGNAL(loadClicked()));
    connect(d->indexBt, SIGNAL(clicked()), this, SIGNAL(indexClicked()));
    connect(d->saveBt, SIGNAL(clicked()), this, SIGNAL(saveClicked()));

    this->setTitle(tr("Actions"));
}

medToolBoxActions::~medToolBoxActions()
{
    delete d;
    d = NULL;
}

void medToolBoxActions::patientSelected(const medDataIndex& index)
{
    if( !(medDataManager::instance()->controllerForDataSource(index.dataSourceId())->isPersistent()) )
        updateButtons("Unsaved Patient");
    else
        updateButtons("Patient");
}

void medToolBoxActions::seriesSelected(const medDataIndex& index)
{
    if( !(medDataManager::instance()->controllerForDataSource(index.dataSourceId())->isPersistent()) )
        updateButtons("Unsaved Series");
    else
        updateButtons("Series");
}

void medToolBoxActions::selectedPathsChanged(const QStringList& paths)
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

void medToolBoxActions::updateButtons(QString item)
{
    QList<QString> actions = d->itemToActions.values(item);

    foreach(QPushButton* bt, d->btList)
    {
        if( actions.contains( bt->accessibleName()) )
        {
            bt->setVisible(true);
        }
        else
        {
           bt->setVisible(false);
        }
    }

    // insert a placeholder if no button is being displayed
    if(actions.size() == 0)
        d->placeholder->setVisible(true);
    else
        d->placeholder->setVisible(false);
}
