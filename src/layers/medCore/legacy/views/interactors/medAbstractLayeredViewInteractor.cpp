/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractLayeredViewInteractor.h>

#include <QAbstractButton>
#include <QtWidgets>
#include <medAbstractLayeredView.h>
#include <medAbstractParameterL.h>
#include <medBoolParameterL.h>
#include <medDataManager.h>

class medAbstractLayeredViewInteractorPrivate
{
public:
    medAbstractLayeredView *view;
    medBoolParameterL *visibilityParameter;
};

medAbstractLayeredViewInteractor::medAbstractLayeredViewInteractor(medAbstractView* parent):
    medAbstractViewInteractor(parent), d(new medAbstractLayeredViewInteractorPrivate)
{
    d->view = dynamic_cast<medAbstractLayeredView *>(parent);
    d->visibilityParameter = nullptr;
}

medAbstractLayeredViewInteractor::~medAbstractLayeredViewInteractor()
{
    delete d;
}

medAbstractBoolParameterL* medAbstractLayeredViewInteractor::visibilityParameter()
{
    bool newVisibilityParameter = !d->visibilityParameter;
    if(newVisibilityParameter)
    {
        d->visibilityParameter = new medBoolParameterL("Visibility", this);
        connect(d->visibilityParameter, SIGNAL(valueChanged(bool)), this, SLOT(setVisibility(bool)));
    }

    if(d->visibilityParameter->getPushButton()->icon().isNull())
    {
        if(this->inputData())
        {
            QPixmap thumbnail = medDataManager::instance().thumbnail(this->inputData()->dataIndex());
            QPushButton* thumbnailButton = d->visibilityParameter->getPushButton();
            QIcon thumbnailIcon;
            // Set the off icon to the greyed out version of the regular icon
            thumbnailIcon.addPixmap(thumbnail, QIcon::Normal, QIcon::On);
            QStyleOption opt(0);
            opt.palette = QApplication::palette();
            QPixmap pix = QApplication::style()->generatedIconPixmap(QIcon::Disabled, thumbnail, &opt);
            thumbnailIcon.addPixmap(pix, QIcon::Normal, QIcon::Off);
            thumbnailButton->setFocusPolicy(Qt::NoFocus);
            thumbnailButton->setIcon(thumbnailIcon);
            thumbnailButton->setIconSize(QSize(22,22));
            thumbnailButton->setCheckable(true);
            if (newVisibilityParameter)
            {
                thumbnailButton->setChecked(true);
            }
            thumbnailButton->setFlat(true);
        }
    }
    return d->visibilityParameter;
}
