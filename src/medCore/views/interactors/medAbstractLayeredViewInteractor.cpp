/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractLayeredViewInteractor.h>

#include <QAbstractButton>

#include <medAbstractLayeredView.h>
#include <medAbstractParameter.h>
#include <medBoolParameter.h>
#include <medMetaDataKeys.h>

class medAbstractLayeredViewInteractorPrivate
{
public:
    medAbstractLayeredView *view;
    medBoolParameter *visibilityParameter;
};

medAbstractLayeredViewInteractor::medAbstractLayeredViewInteractor(medAbstractView* parent):
    medAbstractViewInteractor(parent), d(new medAbstractLayeredViewInteractorPrivate)
{
    d->view = dynamic_cast<medAbstractLayeredView *>(parent);
    d->visibilityParameter = NULL;
}

medAbstractLayeredViewInteractor::~medAbstractLayeredViewInteractor()
{
    delete d;
}

medAbstractBoolParameter* medAbstractLayeredViewInteractor::visibilityParameter()
{
    if(!d->visibilityParameter)
    {
        d->visibilityParameter = new medBoolParameter("Visibility", this);
        connect(d->visibilityParameter, SIGNAL(valueChanged(bool)), this, SLOT(setVisibility(bool)));
    }
    if(d->visibilityParameter->getPushButton()->icon().isNull())
    {
        if(this->inputData())
        {
            QString thumbPath = medMetaDataKeys::SeriesThumbnail.getFirstValue(this->inputData(),":icons/layer.png");
            QPushButton* thumbnailButton = d->visibilityParameter->getPushButton();
            QIcon thumbnailIcon;
            // Set the off icon to the greyed out version of the regular icon
            thumbnailIcon.addPixmap(QPixmap(thumbPath), QIcon::Normal, QIcon::On);
            QStyleOption opt(0);
            opt.palette = QApplication::palette();
            QPixmap pix = QApplication::style()->generatedIconPixmap(QIcon::Disabled, QPixmap(thumbPath), &opt);
            thumbnailIcon.addPixmap(pix, QIcon::Normal, QIcon::Off);
            thumbnailButton->setFocusPolicy(Qt::NoFocus);
            thumbnailButton->setIcon(thumbnailIcon);
            thumbnailButton->setIconSize(QSize(22,22));
            thumbnailButton->setCheckable(true);
            thumbnailButton->setChecked(true);
            thumbnailButton->setFlat(true);
        }
    }
    return d->visibilityParameter;
}
