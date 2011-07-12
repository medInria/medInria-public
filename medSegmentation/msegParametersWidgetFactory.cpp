/*
 * msegParametersWidgetFactory.cpp
 *
 *  Created on: 31 mai 2011 14:55:00
 *      Author: jstark
 */

#include "msegParametersWidgetFactory.h"

namespace mseg {

ParametersWidgetFactory *ParametersWidgetFactory::s_instance = NULL;

class ParametersWidgetFactory::ParametersWidgetFactoryPrivate {
public:
    struct ParametersWidgetInfo {
        QString type;
        QString localizedName;
        ParametersWidgetFactory::CreatorFunc func;
    };
    typedef QHash<QString, ParametersWidgetInfo> ParametersWidgetFactoryInfoHash;
    ParametersWidgetFactoryInfoHash creators;
};

ParametersWidgetFactory::ParametersWidgetFactory() : dtkAbstractFactory(), d(new ParametersWidgetFactoryPrivate)
{

}

ParametersWidgetFactory::~ParametersWidgetFactory()
{
    delete d;
    d = NULL;
}

ParametersWidgetFactory *ParametersWidgetFactory::instance(void)
{
    if(!s_instance)
        s_instance = new ParametersWidgetFactory;

    return s_instance;
}

QList<QString> ParametersWidgetFactory::widgets(void)
{
    return d->creators.keys();
}

AlgorithmParametersWidget *ParametersWidgetFactory::create(QString type, Controller * controller, QWidget *parent)
{
    if(d->creators.contains(type))
        return  d->creators[type].func(controller, parent);

    return NULL;
}

QString ParametersWidgetFactory::localizedName( const QString &type ) const
{
    if(d->creators.contains(type))
        return d->creators.find(type)->localizedName;

    return QString();
}

bool ParametersWidgetFactory::registerWidget(const QString &type, const QString &localizedName, CreatorFunc func)
{
    if(d->creators.contains(type))
        return false;  // Already have a widget with this name registered.

    d->creators[type].type = type;
    d->creators[type].localizedName = localizedName;
    d->creators[type].func = func;

    emit widgetAdded( type );
}


} //namespace

