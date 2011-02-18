/*
 * medViewerToolBoxViewProperties.cpp
 *
 *  Created on: 15 févr. 2011
 *      Author: aabadie
 */

#include <medViewerToolBoxViewProperties.h>

#include <dtkCore/dtkAbstractData.h>
#include <medCore/medAbstractView.h>
#include <medGui/medToolBoxTab.h>

class medViewerToolBoxViewPropertiesPrivate
{
    public:
        medAbstractView *view;
        QWidget* propertiesToolBoxWidget;
        QComboBox * dataComboBox;
        QComboBox * lutComboBox;
        QCheckBox * visible;
        QDoubleSpinBox * opacity;
        QSlider * opacitySlider;
        QTreeWidget * propertiesTree;
};

medViewerToolBoxViewProperties::medViewerToolBoxViewProperties(QWidget *parent) :
    medToolBox(parent), d(new medViewerToolBoxViewPropertiesPrivate)
{
    d->view = 0;

    d->propertiesTree = new QTreeWidget(this);
//    QString style = QString("border-image: '';")
//                  + QString("border-top-width: 0px;")
//                  + QString("border-left-width: 0px;")
//                  + QString("border-right-width: 0px;")
//                  + QString("border-bottom-width: 0px;")
//                  + QString("padding-top: -7px;")
//                  + QString("padding-left: -4px;")
//                  + QString("padding-right: -5px;")
//                  + QString("color: #b2b8b2;")
//                  + QString("background: #313131;");
//
//    d->propertiesTree->setStyleSheet(style);
    d->propertiesTree->setColumnCount(3);
    d->propertiesTree->setColumnWidth(0,50);
    QStringList headers;
    headers << "Layer" << "Name" << "Value";
    d->propertiesTree->setHeaderLabels(headers);
//    d->propertiesTree->setAnimated(true);
    d->propertiesTree->setAlternatingRowColors(true);

    d->dataComboBox = new QComboBox(this);
    d->dataComboBox->setFocusPolicy(Qt::NoFocus);

    d->lutComboBox = new QComboBox(this);
    d->lutComboBox->setFocusPolicy(Qt::NoFocus);
    d->lutComboBox->addItem("Default");
    d->lutComboBox->addItem("Black & White");
    d->lutComboBox->addItem("Black & White Inversed");
    d->lutComboBox->addItem("Spectrum");
    d->lutComboBox->addItem("Hot Metal");
    d->lutComboBox->addItem("Hot Green");
    d->lutComboBox->addItem("Hot Iron");
    d->lutComboBox->addItem("GE");
    d->lutComboBox->addItem("Flow");
    d->lutComboBox->addItem("Loni");
    d->lutComboBox->addItem("Loni 2");
    d->lutComboBox->addItem("Asymmetry");
    d->lutComboBox->addItem("P-Value");
    d->lutComboBox->addItem("Red Black Alpha");
    d->lutComboBox->addItem("Green Black Alpha");
    d->lutComboBox->addItem("Blue Black Alpha");
    d->lutComboBox->addItem("Muscles & Bones");
    d->lutComboBox->addItem("Bones");
    d->lutComboBox->addItem("Red Vessels");
    d->lutComboBox->addItem("Cardiac");
    d->lutComboBox->addItem("Gray Rainbow");
    d->lutComboBox->addItem("Stern");
    d->lutComboBox->addItem("Black Body");

    d->visible = new QCheckBox(this);

    d->opacitySlider = new QSlider(this);
    d->opacitySlider->setRange(0, 100);
    d->opacitySlider->setValue(100);
    d->opacitySlider->setOrientation(Qt::Horizontal);

    d->opacity = new QDoubleSpinBox(this);
    d->opacity->setRange(0.0, 1.0);
    d->opacity->setSingleStep(0.01);
    d->opacity->setValue(1.0);

    d->propertiesToolBoxWidget = new QWidget;
    d->propertiesToolBoxWidget->setEnabled(false);

    QHBoxLayout * opacityLayout = new QHBoxLayout;
    opacityLayout->addWidget(d->opacitySlider);
    opacityLayout->addWidget(d->opacity);

    QFormLayout * viewPropertiesLayout = new QFormLayout(d->propertiesToolBoxWidget);
    viewPropertiesLayout->addRow("Select data :", d->dataComboBox);
    viewPropertiesLayout->addRow("Visible :", d->visible);
    //    viewPropertiesLayout->addRow("Opacity :", d->opacity);
    viewPropertiesLayout->addRow("Opacity :", opacityLayout);
    viewPropertiesLayout->addRow("LUT :", d->lutComboBox);
    viewPropertiesLayout->setFormAlignment(Qt::AlignHCenter);

    this->setTitle("View Properties");
    this->addWidget(d->propertiesToolBoxWidget);
    this->addWidget(d->propertiesTree);

    QObject::connect(d->dataComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onDataSelected(int)));
    QObject::connect(d->visible, SIGNAL(stateChanged(int)), this, SLOT(onVisibilitySet(int)));
    QObject::connect(d->opacity, SIGNAL(valueChanged(double)), this, SLOT(onOpacitySet(double)));
    QObject::connect(d->opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(onOpacitySliderSet(int)));
    QObject::connect(d->lutComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onLUTChanged(int)));

    this->hide();
}

