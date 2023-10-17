/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medEmptyDbWarning.h>

#include<medSettingsManager.h>

#include <QtGui>
#include <QtWidgets>

class medEmptyDbWarningPrivate
{
public:
    QCheckBox* showAgainBox;
};

medEmptyDbWarning::medEmptyDbWarning(QWidget *parent) :
    QDialog(parent), d(new medEmptyDbWarningPrivate)
{
    QLabel * label = new QLabel (tr("The database is empty. Switch to the Browser workspace to import data."),
                                 this);
    QPushButton * okButton = new QPushButton(tr("Ok"),this);
    d->showAgainBox = new QCheckBox(tr("Keep showing this message."),
                                             this);
    d->showAgainBox->setChecked(true);

    QVBoxLayout * vLayout = new QVBoxLayout(this);

    vLayout->addWidget(label);

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget(d->showAgainBox);
    hLayout->addStretch();
    hLayout->addWidget(okButton);

    vLayout->addLayout(hLayout);
    connect (okButton,SIGNAL(clicked()),this,SLOT(accept()));
    this->setModal(true);
}

medEmptyDbWarning::~medEmptyDbWarning()
{
    delete d;
    d = nullptr;
}

void medEmptyDbWarning::accept()
{
    medSettingsManager::instance().setValue("system","showEmptyDbWarning",d->showAgainBox->isChecked());
    QDialog::accept();
}

void medEmptyDbWarning::reject()
{
    medSettingsManager::instance().setValue("system","showEmptyDbWarning",d->showAgainBox->isChecked());
    QDialog::reject();
}
