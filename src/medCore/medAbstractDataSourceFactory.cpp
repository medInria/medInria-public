#include "medAbstractDataSourceFactory.h"
#include "medAbstractDataSource.h"

class medAbstractDataSourceFactoryPrivate
{
public:
    medAbstractDataSourceFactory::medAbstractDataSourceCreatorHash dataSource_creators;
};

medAbstractDataSourceFactory *medAbstractDataSourceFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medAbstractDataSourceFactory;

    return s_instance;
}

bool medAbstractDataSourceFactory::registerDataSource(QString type, medAbstractDataSourceCreator func)
{
    if(!d->dataSource_creators.contains(type)) {
        d->dataSource_creators.insert(type, func);
        return true;
    }

    return false;
}

QList<QString> medAbstractDataSourceFactory::dataSourcePlugins(void)
{
    return d->dataSource_creators.keys();
}

medAbstractDataSource *medAbstractDataSourceFactory::create(QString type,QWidget * parent)
{
    if(!d->dataSource_creators.contains(type))
        return NULL;

    medAbstractDataSource *conf = d->dataSource_creators[type](parent);

    return conf;
}

medAbstractDataSourceFactory::medAbstractDataSourceFactory(void) : dtkAbstractFactory(), d(new medAbstractDataSourceFactoryPrivate)
{
}

medAbstractDataSourceFactory::~medAbstractDataSourceFactory(void)
{
    delete d;
    d = NULL;
}

medAbstractDataSourceFactory *medAbstractDataSourceFactory::s_instance = NULL;
