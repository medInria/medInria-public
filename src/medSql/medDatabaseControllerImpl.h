#ifndef medDbController_h__
#define medDbController_h__


#include "medSqlExport.h"
#include <medCore/medAbstractDbController.h>

class dtkAbstractData;
class SigEmitter;

/**
 * Concrete dbController implementation adhering to abstract base class
 */
class MEDSQL_EXPORT medDatabaseControllerImpl: public medAbstractDbController 
{
    Q_OBJECT

public:

    medDatabaseControllerImpl();
    ~medDatabaseControllerImpl();

    QSqlDatabase *database(void);

    bool createConnection(void);
    bool  closeConnection(void);

    medDataIndex indexForPatient(int id);
    medDataIndex indexForStudy  (int id);
    medDataIndex indexForSeries (int id);
    medDataIndex indexForImage  (int id);

    QStringList patientNames(void);

    dtkAbstractData *read(int patientId, int studyId, int seriesId);
    dtkAbstractData *read(int patientId, int studyId, int seriesId, int imageId);

    bool moveDatabase(QString newLocation);

    bool isConnected();

signals:
    /**
     * Status message from controller to some user interface
     */
    void copyMessage(QString, int, QColor);


public slots:
    dtkAbstractData *read(const medDataIndex& index) const;
    medDataIndex import(const QString& file);
    medDataIndex import(const dtkAbstractData& data);

protected slots:
    void forwardMessage(QString);

private:
    void createPatientTable(void);
    void   createStudyTable(void);
    void  createSeriesTable(void);
    void   createImageTable(void);

    /*
    bool copyFiles(QStringList sourceList, QStringList destList);
    bool createDestination(QStringList sourceList, QStringList& destList, QString sourceDir, QString destDir);
    void recurseAddDir(QDir d, QStringList & list);
    bool removeDir(QString dirName);
    */

    QSqlDatabase m_database;
    bool m_isConnected;
    SigEmitter* emitter;
};

#endif