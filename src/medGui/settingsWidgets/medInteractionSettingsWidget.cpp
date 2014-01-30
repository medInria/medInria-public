/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medInteractionSettingsWidget.h>

#include <medStorage.h>
#include <medSettingsManager.h>

#include <QtGui>

class medInteractionSettingsWidgetPrivate {
public:
  QComboBox * defaultInteraction;
};

medInteractionSettingsWidget::medInteractionSettingsWidget(QWidget * parent):
    medSettingsWidget(parent),
    d(new medInteractionSettingsWidgetPrivate())
{
    setTabName(tr("Interactions"));

    d->defaultInteraction = new QComboBox(this);
    d->defaultInteraction->addItem(tr("Windowing"),"Windowing");
    d->defaultInteraction->addItem(tr("Zooming"),"Zooming");
    d->defaultInteraction->addItem(tr("Slicing"),"Slicing");
    d->defaultInteraction->addItem(tr("Measuring"),"Measuring");
    d->defaultInteraction->setToolTip(
                tr("Default effect of the left mouse button"));


    QFormLayout* layout = new QFormLayout(this);
    layout->addRow(tr("Default Mouse Interaction"), d->defaultInteraction);

}

medSettingsWidget* createInteractionSettingsWidget(QWidget *parent){
    return new medInteractionSettingsWidget(parent);
}

bool medInteractionSettingsWidget::validate()
{
    return true;
}

void medInteractionSettingsWidget::read()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    QString interaction = mnger->value("interactions","mouse",
                                       "Windowing").toString();
    int index = d->defaultInteraction->findData(interaction);
    //Clamp value to the default one in case the index is not found.
    if ( index == -1) index = 0;
    d->defaultInteraction->setCurrentIndex(index                );

}

bool medInteractionSettingsWidget::write()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    QString interaction = d->defaultInteraction->itemData(
                d->defaultInteraction->currentIndex()).toString();
    mnger->setValue("interactions","mouse",interaction);
    return true;
}

QString medInteractionSettingsWidget::identifier() const {
    return "interactions";
}

QString medInteractionSettingsWidget::description() const {
    return tr("Interactions");
}
