/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#pragma once

#include <dtkCore/dtkTest.h>
#include <medAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medDataIndex.h>
#include <medDataManager.h>
#include <medAbstractDbController.h>


class medDataManagerTestObject : public QObject
{
    Q_OBJECT

public:
             medDataManagerTestObject(void);
    virtual ~medDataManagerTestObject(void);
    
    typedef QList<medDataIndex> IndexList;
    
signals:
    void dataAdded();
    void dataRemoved();
    
private slots:
    void initTestCase(void);
    void init(void);
    void cleanup(void);
    void cleanupTestCase(void);

private slots:
    void testImport(void);
    void testImportNonPersistent(void);
    void testImport2NonPersistentSeries(void);
    void testImportFile(void);
    void testIndexFile(void);
    void testImportNonPersistentFile(void);
    void testClearNonPersistentData(void);
    void testStoreNonPersistentDataToDatabase (void);
    void testStoreNonPersistentMultipleDataToDatabase(void);
    void testStoreNonPersistentSingleDataToDatabase(void);
    void testRemoveData(void);
    void testMoveStudy(void);
    void testMoveSerie(void);
  
private slots:
    void onDataAdded(const medDataIndex& index);
    void onDataRemoved(const medDataIndex& index);
    
    
private:
    dtkSmartPointer<medAbstractData> createTestData(void);
    bool removeDir(const QString & dirName);
    void compareData(void);
    void compareData(dtkSmartPointer<medAbstractData> data1, dtkSmartPointer<medAbstractData> data2);
    void waitForInsertions(int numberOfInsertions = 1, int timeout = 10000 );
    void waitForDeletions(int numberOfDeletions = 1, int timeout = 10000 );
     
private:
    dtkSmartPointer<medAbstractData> m_currentData;
    IndexList m_insertedIndexes;
    medDataIndex m_lastInsertedIndex;
    
    medAbstractDbController * db;
    medAbstractDbController * npDb;
    QString m_storagePath;
    QString m_fileToImport;
    int m_currentId;
    QEventLoop m_eventLoop;
    
};





