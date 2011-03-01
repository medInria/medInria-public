#ifndef medDbController_h__
#define medDbController_h__


#include "medSqlExport.h"
#include <medCore/medAbstractDbController.h>

class dtkAbstractData;

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

    dtkAbstractData *read(const medDataIndex& index);
    dtkAbstractData *read(int patientId, int studyId, int seriesId);
    dtkAbstractData *read(int patientId, int studyId, int seriesId, int imageId);

    bool moveDatabase(QString newLocation);

    bool isConnected();

public slots:
    void import(const QString& file);

protected slots:
    // some helpers
    bool copyFiles(QStringList sourceList, QStringList destList);
    bool createDestination(QStringList sourceList, QStringList& destList, QString sourceDir, QString destDir);
    void recurseAddDir(QDir d, QStringList & list);
    bool removeDir(QString dirName);

private:
    void createPatientTable(void);
    void   createStudyTable(void);
    void  createSeriesTable(void);
    void   createImageTable(void);

    QSqlDatabase m_database;
    bool m_isConnected;
};

#endif