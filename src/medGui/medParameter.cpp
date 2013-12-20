#include <medParameter.h>
#include <QStringList>


//------------------ medActionParameter -------------------------------------

medActionParameter::medActionParameter(QString name, dtkAbstractObject *attachedObject)
    : medAbstractParameter(name, attachedObject)
{
     setupWidget();
}

void medActionParameter::setIcon(QIcon icon)
{
    _button->setIcon(icon);
}

void medActionParameter::setIconSize(int width, int height)
{
    _button->setIconSize(QSize(width, height));
    _button->setMinimumSize(width, height);
}

void medActionParameter::setText(QString value)
{
    _button->setText(value);
}

void medActionParameter::setCheckable(bool value)
{
    _button->setCheckable(value);
}


void medActionParameter::setupWidget()
{
    initializeWidget();
    _button = new QPushButton;
    _button->setToolTip(_name);
    _layout->addWidget(_button);

    connect(_button, SIGNAL(clicked()), this, SLOT(emitTriggered()));
    connect(_button, SIGNAL(toggled(bool)), this, SLOT(emitTriggered(bool)));
}

void medActionParameter::emitTriggered(bool value)
{
    if(dynamic_cast<dtkAbstractData*>(_attachedObject) )
    {
      emit triggered(dynamic_cast<dtkAbstractData*>(_attachedObject));
      emit triggered(dynamic_cast<dtkAbstractData*>(_attachedObject), value);
    }
    else if(dynamic_cast<dtkAbstractProcess*>(_attachedObject) )
    {
      emit triggered(dynamic_cast<dtkAbstractProcess*>(_attachedObject));
      emit triggered(dynamic_cast<dtkAbstractProcess*>(_attachedObject), value);
    }
    else if(dynamic_cast<dtkAbstractView*>(_attachedObject) )
    {
      emit triggered(dynamic_cast<dtkAbstractView*>(_attachedObject));
      emit triggered(dynamic_cast<dtkAbstractView*>(_attachedObject), value);
    }

    emit triggered();
}


//------------------ medBooleanParameter -------------------------------------

medBooleanParameter::medBooleanParameter(QString name, dtkAbstractObject *attachedObject)
    : medAbstractParameter(name, attachedObject)
{
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
    initializeWidget();
    _checkbox = new QCheckBox;
    _layout->addWidget(_checkbox);

    connect(_checkbox, SIGNAL(toggled(bool)), this, SLOT(emitValueChanged(bool)));
}

void medBooleanParameter::emitValueChanged(bool value)
{
    if(dynamic_cast<dtkAbstractData*>(_attachedObject) )
      emit valueChanged(dynamic_cast<dtkAbstractData*>(_attachedObject) , value);
    else if(dynamic_cast<dtkAbstractProcess*>(_attachedObject) )
      emit valueChanged(dynamic_cast<dtkAbstractProcess*>(_attachedObject) , value);
    else if(dynamic_cast<dtkAbstractView*>(_attachedObject) )
      emit valueChanged(dynamic_cast<dtkAbstractView*>(_attachedObject) , value);

    emit valueChanged(value);
}

//------------------ medDoubleParameter -------------------------------------

medDoubleParameter::medDoubleParameter(QString name, dtkAbstractObject *attachedObject)
    : medAbstractParameter(name, attachedObject)
{
        setupWidget();
}

void medDoubleParameter::setupWidget()
{
    initializeWidget();
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
    if(dynamic_cast<dtkAbstractData*>(_attachedObject) )
      emit valueChanged(dynamic_cast<dtkAbstractData*>(_attachedObject) , value);
    else if(dynamic_cast<dtkAbstractProcess*>(_attachedObject) )
      emit valueChanged(dynamic_cast<dtkAbstractProcess*>(_attachedObject) , value);
    else if(dynamic_cast<dtkAbstractView*>(_attachedObject) )
      emit valueChanged(dynamic_cast<dtkAbstractView*>(_attachedObject) , value);

    emit valueChanged(value);
}

//------------------ medIntParameter -------------------------------------

medIntParameter::medIntParameter(QString name, dtkAbstractObject *attachedObject)
    : medAbstractParameter(name, attachedObject)
{
        setupWidget();
}

