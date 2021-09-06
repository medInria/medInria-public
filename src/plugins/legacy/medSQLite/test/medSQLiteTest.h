//
// Created by Julien Castelneau on 26/07/2021.
//

#define private public
#include "medSQLite.h"

static bool driverAvailability = true;
static bool valid;
static bool flagOpen;
static QStringList fakeTables = QStringList();
static int removeCount;

class FakeEngine
{
public:
    FakeEngine(){};
    FakeEngine(const FakeEngine &other){};
    FakeEngine &operator=(const FakeEngine &other) { return *this; };

    static bool isDriverAvailable(const QString &name) { return driverAvailability; }

    bool isValid() { return valid; };
    bool open() { return flagOpen; };
    void setDatabaseName(const QString &name){};
    MOCK_METHOD(void, close, ());
    MOCK_METHOD(bool, isOpen, ());
    MOCK_METHOD(QSqlQuery, exec, ());
//    QSqlQuery exec(){return QSqlQuery();};
    QStringList tables(){return fakeTables;};

    static FakeEngine &addDatabase(const QString &, const QString &)
    {
        static FakeEngine db;
        return db;
    };

    static FakeEngine &database(const QString &)
    {
        static FakeEngine db;
        return db;
    };

    static void removeDatabase(const QString &)
    {
        removeCount++;
    };
};

class FakeMedSQLite : public medSQlite<FakeEngine>
{
public:
    FakeMedSQLite &operator=(const FakeMedSQLite &other) { return *this; };
    void changeDatabasePath(const QString& value) {
        m_DbPath->setValue(value);
    }

    MOCK_METHOD(bool, createTable, (const QString &));
    MOCK_METHOD(void, optimizeSpeedSQLiteDB, ());
    MOCK_METHOD(QList<levelMinimalEntries>, getPatientMinimalEntries, ());
    MOCK_METHOD(QList<levelMinimalEntries>, getStudyMinimalEntries, (QString& parentId));
    MOCK_METHOD(QList<levelMinimalEntries>, getSeriesMinimalEntries, (QString& parentId));
    MOCK_METHOD(QString, getSeriesDirectData, (QString& key));
};

// Test Integration
class RealMedSQLite : public medSQlite<QSqlDatabase>
{
public:
    void changeDatabasePath(const QString& value) {
        m_DbPath->setValue(value);
    }

};