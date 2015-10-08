/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameterTest.h>


#include <QtGui>
#include <QSignalSpy>

#include <dtkCore/dtkSmartPointer.h>
#include <dtkCore/dtkAbstractObject.h>


#include <medAbstractParameter.h>
#include <medTestParameter.h>



medAbstractParameterTestObject::medAbstractParameterTestObject(void)
{
     m_name = QString("testWithObjectName");
     m_toolTip = QString("testWithObjectToolTip");
     m_label = NULL;
     m_widget = NULL;

}

medAbstractParameterTestObject::~medAbstractParameterTestObject(void)
{
}

void medAbstractParameterTestObject::initTestCase()
{
}

void medAbstractParameterTestObject::init()
{
}

void medAbstractParameterTestObject::cleanup()
{
}

void medAbstractParameterTestObject::cleanupTestCase()
{
}

void medAbstractParameterTestObject::testShowHideDelete()
{
    medTestParameter* testParameter = new medTestParameter(m_name);
    m_widget = testParameter->getWidget();
    m_label = testParameter->getLabel();
    QSignalSpy spy1(m_widget, SIGNAL(destroyed()));
    QSignalSpy spy2(m_label, SIGNAL(destroyed()));

    testParameter->show();
    QVERIFY(m_widget->isVisible());
    QVERIFY(m_label->isVisible());

    testParameter->hide();
    QVERIFY(!m_widget->isVisible());
    QVERIFY(!m_label->isVisible());

    delete testParameter;
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);
    m_label = NULL;
    m_widget = NULL;
}

void medAbstractParameterTestObject::testInternalSate()
{
    medTestParameter* testParameter = NULL;
    medTestParameter* testParameter2 = NULL;

    testParameter = new medTestParameter(m_name);
    testParameter2 = new medTestParameter(m_name);
    QVERIFY(testParameter->match(testParameter2));
    delete testParameter2;

    m_widget = testParameter->getWidget();
    m_label = testParameter->getLabel();
    testParameter->setToolTip(m_toolTip);
    QVERIFY(m_label->text() == m_name);
    QVERIFY(m_widget->toolTip() == m_toolTip);

    delete testParameter;
    testParameter = new medTestParameter(m_name);
    testParameter->setToolTip(m_toolTip);
    m_widget = testParameter->getWidget();
    m_label = testParameter->getLabel();
    QVERIFY(m_label->text() == m_name);
    QVERIFY(m_widget->toolTip() == m_toolTip);


    delete testParameter;
    m_widget = NULL;
    m_label = NULL;
}
DTKTEST_MAIN(medAbstractParameterTest, medAbstractParameterTestObject)
