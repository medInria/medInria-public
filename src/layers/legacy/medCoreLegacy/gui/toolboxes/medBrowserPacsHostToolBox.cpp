/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medBrowserPacsHostToolBox.h>

#include <QtGui>

#include <iostream>

class medBrowserPacsHostToolBoxPrivate
{
public:
    QLineEdit *title;
    QLineEdit *port;
    QPushButton *apply;
};

medBrowserPacsHostToolBox::medBrowserPacsHostToolBox(QWidget *parent) : medToolBox(parent), d(new medBrowserPacsHostToolBoxPrivate)
{
    QWidget *page = new QWidget(this);

    QValidator* validator = new QIntValidator( 0, 65535, this );
    d->title = new QLineEdit(page);
    d->port = new QLineEdit(page);
    d->port->setValidator(validator);
    d->apply = new QPushButton("Apply", page);

    QFormLayout *layout = new QFormLayout(page);
    layout->addRow("Title", d->title);
    layout->addRow("Port", d->port);
    layout->addRow(d->apply);

    this->setTitle("DICOM Server");
    this->addWidget(page);

    connect(d->apply, SIGNAL(clicked()), this, SLOT(onSettingsApplied()));

    this->readSettings();
}

medBrowserPacsHostToolBox::~medBrowserPacsHostToolBox(void)
{
    this->writeSettings();

    delete d;

    d = NULL;
}

void medBrowserPacsHostToolBox::readSettings(void)
{
    QSettings settings;
    settings.beginGroup("medBrowserPacsHostToolBox");
    QString title = settings.value("title").toString();
    QString port = settings.value("port").toString();
    settings.endGroup();

    if (title.isEmpty())
        d->title->setText("MEDINRIA");
    else
        d->title->setText(title);
    if (port.isEmpty())
        d->port->setText("9999");
    else
        d->port->setText(port);

}

void medBrowserPacsHostToolBox::writeSettings(void)
{
    QSettings settings;
    settings.beginGroup("medBrowserPacsHostToolBox");
    settings.setValue("title", d->title->text());
    settings.setValue("port", d->port->text());
    settings.endGroup();
}

QString medBrowserPacsHostToolBox::title(void)
{
    return d->title->text();
}

QString medBrowserPacsHostToolBox::port(void)
{
    return d->port->text();
}

void medBrowserPacsHostToolBox::onSettingsApplied(void)
{
    this->writeSettings();
}
