/* medToolBoxPacsNodes.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct  5 15:49:05 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Oct  5 17:43:01 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 142
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medToolBoxPacsNodes.h"

#include <medPacs/medAbstractPacsFactory.h>
#include <medPacs/medAbstractPacsEchoScu.h>

class medToolBoxPacsNodesPrivate
{
public:
    QLineEdit *title;
    QLineEdit *address;
    QLineEdit *port;

    QTableWidget *table;

    QPushButton *add;
    QPushButton *ech;
    QPushButton *rem;

    QString host_title;
    QString host_address;
    QString host_port;
};

medToolBoxPacsNodes::medToolBoxPacsNodes(QWidget *parent) : medToolBox(parent), d(new medToolBoxPacsNodesPrivate)
{
    QWidget *page = new QWidget(this);

    d->title = new QLineEdit("DCM4CHEE", page);
    d->address = new QLineEdit("jumbo-4.irisa.fr", page);
    d->port = new QLineEdit("10012", page);

    d->table = new QTableWidget(page);
    d->table->setRowCount(0);
    d->table->setColumnCount(3);
    d->table->setHorizontalHeaderLabels(QStringList() << "Title" << "Address" << "Port");

    d->add = new QPushButton("Add"); d->add->setObjectName("left");
    d->ech = new QPushButton("Echo"); d->ech->setObjectName("center");
    d->rem = new QPushButton("Remove"); d->rem->setObjectName("right");

    QHBoxLayout *buttons_layout = new QHBoxLayout;
    buttons_layout->setSpacing(0);
    buttons_layout->addWidget(d->add);
    buttons_layout->addWidget(d->ech);
    buttons_layout->addWidget(d->rem);

    QFormLayout *form = new QFormLayout;
    form->addRow("Title", d->title);
    form->addRow("Address", d->address);
    form->addRow("Port", d->port);

    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->addLayout(form);
    layout->addWidget(d->table);
    layout->addLayout(buttons_layout);

    this->setTitle("Pacs nodes");
    this->setWidget(page);

    connect(d->add, SIGNAL(clicked()), this, SLOT(addNode()));
    connect(d->ech, SIGNAL(clicked()), this, SLOT(echo()));
    connect(d->rem, SIGNAL(clicked()), this, SLOT(remNode()));

    this->readSettings();
}

medToolBoxPacsNodes::~medToolBoxPacsNodes(void)
{
    this->writeSettings();

    delete d;

    d = NULL;
}

void medToolBoxPacsNodes::readSettings(void)
{
    QList<QVariant> nodes;

    QSettings settings("inria", "medinria");

    settings.beginGroup("medToolBoxPacsHost");
    d->host_title = settings.value("title").toString();
    d->host_address = settings.value("address").toString();
    d->host_port = settings.value("port").toString();
    settings.endGroup();

    settings.beginGroup("medToolBoxPacsNodes");
    nodes = settings.value("nodes").toList();
    settings.endGroup();

    foreach(QVariant node, nodes) {
        int row = d->table->rowCount(); d->table->insertRow(row);
        
        d->table->setItem(row, 0, new QTableWidgetItem(node.toStringList().at(0)));
        d->table->setItem(row, 1, new QTableWidgetItem(node.toStringList().at(1)));
        d->table->setItem(row, 2, new QTableWidgetItem(node.toStringList().at(2)));
    }
}

void medToolBoxPacsNodes::writeSettings(void)
{
    QList<QVariant> nodes;

    for(int i = 0; i < d->table->rowCount(); i++) {
        QStringList node;
        node << d->table->item(i, 0)->text();
        node << d->table->item(i, 1)->text();
        node << d->table->item(i, 2)->text();

        nodes << node;
    }

    QSettings settings("inria", "medinria");
    settings.beginGroup("medToolBoxPacsNodes");
    settings.setValue("nodes", nodes);
    settings.endGroup();
}

void medToolBoxPacsNodes::setHostTitle(const QString& title)
{
    d->host_title = title;
}

void medToolBoxPacsNodes::setHostAddress(const QString& address)
{
    d->host_address = address;
}

void medToolBoxPacsNodes::setHostPort(const QString& port)
{
    d->host_port = port;
}

void medToolBoxPacsNodes::addNode(void)
{
    int row = d->table->rowCount(); d->table->insertRow(row);

    d->table->setItem(row, 0, new QTableWidgetItem(d->title->text()));
    d->table->setItem(row, 1, new QTableWidgetItem(d->address->text()));
    d->table->setItem(row, 2, new QTableWidgetItem(d->port->text()));
}

void medToolBoxPacsNodes::remNode(void)
{
    d->table->removeRow(d->table->currentRow());
}

void medToolBoxPacsNodes::echo(void)
{
    for(int i = 0; i < d->table->rowCount(); i++) {

        medAbstractPacsEchoScu *scu = medAbstractPacsFactory::instance()->createEchoScu("dcmtkEchoScu");
        
        if(!scu->sendEchoRequest(
               d->table->item(i, 0)->text().toAscii().constData(),
               d->table->item(i, 1)->text().toAscii().constData(),
               d->table->item(i, 2)->text().toInt(),
               d->host_title.toAscii().constData(),
               d->host_address.toAscii().constData(),
               d->host_port.toInt())) {

            d->table->item(i, 0)->setBackground(Qt::green);
            d->table->item(i, 1)->setBackground(Qt::green);
            d->table->item(i, 2)->setBackground(Qt::green);
        } else {
            d->table->item(i, 0)->setBackground(Qt::red);
            d->table->item(i, 1)->setBackground(Qt::red);
            d->table->item(i, 2)->setBackground(Qt::red);
        }
    }
}
