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
#include <medAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

class medViewContainerSplitterTestObject : public QObject
{
    Q_OBJECT

public:
    medViewContainerSplitterTestObject();
    virtual ~medViewContainerSplitterTestObject();

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

    void testAddViewContainer();
    void testInsertViewContainer();
    void testSplitVertically();
    void testSplitHorizontally();
    void testSplit();

};


