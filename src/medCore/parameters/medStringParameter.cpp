/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medStringParameter.h>

#include <QLineEdit>
#include <QDebug>


class medStringParameterPrivate
{
public:
    QLineEdit* lineEdit;

    ~medStringParameterPrivate(){delete lineEdit;}
};

medStringParameter::medStringParameter(QString name, QObject* parent):
    medAbstractStringParameter(name, parent),
    d(new medStringParameterPrivate)
{
    d->lineEdit = NULL;
}

medStringParameter::~medStringParameter()
{
    delete d;
}

void medStringParameter::clear()
{
    if(d->lineEdit)
        d->lineEdit->clear();
}

QLineEdit* medStringParameter::getLineEdit()
{
    if(!d->lineEdit)
    {
        d->lineEdit = new QLineEdit;

        d->lineEdit->setText(this->value());

        this->addToInternWidgets(d->lineEdit);

        connect(d->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(setValue(QString)));
        connect(d->lineEdit, SIGNAL(destroyed()), this, SLOT(removeInternLineEdit()));
    }
    return d->lineEdit;
}

QWidget* medStringParameter::getWidget()
{
    return this->getLineEdit();
}

void medStringParameter::updateInternWigets()
{
    if (d->lineEdit)
    {
        d->lineEdit->setText(m_value);
    }
}

void medStringParameter::removeInternLineEdit()
{
    this->removeFromInternWidgets(d->lineEdit);
    d->lineEdit = NULL;
}

