
#ifndef medSettingsManager_h__
#define medSettingsManager_h__

#include <QtCore>

#include "medCoreExport.h"

class medSettingsManagerPrivate;


enum ESections {
    SEC_NONE,
    SEC_GENERAL,
    SEC_SYSTEM,
    SEC_DICOM,
    SEC_STARTUP,
    SEC_LOGGING
};

class MEDCORE_EXPORT medSettingsManager : public QObject
{
    Q_OBJECT

public:
    /**
    * instance - singleton access method, returns a single static instance of the manager
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
    void setValue( ESections section, const QString & key, const QVariant & value );

    /**
    * value
    * @params: const QString & key
    * @params: const QVariant & defaultValue
    * @return   QT_NAMESPACE::QVariant
    */
    QVariant value ( ESections section, const QString & key, const QVariant & defaultValue = QVariant() );

signals:
    void settingsChanged( ESections );

protected:
    medSettingsManager(void);
    ~medSettingsManager(void);

private:
    // converts the enum to a string to be used internally
    QString sectionEnumToString(ESections section);

    static medSettingsManager *s_instance;

    medSettingsManagerPrivate *d;
};
#endif // medSettingsManager_h__
