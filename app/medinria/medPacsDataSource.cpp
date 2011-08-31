#include "medPacsDataSource.h"

#include <medJobManager.h>

#include <medBrowserToolBoxPacsHost.h>
#include <medBrowserToolBoxPacsNodes.h>
#include <medBrowserToolBoxPacsSearch.h>
#include <medPacsSelector.h>
#include <medToolBox.h>

#include <medPacsWidget.h>
#include <medPacsMover.h>

class medPacsDataSourcePrivate
{
public:

    // mainViewWidget
    medPacsWidget* pacsWidget;

    // selector
    medPacsSelector* pacs_selector;

    // specific toolboxes
    medBrowserToolBoxPacsHost *toolbox_pacs_host;
    medBrowserToolBoxPacsNodes *toolbox_pacs_nodes;
    medBrowserToolBoxPacsSearch *toolbox_pacs_search;

    QList<medToolBox*> toolboxes;
};

medPacsDataSource::medPacsDataSource(QObject* parent) : medAbstractDataSource(parent), d(new medPacsDataSourcePrivate)
{
    d->pacsWidget = new medPacsWidget;
    d->pacs_selector = new medPacsSelector();
    d->toolbox_pacs_host = new medBrowserToolBoxPacsHost();
    d->toolboxes.push_back(d->toolbox_pacs_host);
    d->toolbox_pacs_nodes = new medBrowserToolBoxPacsNodes();
    d->toolboxes.push_back(d->toolbox_pacs_nodes);
    d->toolbox_pacs_search = new medBrowserToolBoxPacsSearch();
    d->toolboxes.push_back(d->toolbox_pacs_search);

    connect(d->pacsWidget, SIGNAL(moveList(const QVector<medMoveCommandItem>&)), this, SLOT(onPacsMove(const QVector<medMoveCommandItem>&)));
    connect(d->pacsWidget, SIGNAL(import(QString)), this, SIGNAL(dataToImportReceived(QString)));

    connect(d->toolbox_pacs_nodes, SIGNAL(nodesUpdated()), d->pacs_selector, SLOT(updateList()));
    connect(d->pacs_selector, SIGNAL(selectionChanged(QVector<int>)), d->pacsWidget, SLOT(updateSelectedNodes(QVector<int>)));

    connect(d->toolbox_pacs_nodes, SIGNAL(echoRequest()), d->pacsWidget, SLOT(onEchoRequest()));
    connect(d->pacsWidget, SIGNAL(echoResponse(QVector<bool>)), d->toolbox_pacs_nodes, SLOT(onEchoResponse(QVector<bool>)));

    connect(d->toolbox_pacs_search, SIGNAL(search(QString)), d->pacsWidget, SLOT(search(QString)));

}

medPacsDataSource::~medPacsDataSource()
{
    delete d;
    d = NULL;
}

QWidget* medPacsDataSource::mainViewWidget()
{
    return d->pacsWidget;
}

QWidget* medPacsDataSource::sourceSelectorWidget()
{
    return d->pacs_selector;
}

QString medPacsDataSource::tabName()
{
    return tr("Pacs");
}

QList<medToolBox*> medPacsDataSource::getToolboxes()
{
    return d->toolboxes;
}

void medPacsDataSource::onPacsMove( const QVector<medMoveCommandItem>& cmdList)
{
    medPacsMover* mover = new medPacsMover(cmdList);
    connect(mover, SIGNAL(import(QString)), this, SIGNAL(dataReceived(QString)));
    medJobManager::instance()->registerJobItem(mover, tr("Moving"));
    QThreadPool::globalInstance()->start(mover);
}
