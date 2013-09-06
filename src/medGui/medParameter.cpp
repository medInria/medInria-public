#include <medParameter.h>
#include <QStringList>


//------------------ medBooleanParameter -------------------------------------

medBooleanParameter::medBooleanParameter(QString name, dtkAbstractObject *attachedData)
    : medAbstractParameter(name, attachedData)
{
    _attachedData = attachedData;
        setupWidget();
}

void medBooleanParameter::setValue(bool value)
{
    _checkbox->setChecked(value);
}

bool medBooleanParameter::value()
{
    return _checkbox->isChecked();
}

void medBooleanParameter::setupWidget()
{
    _checkbox = new QCheckBox;
    _layout->addWidget(_checkbox);

    connect(_checkbox, SIGNAL(toggled(bool)), this, SLOT(emitValueChanged(bool)));
}

void medBooleanParameter::emitValueChanged(bool value)
{
    if(dynamic_cast<dtkAbstractData*>(_attachedData) )
      emit valueChanged(dynamic_cast<dtkAbstractData*>(_attachedData) , value);
    else if(dynamic_cast<dtkAbstractProcess*>(_attachedData) )
      emit valueChanged(dynamic_cast<dtkAbstractProcess*>(_attachedData) , value);
    else emit valueChanged(value);
}

//------------------ medDoubleParameter -------------------------------------

medDoubleParameter::medDoubleParameter(QString name, dtkAbstractObject *attachedData)
    : medAbstractParameter(name, attachedData)
{
        setupWidget();
}

void medDoubleParameter::setupWidget()
{
    _slider = new medDoubleSliderSpinboxPair;
    _layout->addWidget(_slider);
    connect(_slider, SIGNAL(valueChanged(double)), this, SLOT(emitValueChanged(double)));
}

void medDoubleParameter::setValue(double value)
{
    _slider->setValue(value);
}

double medDoubleParameter::value()
{
    return _slider->value();
}

void medDoubleParameter::setMinimum(const double min)
{
    _slider->setMinimum(min);
}

void medDoubleParameter::setMaximum(const double max)
{
    _slider->setMaximum(max);
}

void medDoubleParameter::emitValueChanged(double value)
{
    if(dynamic_cast<dtkAbstractData*>(_attachedData) )
      emit valueChanged(dynamic_cast<dtkAbstractData*>(_attachedData) , value);
    else if(dynamic_cast<dtkAbstractProcess*>(_attachedData) )
      emit valueChanged(dynamic_cast<dtkAbstractProcess*>(_attachedData) , value);
    else emit valueChanged(value);
}

//------------------ medIntParameter -------------------------------------

medIntParameter::medIntParameter(QString name, dtkAbstractObject *attachedData)
    : medAbstractParameter(name, attachedData)
{
        setupWidget();
}

void medIntParameter::setupWidget()
{
    _slider = new medSliderSpinboxPair;
    _layout->addWidget(_slider);
    connect(_slider, SIGNAL(valueChanged(int)), this, SLOT(emitValueChanged(int)));
}

void medIntParameter::setValue(int value)
{
    _slider->setValue(value);
}

int medIntParameter::value()
{
    return _slider->value();
}

void medIntParameter::setMinimum(const int min)
{
    _slider->setMinimum(min);
}

void medIntParameter::setMaximum(const int max)
{
    _slider->setMaximum(max);
}

void medIntParameter::emitValueChanged(int value)
{
    if(dynamic_cast<dtkAbstractData*>(_attachedData) )
      emit valueChanged(dynamic_cast<dtkAbstractData*>(_attachedData) , value);
    else if(dynamic_cast<dtkAbstractProcess*>(_attachedData) )
      emit valueChanged(dynamic_cast<dtkAbstractProcess*>(_attachedData) , value);
    else emit valueChanged(value);
}

//------------------ medListParameter -------------------------------------

medListParameter::medListParameter(QString name, dtkAbstractObject *attachedData)
    : medAbstractParameter(name, attachedData)
{
    setupWidget();
}

void medListParameter::setupWidget()
{
    _comboBox = new QComboBox;
    _layout->addWidget(_comboBox);
    connect(_comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(emitValueChanged(QString)));
}

void medListParameter::setValues(QStringList values)
{
    _comboBox->clear();
    _comboBox->addItems(values);
}

void medListParameter::setDefaultValue(QString value)
{
    _comboBox->setCurrentIndex(_comboBox->findText(value));
}

QString medListParameter::value()
{
    return _comboBox->currentText();
}

void medListParameter::emitValueChanged(QString value)
{
    if(dynamic_cast<dtkAbstractData*>(_attachedData) )
      emit valueChanged(dynamic_cast<dtkAbstractData*>(_attachedData) , value);
    else if(dynamic_cast<dtkAbstractProcess*>(_attachedData) )
      emit valueChanged(dynamic_cast<dtkAbstractProcess*>(_attachedData) , value);
    else emit valueChanged(value);
}
