#pragma once

#include <QObject>
#include <QtTest/QtTest>
#include <QDir>

class medPluginGeneratorTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void testCompile_data();
    void testCompile();

private:
    QDir _tmpPath;
};
