/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTriggerParameterTest.h>


#include <QtGui>
#include <QSignalSpy>

#include <medTriggerParameter.h>



medTriggerParameterTestObject::medTriggerParameterTestObject(void)
{


     m_name = QString("testWithObjectName");
     m_text = QString("testWithObjectToolText");
     m_icon = QIcon(":/pixmaps/plus_button.png");
     m_iconSize = QSize(42,42);
     m_pushButton = NULL;
     m_widget = NULL;
}

medTriggerParameterTestObject::~medTriggerParameterTestObject(void)
{
}

void medTriggerParameterTestObject::initTestCase()
{
}

void medTriggerParameterTestObject::init()
{
}

void medTriggerParameterTestObject::cleanup()
{
}

void medTriggerParameterTestObject::cleanupTestCase()
{
}

void medTriggerParameterTestObject::testShowHideDelete()
{
    medTriggerParameter* triggerParameter = new medTriggerParameter(m_name);
    m_widget = triggerParameter->getWidget();
    m_pushButton = triggerParameter->getPushButton();
    QSignalSpy spy1(m_widget, SIGNAL(destroyed()));
    QSignalSpy spy2(m_pushButton, SIGNAL(destroyed()));

    triggerParameter->show();
    QVERIFY(m_pushButton->isVisible());
    QVERIFY(m_widget->isVisible());
    triggerParameter->hide();
    QVERIFY(!m_pushButton->isVisible());
    QVERIFY(!m_widget->isVisible());

    delete triggerParameter;
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);
    m_pushButton = NULL;
    m_widget = NULL;
}

void medTriggerParameterTestObject::testInternalSate()
{
    medTriggerParameter* triggerParameter = NULL;

    // test gui stuff
    triggerParameter = new medTriggerParameter(m_name);
    m_pushButton = triggerParameter->getPushButton();
    triggerParameter->setButtonText(m_text);
    triggerParameter->setButtonIcon(m_icon);
    triggerParameter->setButtonIconSize(m_iconSize);
    QVERIFY(m_pushButton->text() == m_text);
    QCOMPARE(m_pushButton->icon(),  m_icon);
    QVERIFY(m_pushButton->minimumSize() == m_iconSize);
    QVERIFY(m_pushButton->iconSize() == m_iconSize);

    delete triggerParameter;
    triggerParameter = new medTriggerParameter(m_name);
    triggerParameter->setButtonText(m_text);
    triggerParameter->setButtonIcon(m_icon);
    triggerParameter->setButtonIconSize(m_iconSize);
    m_pushButton = triggerParameter->getPushButton();
    QVERIFY(m_pushButton->text() == m_text);
    QCOMPARE(m_pushButton->icon(),  m_icon);
    QVERIFY(m_pushButton->minimumSize() == m_iconSize);
    QVERIFY(m_pushButton->iconSize() == m_iconSize);

    delete triggerParameter;
    m_pushButton = NULL;
}

void medTriggerParameterTestObject::testSignalsAndAttachedData()
{
    medTriggerParameter* triggerParameter = NULL;

    triggerParameter = new medTriggerParameter(m_name);
    m_pushButton = triggerParameter->getPushButton();
    QSignalSpy spy1(triggerParameter, SIGNAL(triggered()));

    QMetaObject::invokeMethod(m_pushButton, "clicked");
    QCOMPARE(spy1.count(), 1);

    delete triggerParameter;
    m_pushButton = NULL;
}
DTKTEST_MAIN(medTriggerParameterTest, medTriggerParameterTestObject)
