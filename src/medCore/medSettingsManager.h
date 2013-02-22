
#ifndef medSettingsManager_h__
#define medSettingsManager_h__

#include <QtCore>

#include "medCoreExport.h"

class medSettingsManagerPrivate;



class MEDCORE_EXPORT medSettingsManager : public QObject
{
    Q_OBJECT

public:
    /**
    * instance - singleton access method, returns a singleViewContainer static instance of the manager
    * @return   medSettingsManager * - the manager
    */
    static medSettingsManager *instance(void);

    /**
    * destroy - should be called on closing the application, to destroy the singleton
    */
    static void destroy(void);

    /**
    * setValue
    * @params: const QString & key
    * @params: const QVariant & value
    * @return   void
    */
    void setValue( const QString & section, const QString & key, const QVariant & value );

    /**
    * value
    * @params: const QString & section
    * @params: const QString & key
    * @params: const QVariant & defaultValue
    * @return   QT_NAMESPACE::QVariant
    */
    QVariant value ( const QString & section, const QString & key, const QVariant & defaultValue = QVariant() );

signals:
    void settingsChanged( const QString & );

protected:
    medSettingsManager(void);
    ~medSettingsManager(void);

private:

    static medSettingsManager *s_instance;

    medSettingsManagerPrivate *d;
};
#endif // medSettingsManager_h__
