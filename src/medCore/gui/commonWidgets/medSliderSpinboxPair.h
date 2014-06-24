/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>
#include <QtGui>
#include <medCoreExport.h>

class MEDCORE_EXPORT medSliderSpinboxPair: public QWidget {

    Q_OBJECT

public:

    medSliderSpinboxPair(QWidget* parent=0): QWidget(parent) {
        slider  = new QSlider(Qt::Horizontal,this);
        spinbox = new QSpinBox(this);

        slider->setTracking(false);

        QHBoxLayout* layout = new QHBoxLayout(this);
        layout->setContentsMargins(0,0,0,0);
        layout->addWidget(slider);
        layout->addWidget(spinbox);

        connect(slider, SIGNAL(valueChanged(int)),this,SLOT(onValueChanged()));
        connect(spinbox,SIGNAL(editingFinished()),this,SLOT(onValueChanged()));
    }

    void setMinimum(const int min) {
        slider->setMinimum(min);
        spinbox->setMinimum(min);
    }

    void setMaximum(const int max) {
        slider->setMaximum(max);
        spinbox->setMaximum(max);
    }

    int value() const { return slider->value(); }

signals:

    void valueChanged(int);

public slots:

    void setValue(int value) {
        slider->blockSignals(true);
        spinbox->blockSignals(true);
        slider->setValue(value);
        spinbox->setValue(value);
        slider->blockSignals(false);
        spinbox->blockSignals(false);
        emit valueChanged(value);
    }

protected slots:

    void onValueChanged() {

        slider->blockSignals(true);
        spinbox->blockSignals(true);
        if (sender()==slider)
            spinbox->setValue(slider->value());
        else
            slider->setValue(spinbox->value());
        slider->blockSignals(false);
        spinbox->blockSignals(false);
        emit valueChanged(slider->value());
    }

private:

    QSlider*  slider;
    QSpinBox* spinbox;
};



class medDoubleSliderSpinboxPair: public QWidget {

    Q_OBJECT

public:

    medDoubleSliderSpinboxPair(QWidget* parent=0): QWidget(parent) {
        slider  = new QSlider(Qt::Horizontal,this);
        spinbox = new QDoubleSpinBox(this);

        slider->setTracking(false);

        QHBoxLayout* layout = new QHBoxLayout(this);
        layout->setContentsMargins(0,0,0,0);
        layout->addWidget(slider);
        layout->addWidget(spinbox);

        connect(slider, SIGNAL(valueChanged(int)),this,SLOT(onValueChanged()));
        connect(spinbox,SIGNAL(editingFinished()),this,SLOT(onValueChanged()));
    }

    void setMinimum(const double min) {
        slider->setMinimum(min);
        spinbox->setMinimum(min);
    }

    void setMaximum(const double max) {
        slider->setMaximum(max);
        spinbox->setMaximum(max);
    }

    double value() const { return slider->value(); }

signals:

    void valueChanged(double);

public slots:

    void setValue(double value) {
        slider->blockSignals(true);
        spinbox->blockSignals(true);
        slider->setValue(value);
        spinbox->setValue(value);
        slider->blockSignals(false);
        spinbox->blockSignals(false);
        emit(valueChanged(value));
    }

protected slots:

    void onValueChanged() {

        slider->blockSignals(true);
        spinbox->blockSignals(true);
        if (sender()==slider)
            spinbox->setValue(slider->value());
        else
            slider->setValue(spinbox->value());
        slider->blockSignals(false);
        spinbox->blockSignals(false);
        emit(valueChanged(slider->value()));
    }

private:

    QSlider*  slider;
    QDoubleSpinBox* spinbox;
};

