/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medViewContainersTest.h>

#include <medAbstractView.h>
#include <medViewContainer.h>
#include <medQtView.h>
#include <medQtDataImage.h>
#include <medAbstractDataFactory.h>
#include <medMetaDataKeys.h>

#include <QtGui>
#include <QtTest/QSignalSpy>



Q_DECLARE_METATYPE(QUuid)

medViewContainersTestObject::medViewContainersTestObject(void)
{
    qRegisterMetaType<dtkAbstractView*>("dtkAbstractView*");
    qRegisterMetaType<QUuid>("QUuid");
    m_currentId = 0;
}

medViewContainersTestObject::~medViewContainersTestObject(void)
{
}

void medViewContainersTestObject::initTestCase()
{
     QVERIFY( medQtDataImage::registered() );
}

void medViewContainersTestObject::init()
{
}

void medViewContainersTestObject::cleanup()
{
}

void medViewContainersTestObject::cleanupTestCase()
{
}

/*
void medViewContainersTestObject::testFoo_data()
{
}
*/

void medViewContainersTestObject::testSelection()
{
    medViewContainer *container = new medViewContainer;
    QUuid uuid = container->uuid();

    container->setFixedSize(500, 500);
    container->show();

    // setup signal spies
    QSignalSpy spy1 (container, SIGNAL(containerSelected(QUuid)));
    QSignalSpy spy2 (container, SIGNAL(containerUnSelected(QUuid)));

    container->setSelected(true);

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 0);
    QList<QVariant> arguments = spy1.takeFirst();
    QUuid receivedUuid = qvariant_cast<QUuid>(arguments.at(0));
    qDebug() << receivedUuid.toString() << uuid.toString();
    QVERIFY( receivedUuid == uuid );
    QVERIFY(container->isSelected() == true);

    container->setSelected(false);

    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 1);
    arguments = spy2.takeFirst();
    receivedUuid = qvariant_cast<QUuid>(arguments.at(0));
    QVERIFY( receivedUuid == uuid );
    QVERIFY(container->isSelected() == false);

    container->setSelected(true);

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 0);
    arguments = spy1.takeFirst();
    receivedUuid = qvariant_cast<QUuid>(arguments.at(0));
    QVERIFY( receivedUuid == uuid );
    QVERIFY(container->isSelected() == true);
}

void medViewContainersTestObject::testMaximization()
{
    medViewContainer *container = new medViewContainer;
    QUuid uuid = container->uuid();

    container->setFixedSize(500, 500);
    container->show();

    // setup signal spies
    QSignalSpy spy1 (container, SIGNAL(maximized(bool)));
    QSignalSpy spy2 (container, SIGNAL(maximized(QUuid,bool)));

    container->setMaximized(false);

    QCOMPARE(spy1.count(), 1);
    QVariant maximized = spy1.takeFirst().at(0);
    QVERIFY( maximized.toBool() == false );

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> arguments = spy2.takeFirst();
    maximized = arguments.at(1);
    QVERIFY( maximized.toBool() == false );
    QUuid receivedUuid = qvariant_cast<QUuid>(arguments.at(0));
    QVERIFY( receivedUuid == uuid );
    QVERIFY( container->isMaximized() == false );


    container->setMaximized(true);

    QCOMPARE(spy1.count(), 1);
    maximized = spy1.takeFirst().at(0);
    QVERIFY( maximized.toBool() == true );

    QCOMPARE(spy2.count(), 1);
    arguments = spy2.takeFirst();
    maximized = arguments.at(1);
    QVERIFY( maximized.toBool() == true );
    receivedUuid = qvariant_cast<QUuid>(arguments.at(0));
    QVERIFY( receivedUuid == uuid );
    QVERIFY( container->isMaximized() == true );

    container->setMaximized(false);

    QCOMPARE(spy1.count(), 1);
    maximized = spy1.takeFirst().at(0);
    QVERIFY( maximized.toBool() == false );

    QCOMPARE(spy2.count(), 1);
    arguments = spy2.takeFirst();
    maximized = arguments.at(1);
    QVERIFY( maximized.toBool() == false );
    receivedUuid = qvariant_cast<QUuid>(arguments.at(0));
    QVERIFY( receivedUuid == uuid );
    QVERIFY( container->isMaximized() == false );

    container->setMaximized(true);

    QCOMPARE(spy1.count(), 1);
    maximized = spy1.takeFirst().at(0);
    QVERIFY( maximized.toBool() == true );

    QCOMPARE(spy2.count(), 1);
    arguments = spy2.takeFirst();
    maximized = arguments.at(1);
    QVERIFY( maximized.toBool() == true );
    receivedUuid = qvariant_cast<QUuid>(arguments.at(0));
    QVERIFY( receivedUuid == uuid );
    QVERIFY( container->isMaximized() == true );
}

