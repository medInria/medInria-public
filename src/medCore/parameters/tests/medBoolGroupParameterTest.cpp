/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medBoolGroupParameterTest.h>


#include <QtGui>
#include <QSignalSpy>

#include <medBoolParameter.h>
#include <medBoolGroupParameter.h>

medBoolGroupParameterTestObject::medBoolGroupParameterTestObject(void)
{
     m_name = QString("testWithObjectName");
     m_checkBoxGroup = NULL;
     m_radioButtonGroup = NULL;
     m_pushButtonGroup = NULL;
     m_widget = NULL;
}

medBoolGroupParameterTestObject::~medBoolGroupParameterTestObject(void)
{
}

void medBoolGroupParameterTestObject::initTestCase()
{
}

void medBoolGroupParameterTestObject::init()
{
}

void medBoolGroupParameterTestObject::cleanup()
{
}

void medBoolGroupParameterTestObject::cleanupTestCase()
{
}

void medBoolGroupParameterTestObject::testShowHideDelete()
{
    medBoolGroupParameter* boolParameter = new medBoolGroupParameter(m_name);
    m_widget = boolParameter->getWidget();
    m_pushButtonGroup = boolParameter->getPushButtonGroup();
    m_checkBoxGroup = boolParameter->getCheckBoxGroup();
    m_radioButtonGroup = boolParameter->getRadioButtonGroup();

    QSignalSpy spy1(m_pushButtonGroup, SIGNAL(destroyed()));
    QSignalSpy spy2(m_checkBoxGroup, SIGNAL(destroyed()));
    QSignalSpy spy3(m_radioButtonGroup, SIGNAL(destroyed()));
    QSignalSpy spy4(m_widget, SIGNAL(destroyed()));

    boolParameter->show();
    QVERIFY(m_widget->isVisible());
    QVERIFY(m_pushButtonGroup->isVisible());
    QVERIFY(m_checkBoxGroup->isVisible());
    QVERIFY(m_radioButtonGroup->isVisible());
    boolParameter->hide();
    QVERIFY(!m_widget->isVisible());
    QVERIFY(!m_pushButtonGroup->isVisible());
    QVERIFY(!m_checkBoxGroup->isVisible());
    QVERIFY(!m_radioButtonGroup->isVisible());

    delete boolParameter;
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(spy3.count(), 1);
    QCOMPARE(spy4.count(), 1);

    m_pushButtonGroup = NULL;
    m_widget = NULL;
    m_radioButtonGroup = NULL;
    m_checkBoxGroup = NULL;
}

void medBoolGroupParameterTestObject::testInternalSate()
{
    medBoolGroupParameter *boolGroupParameter = NULL;
    medBoolParameter *parameter1 = NULL;
    medBoolParameter *parameter2 = NULL;
    medBoolParameter *parameter3 = NULL;

    boolGroupParameter = new medBoolGroupParameter(m_name);
    parameter1 = new medBoolParameter("1");
    parameter2 = new medBoolParameter("2");
    parameter3 = new medBoolParameter("3");

    boolGroupParameter->addParameter(parameter1);
    boolGroupParameter->addParameter(parameter2);
    boolGroupParameter->addParameter(parameter3);
    QCOMPARE(boolGroupParameter->getWidget()->layout()->count(), 3);
    boolGroupParameter->removeBoolParameter(parameter3);
    QCOMPARE(boolGroupParameter->getWidget()->layout()->count(), 2);

    boolGroupParameter->addParameter(parameter3);
    parameter1->setValue(true);
    QVERIFY(parameter1->value());
    QVERIFY(!parameter2->value());
    QVERIFY(!parameter3->value());
    parameter2->setValue(true);
    QVERIFY(!parameter1->value());
    QVERIFY(parameter2->value());
    QVERIFY(!parameter3->value());
    parameter3->setValue(true);
    QVERIFY(!parameter1->value());
    QVERIFY(!parameter2->value());
    QVERIFY(parameter3->value());
}
DTKTEST_MAIN(medBoolGroupParameterTest, medBoolGroupParameterTestObject)
