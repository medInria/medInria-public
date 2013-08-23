#pragma once

#include <QObject>
#include <QtTest/QtTest>

class medPluginGeneratorTest : public QObject
{
    Q_OBJECT
private slots:
    void testCompile_data();
    void testCompile();
};
