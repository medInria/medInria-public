#pragma once

#include <QObject>
#include <QtTest/QtTest>

class medQSSParserTest : public QObject
{
    Q_OBJECT
private slots:
    void testParse_data();
    void testParse();
};

