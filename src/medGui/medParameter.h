#ifndef MEDPARAMETER_H
#define MEDPARAMETER_H

#include <QtGui>
#include <medSliderSpinboxPair.h>
#include <dtkCore/dtkAbstractObject.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractData.h>


class medAbstractParameter : public QObject
{
    Q_OBJECT

public:
    medAbstractParameter(QString name, dtkAbstractObject* attachedData = 0) : _name(name), _widget(0), _attachedData(attachedData)
    {
        _widget = new QWidget;
        _layout = new QHBoxLayout(_widget);
        _label = new QLabel(name);
        _layout->addWidget(_label);

        _layout->setContentsMargins(1,1,1,1);
    }
    virtual ~medAbstractParameter() {delete _widget;}

    QString name() const {return _name;}

    QWidget* getWidget() {if (!_widget) setupWidget(); return _widget;}

    // pour les medParamPools ? ptet pas utile
    //virtual void sync(medAbstractParam * p) = 0;

protected:
    virtual void setupWidget() = 0;

    QString       _name;
    QWidget *     _widget;
    QLabel *      _label;
    QHBoxLayout * _layout;
    dtkAbstractObject *_attachedData;

};


class medBooleanParameter : public medAbstractParameter
{
    Q_OBJECT

public:
    medBooleanParameter(QString name, dtkAbstractObject* attachedData = 0);

    void setValue(bool value);
    bool value();

signals:
    void valueChanged(bool value);
    void valueChanged(dtkAbstractData * attachedData, bool value);
    void valueChanged(dtkAbstractProcess * attachedProcess, bool value);

protected:
    void setupWidget();

private slots:
    void emitValueChanged(bool);

private:
    QCheckBox * _checkbox;

};

class medDoubleParameter: public medAbstractParameter
{
    Q_OBJECT

public:
    medDoubleParameter(QString name, dtkAbstractObject* attachedData = 0);

    void setValue(double value);
    double value();
    void setMinimum(const double min);
    void setMaximum(const double max);

signals:
    void valueChanged(double value);
    void valueChanged(dtkAbstractData * attachedData, double value);
    void valueChanged(dtkAbstractProcess * attachedProcess, double value);

protected:
    void setupWidget();

private slots:
    void emitValueChanged(double);

private:
    medDoubleSliderSpinboxPair *_slider;
};


class medIntParameter : public medAbstractParameter
{

    Q_OBJECT

public:
    medIntParameter(QString name, dtkAbstractObject* attachedData = 0);

    void setValue(int value);
    int value();
    void setMinimum(const int min);
    void setMaximum(const int max);

signals:
    void valueChanged(int value);
    void valueChanged(dtkAbstractData * attachedData, int value);
    void valueChanged(dtkAbstractProcess * attachedProcess, int value);

protected:
    void setupWidget();

private slots:
    void emitValueChanged(int);

private:
    medSliderSpinboxPair *_slider;
};


class medListParameter : public medAbstractParameter
{
    Q_OBJECT

public:
    medListParameter(QString name, dtkAbstractObject* attachedData = 0);

    void setValues(QStringList values);
    void setDefaultValue(QString value);
    QString value();

signals:
    void valueChanged(QString value);
    void valueChanged(dtkAbstractData * attachedData, QString value);
    void valueChanged(dtkAbstractProcess * attachedProcess, QString value);

protected:
    void setupWidget();

private slots:
    void emitValueChanged(QString);

private:
    QComboBox *_comboBox;
};

#endif // MEDPARAMETERFACTORY_H