void medIntParameter::setupWidget()
{
    initializeWidget();
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
    if(dynamic_cast<dtkAbstractData*>(_attachedObject) )
      emit valueChanged(dynamic_cast<dtkAbstractData*>(_attachedObject) , value);
    else if(dynamic_cast<dtkAbstractProcess*>(_attachedObject) )
      emit valueChanged(dynamic_cast<dtkAbstractProcess*>(_attachedObject) , value);
    else if(dynamic_cast<dtkAbstractView*>(_attachedObject) )
      emit valueChanged(dynamic_cast<dtkAbstractView*>(_attachedObject) , value);

    emit valueChanged(value);
}

//------------------ medListParameter -------------------------------------

medListParameter::medListParameter(QString name, dtkAbstractObject *attachedObject)
    : medAbstractParameter(name, attachedObject)
{
    setupWidget();
}

void medListParameter::setupWidget()
{
    initializeWidget();
    _comboBox = new QComboBox;
    _layout->addWidget(_comboBox);
    connect(_comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(emitValueChanged(QString)));
    connect(_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(emitValueChanged(int)));
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
    if(dynamic_cast<dtkAbstractData*>(_attachedObject) )
      emit valueChanged(dynamic_cast<dtkAbstractData*>(_attachedObject) , value);
    else if(dynamic_cast<dtkAbstractProcess*>(_attachedObject) )
      emit valueChanged(dynamic_cast<dtkAbstractProcess*>(_attachedObject) , value);
    else if(dynamic_cast<dtkAbstractView*>(_attachedObject) )
      emit valueChanged(dynamic_cast<dtkAbstractView*>(_attachedObject) , value);

    emit valueChanged(value);
}

void medListParameter::emitValueChanged(int value)
{
    if(dynamic_cast<dtkAbstractData*>(_attachedObject) )
      emit valueChanged(dynamic_cast<dtkAbstractData*>(_attachedObject) , value);
    else if(dynamic_cast<dtkAbstractProcess*>(_attachedObject) )
      emit valueChanged(dynamic_cast<dtkAbstractProcess*>(_attachedObject) , value);
    else if(dynamic_cast<dtkAbstractView*>(_attachedObject) )
      emit valueChanged(dynamic_cast<dtkAbstractView*>(_attachedObject) , value);

    emit valueChanged(value);
}

//------------------ medColorListParameter -------------------------------------

medColorListParameter::medColorListParameter(QString name, dtkAbstractObject *attachedObject)
    : medAbstractParameter(name, attachedObject)
{
    setupWidget();
}

void medColorListParameter::setupWidget()
{
    initializeWidget();
    _comboBox = new QComboBox;
    _layout->addWidget(_comboBox);
    connect(_comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(emitValueChanged(QString)));
}

void medColorListParameter::setValues(QList<QColor> values)
{
    _comboBox->clear();

    foreach(QColor color, values)
    {
        _comboBox->addItem(createIcon(color.name()),color.name(),color);
    }
}

void medColorListParameter::setValues(QStringList values)
{
    _comboBox->clear();

    foreach(QString color, values)
    {
        _comboBox->addItem(createIcon(color),color,QColor(color));
    }
}

void medColorListParameter::setDefaultValue(QString value)
{
    _comboBox->setCurrentIndex(_comboBox->findText(value));
}

void medColorListParameter::setDefaultValue(QColor value)
{
    _comboBox->setCurrentIndex(_comboBox->findText(value.name()));
}

QString medColorListParameter::value()
{
    return _comboBox->currentText();
}

void medColorListParameter::emitValueChanged(QString value)
{
    QColor color = QColor(value);

    if(dynamic_cast<dtkAbstractData*>(_attachedObject) )
    {
        emit valueChanged(dynamic_cast<dtkAbstractData*>(_attachedObject) , value);
        emit valueChanged(dynamic_cast<dtkAbstractData*>(_attachedObject) , color);
    }
    else if(dynamic_cast<dtkAbstractProcess*>(_attachedObject) )
    {
        emit valueChanged(dynamic_cast<dtkAbstractProcess*>(_attachedObject) , value);
        emit valueChanged(dynamic_cast<dtkAbstractProcess*>(_attachedObject) , color);
    }
    else if(dynamic_cast<dtkAbstractView*>(_attachedObject) )
    {
        emit valueChanged(dynamic_cast<dtkAbstractView*>(_attachedObject) , value);
        emit valueChanged(dynamic_cast<dtkAbstractView*>(_attachedObject) , color);
    }

    emit valueChanged(value);
}

