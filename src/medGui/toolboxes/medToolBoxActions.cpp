#include "medToolBoxActions.h"

#include <QtGui>

#include <medDataManager.h>
#include <medAbstractDbController.h>
#include <medToolBoxBody.h>

class medToolBoxActionsPrivate
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

    QList<QPushButton*> buttonsList;
    QMultiMap<QString, QString> itemToActions;
};

medToolBoxActions::medToolBoxActions( QWidget *parent /*= 0*/ ) : medToolBox(parent), d(new medToolBoxActionsPrivate)
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

    /* Begin create buttons */

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

    /* End create buttons */

    // the order of the buttons in this list determines the order used to place them in the grid layout
    d->buttonsList << d->viewBt << d->loadBt << d->importBt << d->indexBt;
    d->buttonsList << d->removeBt << d->exportBt << d->saveBt << d->bookmarkBt;

    int COLUMNS = 4; // we will use 2 rows of 4 buttons each
    int i = 0;
    QGridLayout *gridLayout = new QGridLayout();
    foreach(QPushButton* bt, d->buttonsList)
    {
        bt->setObjectName("actionToolBoxButton"); // set for style sheet medInria.qss

        // this widget is required to keep the space even if the button is invisible
        QWidget* placeHolder = new QWidget();
        placeHolder->setFixedHeight(38);
        placeHolder->setFixedWidth(58);
        bt->setParent(placeHolder);
        gridLayout->addWidget(placeHolder, i / COLUMNS, i % COLUMNS, Qt::AlignCenter);
        i++;
    }

    d->buttonsWidget->setLayout(gridLayout);
    this->addWidget(d->buttonsWidget);
    d->buttonsWidget->setVisible(false);

    QLabel* noButtonsSelectedLabel = new QLabel(tr("Select any item to see possible actions."));
    noButtonsSelectedLabel->setObjectName("actionToolBoxLabel");
    // we use a layout to center the label
    QHBoxLayout* noButtonsSelectedLayout = new QHBoxLayout();
    noButtonsSelectedLayout->addWidget(noButtonsSelectedLabel, 0, Qt::AlignCenter);
    d->noButtonsSelectedWidget->setLayout(noButtonsSelectedLayout);
    this->addWidget(d->noButtonsSelectedWidget);

    connect(d->removeBt, SIGNAL(clicked()), this, SIGNAL(removeClicked()));
    connect(d->viewBt, SIGNAL(clicked()), this, SIGNAL(viewClicked()));
    connect(d->exportBt, SIGNAL(clicked()), this, SIGNAL(exportClicked()));
    connect(d->bookmarkBt, SIGNAL(clicked()), this, SIGNAL(bookmarkClicked()));
    connect(d->importBt, SIGNAL(clicked()), this, SIGNAL(importClicked()));
    connect(d->loadBt, SIGNAL(clicked()), this, SIGNAL(loadClicked()));
    connect(d->indexBt, SIGNAL(clicked()), this, SIGNAL(indexClicked()));
    connect(d->saveBt, SIGNAL(clicked()), this, SIGNAL(saveClicked()));

    // we keep the size of the toolbox fixed so as it doesn't not resize
    // constantly due to the exchange of the widgets
    this->body()->setFixedHeight(38 + 38 + 35);

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

void medToolBoxActions::noPatientOrSeriesSelected()
{
    updateButtons("None");
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

void medToolBoxActions::updateButtons(QString selectedItem)
{
    QList<QString> actions = d->itemToActions.values(selectedItem);

    foreach(QPushButton* bt, d->buttonsList)
    {
        bool showButton = actions.contains( bt->accessibleName() );
        bt->setVisible(showButton);
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

void medToolBoxActions::initializeItemToActionsMap()
{
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
}
