#ifndef MEDSOURCEDATAPLUGINFACTORY_H
#define MEDSOURCEDATAPLUGINFACTORY_H

#include "medCoreExport.h"

#include <dtkCore/dtkAbstractFactory.h>

#include <QtCore>

class medAbstractSourceDataPlugin;
class medSourceDataPluginFactoryPrivate;

class MEDCORE_EXPORT medSourceDataPluginFactory : public dtkAbstractFactory
{
	Q_OBJECT
	
public:
	typedef medAbstractSourceDataPlugin *(*medAbstractSourceDataPluginCreator)(void);
	
	typedef QHash<QString, medAbstractSourceDataPluginCreator> medAbstractSourceDataPluginCreatorHash;
	
public:
	static medSourceDataPluginFactory *instance(void);
	
	bool registerSourceDataPlugin(QString type, medAbstractSourceDataPluginCreator func);
	
	QList<QString> sourcedata_plugins(void);
	
	public slots:
	medAbstractSourceDataPlugin *create(QString type);
	
protected:
	medSourceDataPluginFactory(void);
	~medSourceDataPluginFactory(void);
	
private:
	static medSourceDataPluginFactory *s_instance;
	
private:
	medSourceDataPluginFactoryPrivate *d;
};

#endif // MEDSOURCEDATAPLUGINFACTORY_H
