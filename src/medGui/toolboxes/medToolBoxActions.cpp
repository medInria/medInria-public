#include "medToolBoxActions.h"

#include <QtGui>

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

    d->itemToActions.insert("Series", "View");
    d->itemToActions.insert("Series", "Export");
    d->itemToActions.insert("Series", "Remove");

    d->itemToActions.insert("Folder", "Bookmark");
    d->itemToActions.insert("Folder", "Import");
    d->itemToActions.insert("Folder", "Index");
    d->itemToActions.insert("Folder", "Load");
    d->itemToActions.insert("Folder", "View");

    d->itemToActions.insert("File", "Import");
    d->itemToActions.insert("File", "Index");
    d->itemToActions.insert("File", "Load");
    d->itemToActions.insert("File", "View");

    /* End Initialization of itemsToActions map */

    d->removeBt = new QPushButton(tr("Remove"));
    d->removeBt->setAccessibleName("Remove");
    d->viewBt = new QPushButton(tr("View"));
    d->viewBt->setAccessibleName("View");
    d->exportBt = new QPushButton(tr("Export"));
    d->exportBt->setAccessibleName("Export");
    d->importBt = new QPushButton(tr("Import"));
    d->importBt->setAccessibleName("Import");
    d->loadBt = new QPushButton(tr("Load"));
    d->loadBt->setAccessibleName("Load");
    d->indexBt = new QPushButton(tr("Index"));
    d->indexBt->setAccessibleName("Index");
    d->bookmarkBt = new QPushButton(tr("Bookmark"));
    d->bookmarkBt->setAccessibleName("Bookmark");

    d->btList = *(new QList<QPushButton*>());
    d->btList << d->removeBt << d->viewBt << d->exportBt << d->bookmarkBt << d->importBt << d->loadBt << d->indexBt;

    QSize btSize(76, 52);
    foreach(QPushButton* bt, d->btList)
    {
        bt->setFixedSize(btSize);
        bt->setVisible(false);
    }

    d->placeholder = new QLabel(tr("Select an item to see possible actions."));
    d->placeholder->setMinimumHeight(52);

    QGridLayout *glayout = new QGridLayout();
    glayout->addWidget(d->removeBt, 0, 0, Qt::AlignHCenter & Qt::AlignVCenter);
    glayout->addWidget(d->viewBt, 0, 1, Qt::AlignHCenter & Qt::AlignVCenter);
    glayout->addWidget(d->exportBt, 0, 2, Qt::AlignHCenter & Qt::AlignVCenter);
    glayout->addWidget(d->importBt, 1, 0, Qt::AlignHCenter & Qt::AlignVCenter);
    glayout->addWidget(d->loadBt, 1, 1, Qt::AlignHCenter & Qt::AlignVCenter);
    glayout->addWidget(d->indexBt, 1, 2, Qt::AlignHCenter & Qt::AlignVCenter);
    glayout->addWidget(d->bookmarkBt, 2, 1, Qt::AlignHCenter & Qt::AlignVCenter);

    glayout->addWidget(d->placeholder, 3, 0, 1, 3, Qt::AlignHCenter & Qt::AlignVCenter);

    d->page->setLayout(glayout);
    this->addWidget(d->page);

    connect(d->removeBt, SIGNAL(clicked()), this, SIGNAL(removeClicked()));
    connect(d->viewBt, SIGNAL(clicked()), this, SIGNAL(viewClicked()));
    connect(d->exportBt, SIGNAL(clicked()), this, SIGNAL(exportClicked()));
    connect(d->bookmarkBt, SIGNAL(clicked()), this, SIGNAL(bookmarkClicked()));
    connect(d->importBt, SIGNAL(clicked()), this, SIGNAL(importClicked()));
    connect(d->loadBt, SIGNAL(clicked()), this, SIGNAL(loadClicked()));
    connect(d->indexBt, SIGNAL(clicked()), this, SIGNAL(indexClicked()));

    this->setTitle(tr("Actions"));
}

medToolBoxActions::~medToolBoxActions()
{
    delete d;
    d = NULL;
}

void medToolBoxActions::patientSelected()
{
    updateButtons("Patient");
}

void medToolBoxActions::seriesSelected()
{
    updateButtons("Series");
}

void medToolBoxActions::pathSelected(QString path)
{
    QFileInfo fi(path);

    if (fi.isDir())
        updateButtons("Folder");
    else
        updateButtons("File");
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
