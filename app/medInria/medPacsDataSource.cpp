#include "medPacsDataSource.h"

#include <medJobManager.h>

#include <medBrowserPacsHostToolBox.h>
#include <medBrowserPacsNodesToolBox.h>
#include <medBrowserPacsSearchToolBox.h>
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
    medBrowserPacsHostToolBox *pacsHostToolBox;
    medBrowserPacsNodesToolBox *pacsNodesToolBox;
    medBrowserPacsSearchToolBox *pacsSearchToolBox;

    QList<medToolBox*> toolboxes;
};

medPacsDataSource::medPacsDataSource(QWidget* parent) : medAbstractDataSource(parent), d(new medPacsDataSourcePrivate)
{
    //this hierarchy is clearly broken: medPacsDataSource inherits from QObject,
    //pacsWidget is a widget, and its parent is the browserArea,
    //see destructor.
    d->pacsWidget = new medPacsWidget(parent);
    d->pacs_selector = new medPacsSelector(d->pacsWidget);
    d->pacsHostToolBox = new medBrowserPacsHostToolBox(d->pacsWidget);
    d->toolboxes.push_back(d->pacsHostToolBox);
    d->pacsNodesToolBox = new medBrowserPacsNodesToolBox(d->pacsWidget);
    d->toolboxes.push_back(d->pacsNodesToolBox);
    d->pacsSearchToolBox = new medBrowserPacsSearchToolBox(d->pacsWidget);
    d->toolboxes.push_back(d->pacsSearchToolBox);

    connect(d->pacsWidget, SIGNAL(moveList(const QVector<medMoveCommandItem>&)), this, SLOT(onPacsMove(const QVector<medMoveCommandItem>&)));
    connect(d->pacsWidget, SIGNAL(import(QString)), this, SIGNAL(dataToImportReceived(QString)));

    connect(d->pacsNodesToolBox, SIGNAL(nodesUpdated()), d->pacs_selector, SLOT(updateList()));
    connect(d->pacs_selector, SIGNAL(selectionChanged(QVector<int>)), d->pacsWidget, SLOT(updateSelectedNodes(QVector<int>)));

    connect(d->pacsNodesToolBox, SIGNAL(echoRequest()), d->pacsWidget, SLOT(onEchoRequest()));
    connect(d->pacsWidget, SIGNAL(echoResponse(QVector<bool>)), d->pacsNodesToolBox, SLOT(onEchoResponse(QVector<bool>)));

    connect(d->pacsSearchToolBox, SIGNAL(search(QString)), d->pacsWidget, SLOT(search(QString)));

}

medPacsDataSource::~medPacsDataSource()
{
    //I don't know what happens if the browser destroys pacsWidget before
    //destroying medPacsDataSource...
    //we can delete the pacsdatasource without destroying the parent.
    d->pacsWidget->deleteLater();
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

QString medPacsDataSource::description(void) const
{
	return tr("Browse PACS server");
}

void medPacsDataSource::onPacsMove( const QVector<medMoveCommandItem>& cmdList)
{
    medPacsMover* mover = new medPacsMover(cmdList);
    connect(mover, SIGNAL(import(QString)), this, SIGNAL(dataReceived(QString)));
    medJobManager::instance()->registerJobItem(mover, tr("Moving"));
    QThreadPool::globalInstance()->start(mover);
}
