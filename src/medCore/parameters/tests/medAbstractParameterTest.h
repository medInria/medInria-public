/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkTest.h>
#include <QtGui>

class medAbstractParameterTestObject : public QObject
{
    Q_OBJECT

public:
             medAbstractParameterTestObject();
    virtual ~medAbstractParameterTestObject();


private:
     QString m_name;
     QString m_toolTip;
     QLabel* m_label;
     QWidget* m_widget;


private slots:
    /*
     * initTestCase() is called before the
     * execution of all the test cases.
     * If it fails, no test is executed.
     */
    void initTestCase();

    /*
     * init() is called before each test.
     * If it fails, the following
     * test is not executed.
     */
    void init();

    /*
     * cleanup() is called after each test.
     */
    void cleanup();

    /*
     * cleanupTestCase() is called
     * after all test have been executed.
     */
    void cleanupTestCase();

private slots:

    // every function here is a test, unless they end with "_data"
    // in this case "testFoo_data()" will prepare some data
    // to run "testFoo()" different times
    // with different input values
    // void testFoo_data();
    void testShowHideDelete();
    void testInternalSate();
};


