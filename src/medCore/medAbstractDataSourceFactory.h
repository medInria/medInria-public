#ifndef MEDABSTRACTDATASOURCEFACTORY_H
#define MEDABSTRACTDATASOURCEFACTORY_H

#include "medCoreExport.h"

#include <dtkCore/dtkAbstractFactory.h>

#include <QtCore>

class medAbstractDataSource;
class medAbstractDataSourceFactoryPrivate;

/** 
 * @class medSourceDataPluginFactory
 * @brief Dynamic source plugins (e.g. Shanoir, PACS, ...) factory
 * medAbstractSourceDataPlugin register to this factory. medBrowserArea instantiates them
 **/

class MEDCORE_EXPORT medAbstractDataSourceFactory : public dtkAbstractFactory
{
	Q_OBJECT
	
public:
	typedef medAbstractDataSource *(*medAbstractDataSourceCreator)(void);
	
	typedef QHash<QString, medAbstractDataSourceCreator> medAbstractDataSourceCreatorHash;
	
public:
	/** Static accessor to the instance of the factory */
	static medAbstractDataSourceFactory *instance(void);
	
	/** Method called by plugins to register themselves */
	bool registerDataSource(QString type, medAbstractDataSourceCreator func);
	
	/** Returns a list of available source data plugins */
	QList<QString> sourcedata_plugins(void);
	
	public slots:
	/** Method to instantiate a plugin */
	medAbstractDataSource *create(QString type);
	
protected:
	medAbstractDataSourceFactory(void);
	~medAbstractDataSourceFactory(void);
	
private:
	static medAbstractDataSourceFactory *s_instance;
	
private:
	medAbstractDataSourceFactoryPrivate *d;
};

#endif // MEDSOURCEDATAPLUGINFACTORY_H
