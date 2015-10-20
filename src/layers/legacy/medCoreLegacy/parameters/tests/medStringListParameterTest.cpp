/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medStringListParameterTest.h>

#include <stdio.h>

#include <QtGui>
#include <QStringList>
#include <QSignalSpy>

#include <medStringListParameter.h>



medStringListParameterTestObject::medStringListParameterTestObject(void)
{
    m_name = QString("testWithObjectName");
    m_widget = NULL;
    m_comboBox = NULL;
}

medStringListParameterTestObject::~medStringListParameterTestObject(void)
{
}

void medStringListParameterTestObject::initTestCase()
{
}

void medStringListParameterTestObject::init()
{
}

void medStringListParameterTestObject::cleanup()
{
}

void medStringListParameterTestObject::cleanupTestCase()
{
}


void medStringListParameterTestObject::testShowHideDelete()
{
    medStringListParameter* parameter = new medStringListParameter(m_name);
    m_widget = parameter->getWidget();
    m_comboBox = parameter->getComboBox();

    QSignalSpy spy1(m_comboBox, SIGNAL(destroyed()));
    QSignalSpy spy2(m_widget, SIGNAL(destroyed()));

    parameter->show();
    QVERIFY(m_widget->isVisible());
    QVERIFY(m_comboBox->isVisible());
    parameter->hide();
    QVERIFY(!m_widget->isVisible());
    QVERIFY(!m_comboBox->isVisible());

    delete parameter;
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);

    m_comboBox = NULL;
    m_widget = NULL;

}

void medStringListParameterTestObject::testInternalSate()
{
//    medStringListParameter* parameter = NULL;

//    parameter = new medStringListParameter(m_name);
//    m_comboBox = parameter->getComboBox();
//    QSignalSpy spy1(parameter, SIGNAL(valueChanged(QString)));
//    QStringList list;
//    list.append("1");
//    list.append("2");
//    list.append("3");

//    QStringList testString;

//    QCOMPARE(m_comboBox->itemText(0), testString);
//    parameter->addItems(list);
//    QCOMPARE(spy1.count(), 1);
//    QStringList toto = parameter->items();
//    QVERIFY(list == parameter->items());
//    QVERIFY(m_comboBox->itemText(0) == "1");
//    testString = parameter->value();
//    parameter->removeItem(testString);
//    QCOMPARE(spy1.count(), 2);
//    QCOMPARE(m_comboBox->count(), 2);
//    QVERIFY(m_comboBox->count() == parameter->items().size());

//    parameter->setValue(testString);
//    QCOMPARE(spy1.count(), 3);
//    QCOMPARE(m_comboBox->count(), 3);
//    QVERIFY(m_comboBox->count() == parameter->items().size());
//    QVERIFY(m_comboBox->itemText(2) == "1");
//    testString = "4";

//    parameter->addItem(testString);
//    QCOMPARE(m_comboBox->count(), 4);
//    QVERIFY(m_comboBox->count() == parameter->items().size());
//    QCOMPARE(m_comboBox->currentIndex(), 2);
//    QVERIFY(parameter->value() == "1");

//    m_comboBox->setCurrentIndex(3);
//    QCOMPARE(spy1.count(), 4);
//    QVERIFY(parameter->value() == "4");

//    parameter->clear();
//    QCOMPARE(spy1.count(), 5);
//    QCOMPARE(m_comboBox->count(), 0);

//    QMetaObject::invokeMethod(m_comboBox, "currentIndexChanged", Qt::DirectConnection,
//                              Q_ARG(QString, testString));
//    QCOMPARE(m_comboBox->count(), 1);
//    QCOMPARE(m_comboBox->itemText(0), testString);

//    delete parameter;
//    m_comboBox = NULL;

}


void medStringListParameterTestObject::testSignalsAndAttachedData()
{
    medStringListParameter* parameter = NULL;

    parameter = new medStringListParameter(m_name);
    m_comboBox = parameter->getComboBox();
    QSignalSpy spy1(parameter, SIGNAL(valueChanged(QString)));
    QMetaObject::invokeMethod(m_comboBox, "currentIndexChanged", Qt::DirectConnection,
                              Q_ARG(QString, "prout"));
    QCOMPARE(spy1.count(), 1);


    delete parameter;
    m_comboBox = NULL;

}
DTKTEST_MAIN(medStringListParameterTest, medStringListParameterTestObject)
