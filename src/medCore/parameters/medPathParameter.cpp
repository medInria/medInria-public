/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medPathParameter.h>

#include <QPushButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QFileDialog>

class medPathParameterPrivate
{
public:
  QWidget* widget;
  QPushButton *selectPathButton;

};

medPathParameter::medPathParameter(QString name, QObject* parent): medStringParameter(name, parent),
    d(new medPathParameterPrivate)
{
    d->widget = NULL;
    d->selectPathButton = NULL;
}

medPathParameter::~medPathParameter()
{
    delete d;
}

QWidget* medPathParameter::getWidget()
{
    if(!d->widget)
    {
        d->widget = new QWidget;
        d->selectPathButton = new QPushButton("...");

        QHBoxLayout *layout = new QHBoxLayout(d->widget);
        layout->addWidget(this->getLineEdit());
        layout->addWidget(d->selectPathButton);

        this->getLineEdit()->setText(this->value());

        this->addToInternWidgets(d->widget);

        connect(d->widget, SIGNAL(destroyed()), this, SLOT(removeInternWidget()));
        connect(d->selectPathButton, SIGNAL(clicked()), this, SLOT(openFileDialog()));
    }
    return d->widget;
}

void medPathParameter::removeInternWidget()
{
    this->removeFromInternWidgets(d->widget);
    d->widget = NULL;
}

void medPathParameter::openFileDialog()
{
    QString fileName = QFileDialog::getOpenFileName(NULL, tr("Select file"),QDir::homePath());

    if(!fileName.isEmpty())
        this->setValue(fileName);
}
