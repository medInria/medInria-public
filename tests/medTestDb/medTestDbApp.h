#ifndef MEDTESTDBAPP_H
#define MEDTESTDBAPP_H

#include <QMutex>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>
#include <dtkLog/dtkLog.h>

#include <medTestCommon/medTestApplication.h>
#include <medDataIndex.h>
#include <medDataManager.h>

class medTestDbApp : public medTestApplication
{
    Q_OBJECT
public:
    medTestDbApp( int argc, char *argv[]);
    virtual ~medTestDbApp();
    void run();

    dtkSmartPointer<dtkAbstractData> CreateTestData( );
    int getTestResult()const {return testResult;}

    /**
     * Overloads parent class method to catch all exceptions thrown in slots.
     *
     */
    bool notify(QObject *rec, QEvent *ev) {
        try {
            return QCoreApplication::notify(rec, ev);
        }
        catch (std::exception & e) {
            dtkDebug() << "Exception caught : "<< e.what();
            testResult = DTK_FAILURE;
            exit();
        }
        catch (...) {
            dtkDebug() << "Exception caught : " << "[Unknown exception type]";
            testResult = DTK_FAILURE;
            exit();
        }
        return false;
    }

public slots:

    void onNonPersistentDataImported(const medDataIndex& index);
    void onPersistentDataImported(const medDataIndex& index);
    void onImportFailed(const medDataIndex& index,const QString & uuid);


private:
    void handlePersistentDataImported();
    void handleNonPersistentDataImported();
    medDataIndex   importedIndex;
    int            testResult;
    medDataManager *dataManager;
    dtkSmartPointer<dtkAbstractData> testData;
    medAbstractDbController * db;
    medAbstractDbController * npDb;
};

#endif // MEDTESTDBAPP_H