medViewerToolBoxViewProperties::~medViewerToolBoxViewProperties(void)
{
    delete d;
    d = NULL;
}

void
medViewerToolBoxViewProperties::update(dtkAbstractView *view)
{
    //    d->propertiesTree->clear();

    if (d->view) {
        QObject::disconnect(d->view, SIGNAL(dataAdded(int)), this, SLOT(onDataAdded(int)));
        QObject::disconnect(d->view, SIGNAL(closing()), this, SLOT(onViewClosed()));

        this->onViewClosed();
    }

    if (medAbstractView *medView = dynamic_cast<medAbstractView *> (view)) {
        d->view = medView;

        this->show();

        //        dtkAbstractData * data =  0;
        //        QString seriesName;

        int count = d->view->layerCount();
        int current = d->view->currentLayer();

        d->dataComboBox->blockSignals(true);
        for (int i = 0; i < count; i++) {
            //            d->propertiesTree->ad
            //            data = d->view->dataInList(i);
            //            seriesName = data->metaDataValues(tr("SeriesDescription"))[0];
            //            seriesName = data->metaDataValues(tr("SeriesDescription"))[0];
            d->dataComboBox->insertItem(i, QString::number(i));
        }
        d->dataComboBox->setCurrentIndex(current);
        d->dataComboBox->blockSignals(false);
        d->propertiesToolBoxWidget->setEnabled(true);
        d->visible->blockSignals(true);
        d->visible->setChecked((d->view->visibility(current) == 1));
        d->visible->blockSignals(false);
        d->opacity->blockSignals(true);
        d->opacity->setValue(d->view->opacity(current));
        d->opacity->blockSignals(false);
        d->opacitySlider->blockSignals(true);
        d->opacitySlider->setValue((int) (d->view->opacity(current) * 100));
        d->opacitySlider->blockSignals(false);

        QObject::connect(d->view, SIGNAL(dataAdded(int)), this, SLOT(onDataAdded(int)), Qt::UniqueConnection);
        QObject::connect(d->view, SIGNAL(closing()), this, SLOT(onViewClosed()), Qt::UniqueConnection);
    }
}

void
medViewerToolBoxViewProperties::onDataAdded(int layer)
{
    if (!d->view)
        return;

    //data->property("SeriesName");
    //
    //    qDebug() << "Data added";
    //
    //    dtkAbstractData * data = d->view->dataInList(layer);
    //    const QString seriesName = data->metaDataValues(tr("SeriesDescription"))[0];
    //    const QString seriesName = data->name();

    //d->view->setVisibility(1, layer);
    //d->view->setOpacity(1.0, layer); // why forcing visibility and opacity?
    d->visible->setChecked(true);

    //    if (d->dataComboBox->findText(seriesName) == -1)
    //        d->dataComboBox->insertItem(layer, seriesName);

    if (d->dataComboBox->findText(QString::number(layer)) == -1)
        d->dataComboBox->insertItem(layer, QString::number(layer));
    d->dataComboBox->blockSignals(true); // if not blocked, combobox slot is called. Is it what is expected?
    d->dataComboBox->setCurrentIndex(layer);
    d->dataComboBox->blockSignals(false);
    d->opacity->blockSignals(true);
    d->opacity->setValue(d->view->opacity(layer));
    d->opacity->blockSignals(false);
    d->opacitySlider->blockSignals(true);
    d->opacitySlider->setValue(d->view->opacity(layer) * 100);
    d->opacitySlider->blockSignals(false);

    d->view->setCurrentLayer(layer);
    d->lutComboBox->blockSignals(true);
    d->lutComboBox->setCurrentIndex(0);
    d->lutComboBox->blockSignals(false);
    d->view->setProperty("LookupTable", "Default");
    d->view->update();

    QTreeWidgetItem * layerItem = new QTreeWidgetItem(d->propertiesTree->invisibleRootItem());//(d->propertiesTree);
    layerItem->setText(0, QString::number(layer));


    QTreeWidgetItem * visibleItem = new QTreeWidgetItem(layerItem);
    visibleItem->setText(1, "Visible");
    QCheckBox * visibleBox = new QCheckBox();
    visibleBox->setChecked(d->visible->isChecked());
    d->propertiesTree->setItemWidget(visibleItem, 2, visibleBox);
    QObject::connect(visibleBox, SIGNAL(stateChanged(int)), this, SLOT(onVisibilitySet(int)));


    QTreeWidgetItem * opacityItem = new QTreeWidgetItem(layerItem);
    opacityItem->setText(1, "Opacity");
    QSlider * opacityBox = new QSlider(Qt::Horizontal);
    opacityBox->setRange(0,100);
    opacityBox->setValue(d->opacitySlider->value());
    d->propertiesTree->setItemWidget(opacityItem, 2, opacityBox);
    QObject::connect(opacityBox, SIGNAL(valueChanged(int)), this, SLOT(onOpacitySliderSet(int)));

    QTreeWidgetItem * lutItem = new QTreeWidgetItem(layerItem);
    lutItem->setText(1, "LUT");
    QComboBox * lutBox = new QComboBox();
    lutBox->setFocusPolicy(Qt::NoFocus);
    lutBox->addItem("Default");
    lutBox->addItem("Black & White");
    lutBox->addItem("Black & White Inversed");
    lutBox->addItem("Spectrum");
    lutBox->addItem("Hot Metal");
    lutBox->addItem("Hot Green");
    lutBox->addItem("Hot Iron");
    lutBox->addItem("GE");
    lutBox->addItem("Flow");
    lutBox->addItem("Loni");
    lutBox->addItem("Loni 2");
    lutBox->addItem("Asymmetry");
    lutBox->addItem("P-Value");
    lutBox->addItem("Red Black Alpha");
    lutBox->addItem("Green Black Alpha");
    lutBox->addItem("Blue Black Alpha");
    lutBox->addItem("Muscles & Bones");
    lutBox->addItem("Bones");
    lutBox->addItem("Red Vessels");
    lutBox->addItem("Cardiac");
    lutBox->addItem("Gray Rainbow");
    lutBox->addItem("Stern");
    lutBox->addItem("Black Body");

    d->propertiesTree->setItemWidget(lutItem, 2, lutBox);

    QObject::connect(lutBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onLUTChanged(int)));

    this->show();
}