QIcon medColorListParameter::createIcon(QString colorName)
{
    QPixmap iconPixmap(32,32);
    iconPixmap.fill(QColor(colorName));
    QIcon itemIcon(iconPixmap);
    return itemIcon;
}


//------------------ medColorListParameter -------------------------------------

medParameterPool::medParameterPool()
{

}

medParameterPool::~medParameterPool()
{
    _pool.clear();
}

void medParameterPool::append(medAbstractParameter *parameter)
{
    _pool.insert(parameter->name(), parameter);
    connectParam(parameter);
}

void medParameterPool::clear()
{
    foreach(medAbstractParameter* param, _pool)
        disconnectParam(param);

    _pool.clear();
}

void medParameterPool::connectParam(medAbstractParameter *parameter)
{
    if(medActionParameter *param = dynamic_cast<medActionParameter*>(parameter))
        connect(param, SIGNAL(triggered()), this, SLOT(triggerParams()));
    else if(medBooleanParameter *param = dynamic_cast<medBooleanParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(bool)), this, SLOT(triggerParams(bool)));
    else if(medIntParameter *param = dynamic_cast<medIntParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(int)), this, SLOT(triggerParams(int)));
    else if(medDoubleParameter *param = dynamic_cast<medDoubleParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(double)), this, SLOT(triggerParams(double)));
    else if(medListParameter *param = dynamic_cast<medListParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(QString)), this, SLOT(triggerParams(QString)));
    else if(medColorListParameter *param = dynamic_cast<medColorListParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(QString)), this, SLOT(triggerParams(QString)));
}

void medParameterPool::disconnectParam(medAbstractParameter *parameter)
{
    parameter->disconnect(this);
}

void medParameterPool::triggerParams()
{
    medActionParameter *sender = dynamic_cast<medActionParameter*>(QObject::sender());

    if(!sender)
        return;

    foreach(medAbstractParameter *param, _pool.values(sender->name()))
    {
        medActionParameter *actionParam = dynamic_cast<medActionParameter*>(param);
        if(actionParam != sender)
        {
            disconnectParam(param);
            actionParam->emitTriggered();
            connectParam(param);
        }
    }
}

void medParameterPool::triggerParams(bool value)
{
    medBooleanParameter *sender = dynamic_cast<medBooleanParameter*>(QObject::sender());

    if(!sender)
        return;

    foreach(medAbstractParameter *param, _pool.values(sender->name()))
    {
        medBooleanParameter *boolParam = dynamic_cast<medBooleanParameter*>(param);
        if(boolParam != sender)
            boolParam->setValue(value);
    }
}

void medParameterPool::triggerParams(int value)
{
    medIntParameter *sender = dynamic_cast<medIntParameter*>(QObject::sender());

    if(!sender)
        return;

    foreach(medAbstractParameter *param, _pool.values(sender->name()))
    {
        medIntParameter *intParam = dynamic_cast<medIntParameter*>(param);
        if(intParam != sender)
            intParam->setValue(value);
    }
}

void medParameterPool::triggerParams(double value)
{
    medDoubleParameter *sender = dynamic_cast<medDoubleParameter*>(QObject::sender());

    if(!sender)
        return;

    foreach(medAbstractParameter *param, _pool.values(sender->name()))
    {
        medDoubleParameter *doubleParam = dynamic_cast<medDoubleParameter*>(param);
        if(doubleParam != sender)
            doubleParam->setValue(value);
    }
}

void medParameterPool::triggerParams(QString value)
{
    medListParameter *sender = dynamic_cast<medListParameter*>(QObject::sender());

    if(!sender)
        return;

    foreach(medAbstractParameter *param, _pool.values(sender->name()))
    {
        medListParameter *listParam = dynamic_cast<medListParameter*>(param);
        if(listParam != sender)
            listParam->setDefaultValue(value);
    }
}
