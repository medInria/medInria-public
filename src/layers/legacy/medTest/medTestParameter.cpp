#include <medTestParameter.h>


class medTestParameterPrivate
{
public:
    QWidget *widget;

    ~medTestParameterPrivate() {delete widget;}
};


medTestParameter::medTestParameter(QString name):
    medAbstractParameterL(name),
    d(new medTestParameterPrivate)
{
    d->widget = NULL;
}

medTestParameter::~medTestParameter()
{
    delete d;
}

QWidget* medTestParameter::getWidget()
{
    if(!d->widget)
    {
        d->widget = new QWidget;
        this->addToInternWidgets(d->widget);
        connect(d->widget, SIGNAL(destroyed()), this, SLOT(removeInternWidget()));
    }

    return d->widget;
}

void medTestParameter::removeInternWidget()
{
    this->removeFromInternWidgets(d->widget);
    d->widget = NULL;
}
