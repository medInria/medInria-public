/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medBrowserPacsNodesToolBox.h>

#include <QSettings>

class medBrowserPacsNodesToolBoxPrivate
{
public:
    QLineEdit *title;
    QLineEdit *address;
    QLineEdit *port;

    QTableWidget *table;

    QPushButton *add;
    QPushButton *ech;
    QPushButton *rem;

    QString hostTitle;
    QString hostAddress;
    QString hostPort;

};

medBrowserPacsNodesToolBox::medBrowserPacsNodesToolBox(QWidget *parent) : medToolBox(parent), d(new medBrowserPacsNodesToolBoxPrivate)
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
    this->addWidget(page);

    connect(d->add, SIGNAL(clicked()), this, SLOT(addNode()));
    connect(d->ech, SIGNAL(clicked()), this, SIGNAL(echoRequest()));
    connect(d->rem, SIGNAL(clicked()), this, SLOT(remNode()));

    this->readSettings();
}

medBrowserPacsNodesToolBox::~medBrowserPacsNodesToolBox(void)
{
    this->writeSettings();
    delete d;

    d = NULL;
}

void medBrowserPacsNodesToolBox::readSettings(void)
{
    QList<QVariant> nodes;

    QSettings settings;

    settings.beginGroup("medBrowserPacsHostToolBox");
    d->hostTitle = settings.value("title").toString();
    d->hostAddress = settings.value("address").toString();
    d->hostPort = settings.value("port").toString();
    settings.endGroup();

    settings.beginGroup("medBrowserPacsNodesToolBox");
    nodes = settings.value("nodes").toList();
    settings.endGroup();

    fillWidget(nodes);

}

void medBrowserPacsNodesToolBox::writeSettings(void)
{
    QList<QVariant> nodes;

    for(int i = 0; i < d->table->rowCount(); i++) {
        QStringList node;
        node << d->table->item(i, 0)->text();
        node << d->table->item(i, 1)->text();
        node << d->table->item(i, 2)->text();

        nodes << node;
    }

    QSettings settings;
    settings.beginGroup("medBrowserPacsNodesToolBox");
    settings.setValue("nodes", nodes);
    settings.endGroup();
}

void medBrowserPacsNodesToolBox::addNode(void)
{
    int row = d->table->rowCount(); d->table->insertRow(row);

    d->table->setItem(row, 0, new QTableWidgetItem(d->title->text()));
    d->table->setItem(row, 1, new QTableWidgetItem(d->address->text()));
    d->table->setItem(row, 2, new QTableWidgetItem(d->port->text()));

    this->writeSettings();
    emit nodesUpdated();
}

void medBrowserPacsNodesToolBox::remNode(void)
{
    d->table->removeRow(d->table->currentRow());

    this->writeSettings();
    emit nodesUpdated();
}

void medBrowserPacsNodesToolBox::fillWidget( QList<QVariant> nodes )
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

void medBrowserPacsNodesToolBox::onEchoResponse( QVector<bool> vect)
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
