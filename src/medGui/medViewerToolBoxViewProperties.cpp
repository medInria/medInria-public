/*
 * medViewerToolBoxViewProperties.cpp
 *
 *  Created on: 15 févr. 2011
 *      Author: aabadie
 */

#include <medViewerToolBoxViewProperties.h>

#include <medCore/medAbstractView.h>
#include <medGui/medToolBoxTab.h>

class medViewerToolBoxViewPropertiesPrivate
{
public:
        medAbstractView *view;
        QComboBox * dataComboBox;
        QCheckBox * visible;
};

medViewerToolBoxViewProperties::medViewerToolBoxViewProperties(QWidget *parent) : medToolBox(parent), d(new medViewerToolBoxViewPropertiesPrivate)
{
    d->view = 0;
    d->dataComboBox = new QComboBox(this);
    d->dataComboBox->setFocusPolicy(Qt::NoFocus);

    d->visible = new QCheckBox(this);
//    d->visible->setChecked(true);

    QWidget *propertiesToolBoxWidget = new QWidget;

    QFormLayout * viewPropertiesLayout = new QFormLayout(propertiesToolBoxWidget);
    viewPropertiesLayout->addRow("Select data :", d->dataComboBox);
    viewPropertiesLayout->addRow("Visible :", d->visible);
    viewPropertiesLayout->setFormAlignment(Qt::AlignHCenter);


    this->setTitle("View properties");
    this->addWidget(propertiesToolBoxWidget);

    QObject::connect(d->dataComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onDataSelected(int)));
    QObject::connect(d->visible, SIGNAL(stateChanged(int)), this, SLOT(onVisibilitySet(int)));
}

medViewerToolBoxViewProperties::~medViewerToolBoxViewProperties(void)
{
    delete d;
    d = NULL;
}

void medViewerToolBoxViewProperties::update(dtkAbstractView *view)
{
    d->view = dynamic_cast<medAbstractView *> (view);

    if (!view)
        return;

    QObject::connect(d->view, SIGNAL(dataAdded(int)), this, SLOT(onDataAdded(int)),Qt::UniqueConnection);
    QObject::connect(d->view, SIGNAL(closed()), this, SLOT(onViewClosed()),Qt::UniqueConnection);
}

void medViewerToolBoxViewProperties::onDataAdded(int layer)
{
    if (!d->view)
        return;

    d->view->setVisibility(1, layer);
    d->visible->setChecked(true);
    d->dataComboBox->insertItem(layer, QString::number(layer));
    d->dataComboBox->setCurrentIndex(layer);

}

void medViewerToolBoxViewProperties::onViewClosed(void)
{
    qDebug() << "Clear combo box";
    d->dataComboBox->clear();
}

void medViewerToolBoxViewProperties::onDataSelected(int index)
{
    if (!d->view || (index == -1))
        return;
    d->view->setVisibility(d->view->visibility(index), index);
    d->visible->setChecked((d->view->visibility(index) == 1));
}

void medViewerToolBoxViewProperties::onVisibilitySet(int state)
{
    if ( (!d->view) || (d->dataComboBox->currentIndex() == -1))
        return;

    if(state == Qt::Checked)
        d->view->setVisibility(1, d->dataComboBox->currentIndex());
    else
        d->view->setVisibility(0, d->dataComboBox->currentIndex());
}

