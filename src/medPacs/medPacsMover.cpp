#include "medPacsMover.h"

#include "medAbstractPacsMoveScu.h"
#include "medAbstractPacsFactory.h"

class medPacsMoverPrivate
{
public:

    QString host_title;
    QString host_address;
    QString host_port;

    int nodeIndex;
    int group;
    int elem;
    QString query; 
    QString storageFolder;

    QList<QStringList> nodes;

    medAbstractPacsMoveScu* move;
};

medPacsMover::medPacsMover(int group, int elem, QString query, 
                           QString storageFolder, int nodeIndex ): QRunnable(), 
                           d(new medPacsMoverPrivate)
{
    d->group = group;
    d->elem = elem;
    d->query = query;
    d->storageFolder = storageFolder;
    d->nodeIndex = nodeIndex;

    d->move = medAbstractPacsFactory::instance()->createMoveScu("dcmtkMoveScu");

    connect(d->move,SIGNAL(progressed(int)),this, SIGNAL(progressed(int)));
}

medPacsMover::~medPacsMover( void )
{
    delete d;

    d = NULL;
}

void medPacsMover::readSettings(void)
{
    QSettings settings("inria", "medinria");
    settings.beginGroup("medToolBoxPacsHost");
    d->host_title = settings.value("title").toString();
    d->host_address = settings.value("address").toString();
    d->host_port = settings.value("port").toString();
    settings.endGroup();

    QList<QVariant> nodes;

    settings.beginGroup("medToolBoxPacsNodes");
    nodes = settings.value("nodes").toList();
    settings.endGroup();

    d->nodes.clear();

    foreach(QVariant node, nodes)
        d->nodes << node.toStringList();
}

void medPacsMover::run( void )
{
    readSettings();
    doMove();
}

void medPacsMover::doMove()
{
    d->move->clearAllQueryAttributes();
    d->move->addQueryAttribute(d->group, d->elem, d->query.toLatin1());
    d->move->useBuildInStoreSCP(true);
    d->move->setStorageDirectory(d->storageFolder.toLatin1());
    if (d->move->sendMoveRequest(
        d->nodes.at(d->nodeIndex).at(0).toLatin1(),
        d->nodes.at(d->nodeIndex).at(1).toLatin1(),
        d->nodes.at(d->nodeIndex).at(2).toInt(),
        d->host_title.toLatin1(),
        d->host_address.toLatin1(),
        d->host_port.toInt()))
    {
      QString errorMessage;
      errorMessage = tr("Failed to move from ") +
          d->nodes.at(d->nodeIndex).at(0);
      emit showError(this, errorMessage,5000);
      emit failure();
      return;
    }
    emit progressed(100);
    emit success();
    emit import(d->storageFolder.toLatin1());
}
