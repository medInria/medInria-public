//
// Created by Julien Castelneau on 26/07/2021.
//

#define private public
#include "medSQLite.h"

class FakeEngine
{
public:

    FakeEngine(){};
    FakeEngine(const FakeEngine &other){};
    FakeEngine &operator=(const FakeEngine &other) { return *this; };

    MOCK_METHOD(void, setDatabaseName, (const QString &));
    MOCK_METHOD(bool, isValid, ());
    MOCK_METHOD(bool, open, ());
    MOCK_METHOD(void, close, ());
    MOCK_METHOD(bool, isOpen, ());
    MOCK_METHOD(QSqlQuery, exec, ());
    MOCK_METHOD(QStringList, tables, ());

    static bool isDriverAvailable(const QString &name) { return true; }
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
    static void removeDatabase(const QString&){};

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
    MOCK_METHOD(void, removeDatabase, ());
    MOCK_METHOD(bool, isDriverAvailable, ());
    MOCK_METHOD(FakeEngine, addDatabase, ());
    MOCK_METHOD(FakeEngine, database, ());
};

// Test Integration
class RealMedSQLite : public medSQlite<QSqlDatabase>
{
public:
    void changeDatabasePath(const QString& value) {
        m_DbPath->setValue(value);
    }

};