void medViewContainersTestObject::testSetView()
{
    medViewContainer *container = new medViewContainer;
    container->setFixedSize(500, 500);
    container->show();

    // create dummy view
    dtkSmartPointer<medQtView> view1 = new medQtView;

    // setup signal spies
    QSignalSpy spy1 (container, SIGNAL(contentChanged()));
    QSignalSpy spy2 (container, SIGNAL(contentRemoved()));

    // test setView:
    // - view should become visible
    // - container->view() should return view1
    // - syp1.count() should return 1
    container->setView(view1);

    QVERIFY(view1->viewWidget()->isVisible());
    QCOMPARE(container->view(), view1.data());
    QCOMPARE(spy1.count(), 1);

    // test null view
    container->setView ((medAbstractView*)NULL);
    QVERIFY(container->view()==NULL);

    // restore view1
    container->setView(view1);

    // create 2nd dummy view
    dtkSmartPointer<medQtView> view2 = new medQtView;

    // test view replacement:
    // - view1 should be hidden
    // - view2 should become visible
    // - spy1.count() should be 3
    // - spy2.count() should be 2
    // - container->view() should be view2
    container->setView(view2);

    QVERIFY(!view1->viewWidget()->isVisible());
    QVERIFY(view2->viewWidget()->isVisible());
    QCOMPARE(spy1.count(), 3);
    QCOMPARE(spy2.count(), 2);
    QCOMPARE(container->view(), view2.data());

    // test closing:
    // - view2 should be hidden
    // - spy2.count() should be 3
    container->close();

    QVERIFY(!view2->viewWidget()->isVisible());
    QCOMPARE(spy2.count(), 3);
}

void medViewContainersTestObject::testAddData()
{
    medViewContainer *container = new medViewContainer;
    container->setFixedSize(500, 500);
    container->show();

    // setup signal spies
    QSignalSpy spy1 (container, SIGNAL(contentChanged()));
    QSignalSpy spy2 (container, SIGNAL(contentRemoved()));
    QSignalSpy spy3 (container, SIGNAL(currentLayerChanged()));

    dtkSmartPointer<medAbstractData> data1 = createTestData();

    // create dummy view
    dtkSmartPointer<medQtView> view1 = new medQtView;

    container->setView(view1);
    container->addData(data1);

    // 2 contentChanged qsignals emitted, one for the view, one for the data addded
    QCOMPARE(spy1.count(), 2);
    QCOMPARE(spy3.count(), 1);

}

void medViewContainersTestObject::testHighlight()
{
}

void medViewContainersTestObject::testLink()
{
}

dtkSmartPointer<medAbstractData> medViewContainersTestObject::createTestData(void)
{
    // Create a data.
    medAbstractDataFactory *dataFactory = medAbstractDataFactory::instance();
    dtkSmartPointer<medAbstractData> testData = dataFactory->createSmartPointer(medQtDataImage::s_description());

    QString sDatetime = QDateTime::currentDateTime().toString();

    medMetaDataKeys::PatientName.set(testData,"TestPatient" + QString::number(m_currentId));
    medMetaDataKeys::StudyDescription.set(testData,"TestStudy" + QString::number(m_currentId));
    medMetaDataKeys::SeriesDescription.set(testData,"TestSeries" + QString::number(m_currentId));
    medMetaDataKeys::BirthDate.set(testData, sDatetime);
    m_currentId++;

    QImage testImage(QSize( 800, 500 ), QImage::Format_Mono );

    QPainter painter(&testImage);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::gray);
    painter.fillRect(testImage.rect(), Qt::black);

    painter.drawEllipse(QPoint(400,250), 300, 100);

    testData->setData( &testImage );
    return testData;
}

/**
   DTKTEST_NOGUI_MAIN will create the entry point without running
   a window manager (such as X on linux). If you need one, change
   it to DTKTEST_MAIN().
 **/
DTKTEST_MAIN(medViewContainersTest,medViewContainersTestObject)
