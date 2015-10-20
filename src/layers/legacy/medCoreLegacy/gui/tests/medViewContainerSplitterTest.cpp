/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medViewContainerSplitterTest.h"

#include <medViewContainerSplitter.h>
#include <medAbstractView.h>
#include <medViewContainer.h>

#include <QtGui>
#include <QtTest/QSignalSpy>


Q_DECLARE_METATYPE(QUuid)

medViewContainerSplitterTestObject::medViewContainerSplitterTestObject(void)
{
    qRegisterMetaType<dtkAbstractView*>("dtkAbstractView*");
    qRegisterMetaType<QUuid>("QUuid");
}

medViewContainerSplitterTestObject::~medViewContainerSplitterTestObject(void)
{
}

void medViewContainerSplitterTestObject::initTestCase()
{
}

void medViewContainerSplitterTestObject::init()
{
}

void medViewContainerSplitterTestObject::cleanup()
{
}

void medViewContainerSplitterTestObject::cleanupTestCase()
{
}

void medViewContainerSplitterTestObject::testAddViewContainer()
{
    medViewContainerSplitter *splitter = new medViewContainerSplitter();
    splitter->show();

    medViewContainer *container1 = new medViewContainer;

    QSignalSpy spy1(splitter, SIGNAL(newContainer(QUuid)));
    splitter->addViewContainer(container1);

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(splitter->count(), 1);
    QCOMPARE(splitter->widget(0), container1);

    QList<QVariant> arguments = spy1.takeFirst();
    QUuid receivedUuid = qvariant_cast<QUuid>(arguments.at(0));
    QCOMPARE(container1->uuid(), receivedUuid);

    medViewContainer *container2 = new medViewContainer;
    splitter->addViewContainer(container2);

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(splitter->count(), 2);
    QCOMPARE(splitter->widget(1), container2);

    arguments = spy1.takeFirst();
    receivedUuid = qvariant_cast<QUuid>(arguments.at(0));
    QCOMPARE(container2->uuid(), receivedUuid);

    delete splitter;
}

void medViewContainerSplitterTestObject::testInsertViewContainer()
{
    medViewContainerSplitter *splitter = new medViewContainerSplitter();
    splitter->show();

    medViewContainer *container1 = new medViewContainer;
    medViewContainer *container2 = new medViewContainer;

    splitter->addViewContainer(container1);
    splitter->addViewContainer(container2);

    QSignalSpy spy1(splitter, SIGNAL(newContainer(QUuid)));

    medViewContainer *containerToInsert = new medViewContainer;

    //insert a new container at the begining
    splitter->insertViewContainer(0, containerToInsert);

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(splitter->count(), 3);
    QCOMPARE(splitter->widget(0), containerToInsert);
    QCOMPARE(splitter->widget(1), container1);
    QCOMPARE(splitter->widget(2), container2);

    QList<QVariant> arguments = spy1.takeFirst();
    QUuid receivedUuid = qvariant_cast<QUuid>(arguments.at(0));
    QCOMPARE(containerToInsert->uuid(), receivedUuid);


    medViewContainer *containerToInsert2 = new medViewContainer;

    //insert a new container at the end
    splitter->insertViewContainer(3, containerToInsert2);

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(splitter->count(), 4);
    QCOMPARE(splitter->widget(0), containerToInsert);
    QCOMPARE(splitter->widget(1), container1);
    QCOMPARE(splitter->widget(2), container2);
    QCOMPARE(splitter->widget(3), containerToInsert2);

    arguments = spy1.takeFirst();
    receivedUuid = qvariant_cast<QUuid>(arguments.at(0));
    QCOMPARE(containerToInsert2->uuid(), receivedUuid);


    medViewContainer *containerToInsert3 = new medViewContainer;

    //insert a new container at the middle
    splitter->insertViewContainer(2, containerToInsert3);

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(splitter->count(), 5);
    QCOMPARE(splitter->widget(0), containerToInsert);
    QCOMPARE(splitter->widget(1), container1);
    QCOMPARE(splitter->widget(2), containerToInsert3);
    QCOMPARE(splitter->widget(3), container2);
    QCOMPARE(splitter->widget(4), containerToInsert2);

    arguments = spy1.takeFirst();
    receivedUuid = qvariant_cast<QUuid>(arguments.at(0));
    QCOMPARE(containerToInsert3->uuid(), receivedUuid);


    delete splitter;

}

void medViewContainerSplitterTestObject::testSplitVertically()
{
    medViewContainerSplitter *splitter = new medViewContainerSplitter();
    splitter->setOrientation(Qt::Vertical);
    splitter->show();

    medViewContainer *container1 = new medViewContainer;
    splitter->addViewContainer(container1);

    QSignalSpy spy1(splitter, SIGNAL(newContainer(QUuid)));

    medViewContainer *newContainer = splitter->splitVertically(container1);

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(splitter->count(), 1);

    medViewContainerSplitter *nestedSplitter = dynamic_cast<medViewContainerSplitter*>(splitter->widget(0));
    QCOMPARE(nestedSplitter->widget(0), container1);
    QCOMPARE(nestedSplitter->widget(1), newContainer);
    QCOMPARE(nestedSplitter->orientation(), Qt::Horizontal);

    QList<QVariant> arguments = spy1.takeFirst();
    QUuid receivedUuid = qvariant_cast<QUuid>(arguments.at(0));
    QCOMPARE(newContainer->uuid(), receivedUuid);

    medViewContainer *newContainer2 = nestedSplitter->splitVertically(newContainer);

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(nestedSplitter->count(), 3);
    QCOMPARE(nestedSplitter->widget(0), container1);
    QCOMPARE(nestedSplitter->widget(1), newContainer);
    QCOMPARE(nestedSplitter->widget(2), newContainer2);
}

void medViewContainerSplitterTestObject::testSplitHorizontally()
{
    medViewContainerSplitter *splitter = new medViewContainerSplitter();
    splitter->setOrientation(Qt::Horizontal);
    splitter->show();

    medViewContainer *container1 = new medViewContainer;
    splitter->addViewContainer(container1);

    QSignalSpy spy1(splitter, SIGNAL(newContainer(QUuid)));

    medViewContainer *newContainer = splitter->splitHorizontally(container1);

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(splitter->count(), 1);

    medViewContainerSplitter *nestedSplitter = dynamic_cast<medViewContainerSplitter*>(splitter->widget(0));
    QCOMPARE(nestedSplitter->widget(0), container1);
    QCOMPARE(nestedSplitter->widget(1), newContainer);
    QCOMPARE(nestedSplitter->orientation(), Qt::Vertical);

    QList<QVariant> arguments = spy1.takeFirst();
    QUuid receivedUuid = qvariant_cast<QUuid>(arguments.at(0));
    QCOMPARE(newContainer->uuid(), receivedUuid);

    medViewContainer *newContainer2 = nestedSplitter->splitHorizontally(newContainer);

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(nestedSplitter->count(), 3);
    QCOMPARE(nestedSplitter->widget(0), container1);
    QCOMPARE(nestedSplitter->widget(1), newContainer);
    QCOMPARE(nestedSplitter->widget(2), newContainer2);
}

void medViewContainerSplitterTestObject::testSplit()
{
}

/**
   DTKTEST_NOGUI_MAIN will create the entry point without running
   a window manager (such as X on linux). If you need one, change
   it to DTKTEST_MAIN().
 **/
DTKTEST_MAIN(medViewContainerSplitterTest,medViewContainerSplitterTestObject)
