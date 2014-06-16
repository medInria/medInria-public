/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medBoolParameter.h>

#include <QIcon>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>

class medBoolParameterPrivate
{
public:
    QPushButton *pushButton;
    QCheckBox *checkBox;
    QRadioButton *radioButton;
    QIcon icon;
    QSize iconSize;
    QString text;

    ~medBoolParameterPrivate()
    {
        delete pushButton;
        delete checkBox;
        delete radioButton;
    }
};

medBoolParameter::medBoolParameter(QString name, QObject* parent):
    medAbstractBoolParameter(name,parent),
    d(new medBoolParameterPrivate)
{
    m_value = false;
    d->pushButton = NULL;
    d->checkBox = NULL;
    d->radioButton = NULL;
    d->text = QString();
    d->icon = QIcon();
    d->iconSize = QSize(15,15);
}

medBoolParameter::~medBoolParameter()
{
    delete d;
}

void medBoolParameter::updateInternWigets()
{    
    if(d->pushButton)
        d->pushButton->setChecked(m_value);

    if(d->checkBox)
        d->checkBox->setChecked(m_value);

    if(d->radioButton)
        d->radioButton->setChecked(m_value);
}

void medBoolParameter::setIcon(QIcon icon)
{
    d->icon = icon;

    if(d->pushButton)
        d->pushButton->setIcon(d->icon);
}

void medBoolParameter::setIconSize(QSize size)
{
    d->iconSize = size;
    if(d->pushButton)
    {
        d->pushButton->setIconSize(size);
        d->pushButton->setMinimumSize(size);
    }
}

void medBoolParameter::setText(QString value)
{
    d->text = value;

    if(d->pushButton)
        d->pushButton->setText(value);
    if(d->checkBox)
        d->checkBox->setText(value);
    if(d->radioButton)
        d->radioButton->setText(value);
}

QPushButton* medBoolParameter::getPushButton()
{
    if(!d->pushButton)
    {
        d->pushButton = new QPushButton;
        d->pushButton->setCheckable(true);
        d->pushButton->setChecked(m_value);
        d->pushButton->setText(d->text);
        if(!d->icon.isNull())
        {
            d->pushButton->setIcon(d->icon);
            d->pushButton->setIconSize(d->iconSize);
            d->pushButton->setMinimumSize(d->iconSize);
        }

        this->addToInternWidgets(d->pushButton);
        connect(d->pushButton, SIGNAL(destroyed()), this, SLOT(removeInternPushButton()));
        connect(d->pushButton, SIGNAL(toggled(bool)), this, SLOT(setValue(bool)));
    }
    return d->pushButton;
}

QCheckBox* medBoolParameter::getCheckBox()
{
    if(!d->checkBox)
    {
        d->checkBox = new QCheckBox;
        d->checkBox->setChecked(m_value);
        d->checkBox->setText(d->text);
        d->checkBox->setTristate(false);

        this->addToInternWidgets(d->checkBox);
        connect(d->checkBox, SIGNAL(destroyed()), this, SLOT(removeInternCheckBox()));
        connect(d->checkBox, SIGNAL(toggled(bool)), this, SLOT(setValue(bool)));
    }
    return d->checkBox;
}



QRadioButton* medBoolParameter::getRadioButton()
{
    if(!d->radioButton)
    {
        d->radioButton = new QRadioButton;
        d->radioButton->setChecked(m_value);
        d->radioButton->setText(d->text);
        d->radioButton->setAutoExclusive(false);

        this->addToInternWidgets(d->radioButton);
        connect(d->radioButton, SIGNAL(destroyed()), this, SLOT(removeInternRadioButton()));
        connect(d->radioButton, SIGNAL(toggled(bool)), this, SLOT(setValue(bool)));
    }
    return d->radioButton;
}

QWidget* medBoolParameter::getWidget()
{
    return this->getCheckBox();
}

void medBoolParameter::removeInternPushButton()
{
    this->removeFromInternWidgets(d->pushButton);
    d->pushButton = NULL;
}

void medBoolParameter::removeInternCheckBox()
{
    this->removeFromInternWidgets(d->checkBox);
    d->checkBox = NULL;
}

void medBoolParameter::removeInternRadioButton()
{
    this->removeFromInternWidgets(d->radioButton);
    d->radioButton = NULL;
}