void
medViewerToolBoxViewProperties::onViewClosed(void)
{
//    d->propertiesTree->clear();
    d->dataComboBox->clear();
    d->opacity->setValue(1.0);
    d->opacitySlider->setValue(100);
    d->visible->setChecked(true);
    d->lutComboBox->setCurrentIndex(0);
    d->propertiesToolBoxWidget->setEnabled(false);

    d->view = 0;

    //    this->hide();
}

void
medViewerToolBoxViewProperties::onDataSelected(int index)
{
    if (!d->view || (index == -1))
        return;

    // this method should update toolbox's GUI w.r.t selected data settings
    // to me, it should not set data settings (only current layer)
    d->view->setCurrentLayer(index);

    // d->view->setVisibility(d->view->visibility(index), index);

    d->visible->blockSignals(true);
    d->visible->setChecked((d->view->visibility(index) == 1));
    d->visible->blockSignals(false);
    // d->view->setOpacity(d->view->opacity(index), index);

    d->opacity->blockSignals(true);
    d->opacity->setValue(d->view->opacity(index));
    d->opacitySlider->setValue((int) (d->opacity->value() * 100));
    d->opacity->blockSignals(false);
}

void
medViewerToolBoxViewProperties::onVisibilitySet(int state)
{
    if ((!d->view) || (d->dataComboBox->currentIndex() == -1))
        return;

    if (state == Qt::Checked)
        d->view->setVisibility(1, d->dataComboBox->currentIndex());
    else
        d->view->setVisibility(0, d->dataComboBox->currentIndex());
    d->view->update();
}

void
medViewerToolBoxViewProperties::onOpacitySet(double opacity)
{
    if ((!d->view) || (d->dataComboBox->currentIndex() == -1))
        return;

    d->opacitySlider->blockSignals(true);
    d->opacitySlider->setValue((int) (d->opacity->value() * 100));
    d->opacitySlider->blockSignals(false);

    d->view->setOpacity(opacity, d->dataComboBox->currentIndex());
    d->view->update();
}

void
medViewerToolBoxViewProperties::onOpacitySliderSet(int opacity)
{
    if ((!d->view) || (d->dataComboBox->currentIndex() == -1))
        return;

    d->opacity->blockSignals(true);
    double d_opacity = static_cast<double> (opacity) / 100.0;
    d->opacity->setValue(d_opacity);
    d->opacity->blockSignals(false);

    d->view->setOpacity(d_opacity, d->dataComboBox->currentIndex());
    d->view->update();
}

void
medViewerToolBoxViewProperties::onLUTChanged(int index)
{
    if ((!d->view) || (d->dataComboBox->currentIndex() == -1))
        return;

    d->view->setProperty("LookupTable", d->lutComboBox->itemText(index));
    d->view->update();
}

