/* medToolBoxPacsHost.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct  5 15:49:05 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct  6 11:37:23 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 55
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medToolBoxPacsHost.h"

#include <QtGui>

class medToolBoxPacsHostPrivate
{
public:
    QLineEdit *title;
    QLineEdit *address;
    QLineEdit *port;
    QPushButton *apply;
};

medToolBoxPacsHost::medToolBoxPacsHost(QWidget *parent) : medToolBox(parent), d(new medToolBoxPacsHostPrivate)
{
    QWidget *page = new QWidget(this);

    d->title = new QLineEdit(page);
    d->address = new QLineEdit(page);
    d->port = new QLineEdit(page);
    d->apply = new QPushButton("Apply", page);

    QFormLayout *layout = new QFormLayout(page);
    layout->addRow("Title", d->title);
    layout->addRow("Address", d->address);
    layout->addRow("Port", d->port);
    layout->addRow(d->apply);

    this->setTitle("DICOM Server");
    this->setWidget(page);

    connect(d->apply, SIGNAL(clicked()), this, SLOT(onSettingsApplied()));

    this->readSettings();
}

medToolBoxPacsHost::~medToolBoxPacsHost(void)
{
    this->writeSettings();

    delete d;

    d = NULL;
}

void medToolBoxPacsHost::readSettings(void)
{
    QSettings settings("inria", "medinria");
    settings.beginGroup("medToolBoxPacsHost");
    d->title->setText(settings.value("title").toString());
    d->address->setText(settings.value("address").toString());
    d->port->setText(settings.value("port").toString());
    settings.endGroup();
}

void medToolBoxPacsHost::writeSettings(void)
{
    QSettings settings("inria", "medinria");
    settings.beginGroup("medToolBoxPacsHost");
    settings.setValue("title", d->title->text());
    settings.setValue("address", d->address->text());
    settings.setValue("port", d->port->text());
    settings.endGroup();
}

QString medToolBoxPacsHost::title(void)
{
    return d->title->text();
}

QString medToolBoxPacsHost::address(void)
{
    return d->address->text();
}

QString medToolBoxPacsHost::port(void)
{
    return d->port->text();
}

void medToolBoxPacsHost::onSettingsApplied(void)
{
    this->writeSettings();
}
