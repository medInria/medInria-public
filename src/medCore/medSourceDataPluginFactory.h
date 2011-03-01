#ifndef MEDSOURCEDATAPLUGINFACTORY_H
#define MEDSOURCEDATAPLUGINFACTORY_H

#include "medCoreExport.h"

#include <dtkCore/dtkAbstractFactory.h>

#include <QtCore>

class medAbstractSourceDataPlugin;
class medSourceDataPluginFactoryPrivate;

/** 
 * @class medSourceDataPluginFactory
 * @brief Dynamic source plugins (e.g. Shanoir, PACS, ...) factory
 * medAbstractSourceDataPlugin register to this factory. medBrowserArea instantiates them
 **/

class MEDCORE_EXPORT medSourceDataPluginFactory : public dtkAbstractFactory
{
	Q_OBJECT
	
public:
	typedef medAbstractSourceDataPlugin *(*medAbstractSourceDataPluginCreator)(void);
	
	typedef QHash<QString, medAbstractSourceDataPluginCreator> medAbstractSourceDataPluginCreatorHash;
	
public:
	/** Static accessor to the instance of the factory */
	static medSourceDataPluginFactory *instance(void);
	
	/** Method called by plugins to register themselves */
	bool registerSourceDataPlugin(QString type, medAbstractSourceDataPluginCreator func);
	
	/** Returns a list of available source data plugins */
	QList<QString> sourcedata_plugins(void);
	
	public slots:
	/** Method to instantiate a plugin */
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
