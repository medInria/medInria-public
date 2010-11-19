/* medToolBoxPacsNodes.cpp --- 
* 
* Author: Julien Wintz
* Copyright (C) 2008 - Julien Wintz, Inria.
* Created: Tue Oct  5 15:49:05 2010 (+0200)
* Version: $Id$
* Last-Updated: Wed Oct  6 18:49:00 2010 (+0200)
*           By: Julien Wintz
*     Update #: 151
*/

/* Commentary: 
* 
*/

/* Change log:
* 
*/

#include "medToolBoxPacsNodes.h"


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

    QValidator* validator = new QIntValidator( 0, 65535, this );

    d->title = new QLineEdit("DCM4CHEE", page);
    d->address = new QLineEdit("jumbo-4.irisa.fr", page);
    d->port = new QLineEdit("10012", page);
    d->port->setValidator(validator);

    d->table = new QTableWidget(page);

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

    this->setTitle("Remote Clients");
    this->setWidget(page);

    connect(d->add, SIGNAL(clicked()), this, SLOT(addNode()));
    connect(d->ech, SIGNAL(clicked()), this, SIGNAL(echoRequest()));
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

    fillWidget(nodes);

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

void medToolBoxPacsNodes::addNode(void)
{
    int row = d->table->rowCount(); d->table->insertRow(row);

    d->table->setItem(row, 0, new QTableWidgetItem(d->title->text()));
    d->table->setItem(row, 1, new QTableWidgetItem(d->address->text()));
    d->table->setItem(row, 2, new QTableWidgetItem(d->port->text()));

    this->writeSettings();
    emit nodesUpdated();
}

void medToolBoxPacsNodes::remNode(void)
{
    d->table->removeRow(d->table->currentRow());

    this->writeSettings();
    emit nodesUpdated();
}

void medToolBoxPacsNodes::fillWidget( QList<QVariant> nodes )
{
    d->table->clear(); 
    d->table->setRowCount(0);
    d->table->setColumnCount(3);
    d->table->setHorizontalHeaderLabels(QStringList() << "Title" << "Address" << "Port");
    d->table->setSelectionBehavior(QAbstractItemView::SelectRows);

    foreach(QVariant node, nodes) {
        int row = d->table->rowCount(); d->table->insertRow(row);

        d->table->setItem(row, 0, new QTableWidgetItem(node.toStringList().at(0)));
        d->table->setItem(row, 1, new QTableWidgetItem(node.toStringList().at(1)));
        d->table->setItem(row, 2, new QTableWidgetItem(node.toStringList().at(2)));
    }
}

void medToolBoxPacsNodes::onEchoResponse( QVector<bool> vect)
{
    for(int i=0;i< vect.size(); i++)
    {
        if (d->table->rowCount() > i)
        {
            if(vect.at(i))
            {
               d->table->item(i, 0)->setBackground(Qt::green);
               d->table->item(i, 1)->setBackground(Qt::green);
               d->table->item(i, 2)->setBackground(Qt::green);
            }
            else 
            {
               d->table->item(i, 0)->setBackground(Qt::red);
               d->table->item(i, 1)->setBackground(Qt::red);
               d->table->item(i, 2)->setBackground(Qt::red);
            }
        }
    }
}
