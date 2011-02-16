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
        QWidget* propertiesToolBoxWidget;
        QComboBox * dataComboBox;
        QComboBox * lutComboBox;
        QCheckBox * visible;
        QDoubleSpinBox * opacity;
        QSlider * opacitySlider;
};

medViewerToolBoxViewProperties::medViewerToolBoxViewProperties(QWidget *parent) :
    medToolBox(parent), d(new medViewerToolBoxViewPropertiesPrivate)
{
    d->view = 0;
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

    this->setTitle("View properties");
    this->addWidget(d->propertiesToolBoxWidget);

    QObject::connect(d->dataComboBox,   SIGNAL(currentIndexChanged(int)),       this, SLOT(onDataSelected(int)));
    QObject::connect(d->visible,        SIGNAL(stateChanged(int)),              this, SLOT(onVisibilitySet(int)));
    QObject::connect(d->opacity,        SIGNAL(valueChanged(double)),           this, SLOT(onOpacitySet(double)));
    QObject::connect(d->opacitySlider,  SIGNAL(valueChanged(int)),              this, SLOT(onOpacitySliderSet(int)));
    QObject::connect(d->lutComboBox,    SIGNAL(currentIndexChanged(int)),       this, SLOT(onLUTChanged(int)));
}

medViewerToolBoxViewProperties::~medViewerToolBoxViewProperties(void)
{
    delete d;
    d = NULL;
}

void
medViewerToolBoxViewProperties::update(dtkAbstractView *view)
{    
    if (d->view)
    {
        QObject::disconnect(d->view, SIGNAL(dataAdded(int)), this, SLOT(onDataAdded(int)));
        QObject::disconnect(d->view, SIGNAL(closing()),      this, SLOT(onViewClosed()));
        
        this->onViewClosed();
    }
    
    if (medAbstractView *medView = dynamic_cast<medAbstractView *> (view))
    {        
        d->view = medView;

        QObject::connect(d->view, SIGNAL(dataAdded(int)), this, SLOT(onDataAdded(int)), Qt::UniqueConnection);
        QObject::connect(d->view, SIGNAL(closing()), this, SLOT(onViewClosed()), Qt::UniqueConnection);
        
        d->propertiesToolBoxWidget->setEnabled(true);
    }
}

void
medViewerToolBoxViewProperties::onDataAdded(int layer)
{
    if (!d->view)
        return;

    //d->view->setVisibility(1, layer);
    //d->view->setOpacity(1.0, layer); // why forcing visibility and opacity?
    d->visible->setChecked(true);
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
}

void
medViewerToolBoxViewProperties::onViewClosed(void)
{
    d->dataComboBox->clear();
    d->opacity->setValue(1.0);
    d->opacitySlider->setValue(100);
    d->visible->setChecked(true);
    d->propertiesToolBoxWidget->setEnabled(false);
    
    d->view = 0;
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

