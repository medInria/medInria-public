/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medViewContainersTest.h"

#include <medAbstractView.h>
#include <medSingleViewContainer.h>
#include <medMultiViewContainer.h>
#include <medCustomViewContainer.h>

#include <QtGui>
#include <QtTest/QSignalSpy>


class testView : public medAbstractView
{
public:
     testView();
    ~testView();

    QString identifier(void) const;

public:
    QWidget *widget(void);
    virtual medAbstractViewCoordinates * coordinates() {return NULL;}

public slots:
    void close(void);

private:
    QWidget *m_widget;
};

testView::testView()
{
    m_widget = new QWidget;
    QLabel *label = new QLabel ("testView", m_widget);
    QHBoxLayout * layout = new QHBoxLayout (m_widget);
    layout->addWidget(label);
}

testView::~testView()
{
}

QString testView::identifier() const
{
    return "testView";
}

QWidget *testView::widget()
{
    return m_widget;
}

void testView::close()
{
    m_widget->close();
    emit closed();
}


medViewContainersTestObject::medViewContainersTestObject(void)
{
    qRegisterMetaType<dtkAbstractView*>("dtkAbstractView*");
}

medViewContainersTestObject::~medViewContainersTestObject(void)
{
}

void medViewContainersTestObject::initTestCase()
{
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

void medViewContainersTestObject::testSingle()
{
    // create a view container singleViewContainer
    medSingleViewContainer *container = new medSingleViewContainer;
    container->setFixedSize(500, 500);
    container->show();

    // setup signal spies
    QSignalSpy spy1 (container, SIGNAL(viewAdded(dtkAbstractView*)));
    QSignalSpy spy2 (container, SIGNAL(viewRemoved(dtkAbstractView*)));
    QSignalSpy spy3 (container, SIGNAL(focused(dtkAbstractView*)));

    // create dummy view
    dtkSmartPointer<testView>view1 = new testView;

    // test setView:
    // - view should become visible
    // - container->view() should return view1
    // - syp1.count() should return 1
    container->setView(view1);

    QVERIFY(view1->widget()->isVisible());
    QCOMPARE(container->view(), view1.data());
    QCOMPARE(spy1.count(), 1);

    // test focus:
    // - container->current() should be itself
    // - spy3.count() should be 1
    container->setFocus (Qt::MouseFocusReason);
    QTest::qWait(500);
    QVERIFY (container->current()==container);
    QCOMPARE (spy3.count(), 1);

    // test null view
    container->setView ((dtkAbstractView*)NULL);
    container->view();
    QVERIFY(!view1->widget()->isVisible());
    QVERIFY(container->view()==NULL);

    // restore view1
    container->setView(view1);

    // create 2nd dummy view
    dtkSmartPointer<testView> view2 = new testView;

    // test view replacement:
    // - view1 should be hidden
    // - view2 should become visible
    // - spy1.count() should be 3
    // - spy2.count() should be 2
    // - container->view() should be view2
    container->setView(view2);

    QVERIFY(!view1->widget()->isVisible());
    QVERIFY(view2->widget()->isVisible());
    QCOMPARE(spy1.count(), 3);
    QCOMPARE(spy2.count(), 2);
    QCOMPARE(container->view(), view2.data());

    // test closing:
    // - view2 should be hidden
    // - spy2.count() should be 3
    // - container->view() should be null
    QMetaObject::invokeMethod(view2, "closing", Qt::DirectConnection);

    QVERIFY(!view2->widget()->isVisible());
    QCOMPARE(spy2.count(), 3);
    QCOMPARE(container->view(), (dtkAbstractView*)NULL);

    // cleanup before exiting test function
    delete container;
}

void medViewContainersTestObject::testMulti()
{
    // create a multi container
    medMultiViewContainer *container = new medMultiViewContainer;
    container->setFixedSize(500, 500);
    container->show();

    QSignalSpy spy1 (container, SIGNAL(viewAdded(dtkAbstractView*)));
    QSignalSpy spy2 (container, SIGNAL(viewRemoved(dtkAbstractView*)));

    // create 4 dummy views
    dtkSmartPointer<testView> view1 = new testView;
    dtkSmartPointer<testView> view2 = new testView;
    dtkSmartPointer<testView> view3 = new testView;
    dtkSmartPointer<testView> view4 = new testView;

    // test setView:
    // - views should be shown
    // - child count should be 4
    container->setView(view1);
    container->setView(view2);
    container->setView(view3);
    container->setView(view4);

    // important to wait to let the event queue proceeds
    // some events
    QTest::qWait(500);

    QVERIFY(view1->widget()->isVisible());
    QVERIFY(view2->widget()->isVisible());
    QVERIFY(view3->widget()->isVisible());
    QVERIFY(view4->widget()->isVisible());
    QCOMPARE(spy1.count(), 4);
    QVERIFY(container->views().contains(view1));
    QVERIFY(container->views().contains(view2));
    QVERIFY(container->views().contains(view3));
    QVERIFY(container->views().contains(view4));
    QCOMPARE(container->childContainers().count(), 4);

    // test a NULL view
    container->setView (NULL);
    QCOMPARE(container->childContainers().count(), 4);

    // test full screen
    QMetaObject::invokeMethod(view1, "fullScreen", Qt::DirectConnection, Q_ARG(bool, true));
    QVERIFY(view1->widget()->isVisible());
    QVERIFY(!view2->widget()->isVisible());
    QVERIFY(!view3->widget()->isVisible());
    QVERIFY(!view4->widget()->isVisible());

    QTest::qWait(500);

    QMetaObject::invokeMethod(view1, "fullScreen", Qt::DirectConnection, Q_ARG(bool, false));
    QVERIFY(view1->widget()->isVisible());
    QVERIFY(view2->widget()->isVisible());
    QVERIFY(view3->widget()->isVisible());
    QVERIFY(view4->widget()->isVisible());

    QTest::qWait(500);

    // test closing
    QMetaObject::invokeMethod(view1, "closing", Qt::DirectConnection);

    QTest::qWait(500);

    QMetaObject::invokeMethod(view2, "closing", Qt::DirectConnection);

    QTest::qWait(500);

    QMetaObject::invokeMethod(view3, "closing", Qt::DirectConnection);

    QTest::qWait(500);

    QMetaObject::invokeMethod(view4, "closing", Qt::DirectConnection);

    QTest::qWait(500);

    QVERIFY(!view1->widget()->isVisible());
    QVERIFY(!view2->widget()->isVisible());
    QVERIFY(!view3->widget()->isVisible());
    QVERIFY(!view4->widget()->isVisible());

    QCOMPARE(spy2.count(), 4);
    QVERIFY(!container->views().contains(view1));
    QVERIFY(!container->views().contains(view2));
    QVERIFY(!container->views().contains(view3));
    QVERIFY(!container->views().contains(view4));
    QCOMPARE(container->childContainers().count(), 0);

    delete container;
}

void medViewContainersTestObject::testCustom()
{
    // create the custom container
    medCustomViewContainer *container = new medCustomViewContainer;
    container->setFixedSize(500, 500);
    container->setPreset (medCustomViewContainer::A);
    container->show();

    for (int preset = medCustomViewContainer::A;
         preset <= medCustomViewContainer::E; preset++) {
        container->setPreset (preset);
        foreach (medViewContainer *c, container->leaves()) {
            c->setFocus (Qt::MouseFocusReason);
            testView *view = new testView;
            c->setView (view);
            QVERIFY(container->views().contains(view));
        }

        QTest::qWait(500);
    }

    // my favorite: let's try to set NULL view to preset E:
    container->setPreset (medCustomViewContainer::E);
    foreach (medViewContainer *c, container->leaves()) {
        c->setFocus (Qt::MouseFocusReason);
        c->setView (NULL);
    }
    QCOMPARE(container->views().count(), 0);
    delete container;
}

/**
   DTKTEST_NOGUI_MAIN will create the entry point without running
   a window manager (such as X on linux). If you need one, change
   it to DTKTEST_MAIN().
 **/
DTKTEST_MAIN(medViewContainersTest,medViewContainersTestObject)
