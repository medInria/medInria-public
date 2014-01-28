/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medEditableStringParameter.h"

#include <QComboBox>
#include <QDir>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QHBoxLayout>

#include <medSettingsManager.h>

class medEditableStringParameterPrivate
{
public:
    QLineEdit* lineEdit;
    ~medEditableStringParameterPrivate() {delete lineEdit;}
};

medEditableStringParameter::medEditableStringParameter(QString name, QObject* parent):
    medAbstractStringParameter(name, parent),
    d(new medEditableStringParameterPrivate)
{
    d->lineEdit = NULL;
}

medEditableStringParameter::~medEditableStringParameter()
{
    delete d;
}


void medEditableStringParameter::clear()
{
    if(d->lineEdit)
        d->lineEdit->clear();;
}

QWidget* medEditableStringParameter::getWidget()
{
    if(!d->lineEdit)
    {
        d->lineEdit = new QLineEdit;
        d->lineEdit->setText(m_value);

        this->addToInternWidgets(d->lineEdit);
        connect(d->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(setValue(QString)));
        connect(d->lineEdit, SIGNAL(destroyed()), this, SLOT(removeInternWidget()));
    }
    return d->lineEdit;
}

void medEditableStringParameter::updateInternWigets()
{


    if (d->lineEdit)
        d->lineEdit->setText(m_value);
}

void medEditableStringParameter::removeInternWidget()
{
    this->removeFromInternWidgets(d->lineEdit);
    d->lineEdit = NULL;
}
