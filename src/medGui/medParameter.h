#ifndef MEDPARAMETER_H
#define MEDPARAMETER_H

#include <QtGui>
#include <medSliderSpinboxPair.h>
#include <dtkCore/dtkAbstractObject.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractView.h>

class medParameterPool;

class medAbstractParameter : public QObject
{
    Q_OBJECT

public:
    medAbstractParameter(QString name, dtkAbstractObject* attachedObject = 0)
        : _name(name), _widget(0), _attachedObject(attachedObject){}

    virtual ~medAbstractParameter() {delete _widget;}

    QString name() const {return _name;}

    QWidget* getWidget() {if (!_widget) setupWidget(); return _widget;}

    bool match(medAbstractParameter const *other)
    {
        return this->name()==other->name();
    }

    void hideLabel(){ _label->hide() ;}

protected:
    virtual void setupWidget() = 0;

    void initializeWidget()
    {
        _widget = new QWidget;
        _layout = new QHBoxLayout(_widget);
        _label = new QLabel(_name);
        _layout->addWidget(_label);

        _layout->setContentsMargins(1,1,1,1);

        connect(_widget, SIGNAL(destroyed()), this, SLOT(deleteWidget()));
    }

    QString       _name;
    QWidget *     _widget;
    QLabel *      _label;
    QHBoxLayout * _layout;
    dtkAbstractObject *_attachedObject;

private slots:
    void deleteWidget(){_widget=0;}

};


class medActionParameter : public medAbstractParameter
{
    Q_OBJECT

public:
    medActionParameter(QString name, dtkAbstractObject* attachedObject = 0);

    void setIcon(QIcon icon);
    void setText(QString text);
    void setCheckable(bool value);

public slots:
    void emitTriggered(bool value = true);

signals:
    void triggered();
    void triggered(dtkAbstractData * attachedData);
    void triggered(dtkAbstractProcess * attachedProcess);
    void triggered(dtkAbstractView * attachedProcess);

    void triggered(bool);
    void triggered(dtkAbstractData * attachedData, bool);
    void triggered(dtkAbstractProcess * attachedProcess, bool);
    void triggered(dtkAbstractView * attachedProcess, bool);

protected:
    void setupWidget();


private:
    QPushButton * _button;

};


class medActionParameterGroup : QObject
{
    Q_OBJECT

public:
    medActionParameterGroup(QWidget *parent){ _buttonGroup = new QButtonGroup(parent);}

    void addActionParameter(medActionParameter *action)
    {
        const QObjectList children = action->getWidget()->children();
        foreach(QObject *child, children)
        {
            QPushButton *button = dynamic_cast<QPushButton *>(child);
            if( button )
                _buttonGroup->addButton(button);
        }
    }
    void setExclusive(bool exclusive){_buttonGroup->setExclusive(exclusive);}

private:
    QButtonGroup *_buttonGroup;
};



class medBooleanParameter : public medAbstractParameter
{
    Q_OBJECT

public:
    medBooleanParameter(QString name, dtkAbstractObject* attachedObject = 0);

    void setValue(bool value);
    bool value();

signals:
    void valueChanged(bool value);
    void valueChanged(dtkAbstractData * attachedData, bool value);
    void valueChanged(dtkAbstractProcess * attachedProcess, bool value);
    void valueChanged(dtkAbstractView * attachedView, bool value);

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
    medDoubleParameter(QString name, dtkAbstractObject* attachedObject = 0);

    void setValue(double value);
    double value();
    void setMinimum(const double min);
    void setMaximum(const double max);

signals:
    void valueChanged(double value);
    void valueChanged(dtkAbstractData * attachedData, double value);
    void valueChanged(dtkAbstractProcess * attachedProcess, double value);
    void valueChanged(dtkAbstractView * attachedView, double value);

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
    medIntParameter(QString name, dtkAbstractObject* attachedObject = 0);

    void setValue(int value);
    int value();
    void setMinimum(const int min);
    void setMaximum(const int max);

signals:
    void valueChanged(int value);
    void valueChanged(dtkAbstractData * attachedData, int value);
    void valueChanged(dtkAbstractProcess * attachedProcess, int value);
    void valueChanged(dtkAbstractView * attachedView, int value);

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
    medListParameter(QString name, dtkAbstractObject* attachedObject = 0);

    void setValues(QStringList values);
    void setDefaultValue(QString value);
    QString value();

signals:
    void valueChanged(QString value);
    void valueChanged(dtkAbstractData * attachedData, QString value);
    void valueChanged(dtkAbstractProcess * attachedProcess, QString value);
    void valueChanged(dtkAbstractView * attachedView, QString value);
    void valueChanged(int value);
    void valueChanged(dtkAbstractData * attachedData, int value);
    void valueChanged(dtkAbstractProcess * attachedProcess, int value);
    void valueChanged(dtkAbstractView * attachedView, int value);

protected:
    void setupWidget();

private slots:
    void emitValueChanged(QString);
    void emitValueChanged(int value);

private:
    QComboBox *_comboBox;
};


class medColorListParameter : public medAbstractParameter
{
    Q_OBJECT

public:
    medColorListParameter(QString name, dtkAbstractObject* attachedObject = 0);

    void setValues(QList<QColor> values);
    void setValues(QStringList values);

    void setDefaultValue(QColor value);
    void setDefaultValue(QString value);

    QString value();

signals:
    void valueChanged(QColor value);
    void valueChanged(dtkAbstractData * attachedData, QColor value);
    void valueChanged(dtkAbstractProcess * attachedProcess, QColor value);
    void valueChanged(dtkAbstractView * attachedView, QColor value);

    void valueChanged(QString value);
    void valueChanged(dtkAbstractData * attachedData, QString value);
    void valueChanged(dtkAbstractProcess * attachedProcess, QString value);
    void valueChanged(dtkAbstractView * attachedView, QString value);

protected:
    void setupWidget();
    QIcon createIcon(QString colorName);

private slots:
    void emitValueChanged(QString);

private:
    QComboBox *_comboBox;
};


class medParameterPool : public QObject
{
    Q_OBJECT

public:
    medParameterPool();
    virtual ~medParameterPool();

    void append(medAbstractParameter *parameter);
    void clear();
    int count(){return _pool.count();}

private slots:
    void triggerParams();
    void triggerParams(bool);
    void triggerParams(int);
    void triggerParams(double);
    void triggerParams(QString);

private:
    void connectParam(medAbstractParameter *parameter);
    void disconnectParam(medAbstractParameter *parameter);

private:
    QMultiMap<QString, medAbstractParameter*> _pool;

};

#endif // MEDPARAMETERFACTORY_H
