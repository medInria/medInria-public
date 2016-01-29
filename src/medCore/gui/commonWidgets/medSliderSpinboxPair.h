/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

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


class QDoubleSlider : public QSlider{
    Q_OBJECT
public:

    explicit QDoubleSlider(Qt::Orientation orientation, QWidget *parent = 0):QSlider(orientation,parent)
    {
        connect(this,SIGNAL(valueChanged(int)),this,SLOT(onValueChanged(int)));
        step=1;
    };
    explicit QDoubleSlider(QWidget *parent = 0):QSlider(parent)
    {
        connect(this,SIGNAL(valueChanged(int)),this,SLOT(onValueChanged(int)));
        step=1;
    };

    public slots:

        void onValueChanged(int i)
        {
            emit valueChanged(value());  
        };

signals:
        void valueChanged(double);

public:
    void setMinimum(double min)
    {
        this->min = min;
        QSlider::setMinimum(ceil(min/step));
    };
    void setMaximum(double max)
    {
        this->max = max;
        QSlider::setMaximum(ceil(max/step));
    };

    double value()
    {
        int i = QSlider::value();
        return i*step;
    };

    void setValue(double d)
    {
        int m = floor((d/step)+0.5);
        QSlider::setValue(m);;    
    };

    void setStep(double s)
    {
      step = s;
      QSlider::setMaximum(ceil(max/step));
      QSlider::setMinimum(ceil(min/step));
      QSlider::setSingleStep(1);
      };

private:
    double min;              
    double max;              
    double step;         
};

class MEDCORE_EXPORT medDoubleSliderSpinboxPair: public QWidget {

    Q_OBJECT

public:

    medDoubleSliderSpinboxPair(QWidget* parent=0): QWidget(parent) {
        slider  = new QDoubleSlider(Qt::Horizontal,this);
        spinbox = new QDoubleSpinBox(this);

        slider->setTracking(false);

        QHBoxLayout* layout = new QHBoxLayout(this);
        layout->setContentsMargins(0,0,0,0);
        layout->addWidget(slider);
        layout->addWidget(spinbox);

        connect(slider, SIGNAL(valueChanged(double)),this,SLOT(onValueChanged()));
        connect(slider, SIGNAL(sliderMoved(double)), this, SLOT(updateSpinbox(double)));
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

    void setStep(double s){
        slider->setStep(s);
    }

    double value() const { return slider->value(); }

    void setSpinBoxSuffix(QString suffix) { spinbox->setSuffix(suffix); }

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

    void setEnabled(bool boolean){
        slider->setEnabled(boolean);
        spinbox->setEnabled(boolean);
    }

    void updateSpinbox(double value)
    {
        spinbox->blockSignals(true);
        spinbox->setValue(value);
        spinbox->blockSignals(false);
    }

protected slots:

    void onValueChanged() {
        //editingFinished is emitted when we press Enter AND when the spinBox loses focus
        if (sender()==spinbox && !spinbox->hasFocus()) //we ignore the latter
            return;
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

    QDoubleSlider*  slider;
    QDoubleSpinBox* spinbox;
};


