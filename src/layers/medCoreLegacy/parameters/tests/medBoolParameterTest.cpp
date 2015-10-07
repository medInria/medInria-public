/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medBoolParameterTest.h>


#include <QtGui>
#include <QSignalSpy>

#include <medBoolParameter.h>



medBoolParameterTestObject::medBoolParameterTestObject(void)
{
    m_name = QString("testWithObjectName");
    m_text = QString("testWithObjectToolText");
    m_icon = QIcon(":/pixmaps/plus_button.png");
    m_iconSize = QSize(42,42);
    m_pushButton = NULL;
    m_widget = NULL;
    m_checkBox = NULL;
    m_radioButton = NULL;
}

medBoolParameterTestObject::~medBoolParameterTestObject(void)
{
}

void medBoolParameterTestObject::initTestCase()
{
}

void medBoolParameterTestObject::init()
{
}

void medBoolParameterTestObject::cleanup()
{
}

void medBoolParameterTestObject::cleanupTestCase()
{
}


void medBoolParameterTestObject::testShowHideDelete()
{
    medBoolParameter* boolParameter = new medBoolParameter(m_name);
    m_widget = boolParameter->getWidget();
    m_pushButton = boolParameter->getPushButton();
    m_checkBox = boolParameter->getCheckBox();
    m_radioButton = boolParameter->getRadioButton();
    
    QSignalSpy spy1(m_pushButton, SIGNAL(destroyed()));
    QSignalSpy spy2(m_checkBox, SIGNAL(destroyed()));
    QSignalSpy spy3(m_radioButton, SIGNAL(destroyed()));
    QSignalSpy spy4(m_widget, SIGNAL(destroyed()));
    
    boolParameter->show();
    QVERIFY(m_widget->isVisible());
    QVERIFY(m_pushButton->isVisible());
    QVERIFY(m_checkBox->isVisible());
    QVERIFY(m_radioButton->isVisible());
    boolParameter->hide();
    QVERIFY(!m_widget->isVisible());
    QVERIFY(!m_pushButton->isVisible());
    QVERIFY(!m_checkBox->isVisible());
    QVERIFY(!m_radioButton->isVisible());

    delete boolParameter;
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(spy3.count(), 1);
    QCOMPARE(spy4.count(), 1);
    
    m_pushButton = NULL;
    m_widget = NULL;
    m_radioButton = NULL;
    m_checkBox = NULL;
}

void medBoolParameterTestObject::testInternalSate()
{
    medBoolParameter* boolParameter = NULL;

    boolParameter = new medBoolParameter(m_name);
    m_pushButton = boolParameter->getPushButton();
    boolParameter->setText(m_text);
    boolParameter->setIcon(m_icon);
    boolParameter->setIconSize(m_iconSize);
    QVERIFY(m_pushButton->text() == m_text);
    QCOMPARE(m_pushButton->icon(),  m_icon);
    QVERIFY(m_pushButton->minimumSize() == m_iconSize);
    QVERIFY(m_pushButton->iconSize() == m_iconSize);
    QVERIFY(m_pushButton->isCheckable());

    delete boolParameter;
    boolParameter = new medBoolParameter(m_name);
    boolParameter->setText(m_text);
    boolParameter->setIcon(m_icon);
    boolParameter->setIconSize(m_iconSize);
    m_pushButton = boolParameter->getPushButton();
    QVERIFY(m_pushButton->text() == m_text);
    QCOMPARE(m_pushButton->icon(),  m_icon);
    QVERIFY(m_pushButton->minimumSize() == m_iconSize);
    QVERIFY(m_pushButton->iconSize() == m_iconSize);
    QVERIFY(m_pushButton->isCheckable());

    m_radioButton = boolParameter->getRadioButton();
    m_checkBox = boolParameter->getCheckBox();

    boolParameter->setValue(false);
    QVERIFY(!m_pushButton->isChecked());
    QVERIFY(!m_radioButton->isChecked());
    QVERIFY(!m_checkBox->isChecked());

    QSignalSpy spy1(m_pushButton, SIGNAL(toggled(bool)));
    QSignalSpy spy2(m_checkBox, SIGNAL(toggled(bool)));
    QSignalSpy spy3(m_radioButton, SIGNAL(toggled(bool)));

    m_radioButton->setChecked(true);
    QVERIFY(m_pushButton->isChecked());
    QVERIFY(m_radioButton->isChecked());
    QVERIFY(m_checkBox->isChecked());

    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 1);

    m_checkBox->setChecked(false);
    QVERIFY(!m_pushButton->isChecked());
    QVERIFY(!m_radioButton->isChecked());
    QVERIFY(!m_checkBox->isChecked());
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(spy3.count(), 1);

    m_pushButton->setChecked(true);
    QVERIFY(m_pushButton->isChecked());
    QVERIFY(m_radioButton->isChecked());
    QVERIFY(m_checkBox->isChecked());
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(spy3.count(), 1);

    delete boolParameter;
    m_pushButton = NULL;
    m_radioButton = NULL;
    m_checkBox = NULL;
}


void medBoolParameterTestObject::testSignalsAndAttachedData()
{
    medBoolParameter* boolParameter = NULL;
    boolParameter = new medBoolParameter(m_name);
    m_pushButton = boolParameter->getPushButton();
    m_radioButton = boolParameter->getRadioButton();
    m_checkBox = boolParameter->getCheckBox();
    QSignalSpy spy1(boolParameter, SIGNAL(valueChanged(bool)));
    QMetaObject::invokeMethod(m_pushButton, "toggle", Qt::DirectConnection);
    QMetaObject::invokeMethod(m_radioButton, "toggle", Qt::DirectConnection);
    QMetaObject::invokeMethod(m_checkBox, "toggle", Qt::DirectConnection);
    QCOMPARE(spy1.count(), 3);

    delete boolParameter;
    m_pushButton = NULL;
    m_checkBox = NULL;
    m_radioButton = NULL;
}
DTKTEST_MAIN(medBoolParameterTest, medBoolParameterTestObject)
