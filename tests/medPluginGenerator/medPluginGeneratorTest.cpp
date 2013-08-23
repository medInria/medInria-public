#include "medPluginGeneratorTest.h"

#include <QProcess>
#include <QDebug>

void medPluginGeneratorTest::testCompile_data()
{
    QTest::addColumn<QString>("type");
    QTest::addColumn<QString>("name");

    QTest::newRow("data") << "Generic Data" << "TestGenericData";
    QTest::newRow("data") << "Generic Process" << "TestGenericProcess";
    QTest::newRow("data") << "Generic View" << "TestGenericView";
    QTest::newRow("data") << "Registration" << "TestRegistration";
    QTest::newRow("data") << "Filtering" << "TestFiltering";
}


void medPluginGeneratorTest::testCompile()
{
    QFETCH(QString, type);
    QFETCH(QString, name);

    if (qApp->argc() != 2)
        QFAIL("This tests to be provided with the generation path as first argument");

    QString binaryPath = qApp->arguments().at(1);
    QDir tmpPath(qApp->arguments().at(2));

    QVERIFY2(QFile::exists(binaryPath), "Incorrect path to the medPluginGenerator binary !");

    // Generation step
    QStringList args;
    args << "--console" << "--output" << tmpPath.absolutePath() << "--name" << name << "--type" << type;

    QProcess pGen;
    pGen.setProcessChannelMode(QProcess::MergedChannels);
    pGen.start(binaryPath, args);
    if (! pGen.waitForFinished()) {
        pGen.kill();
        QFAIL("Generation step took too long, killed !");
    }

    if(pGen.exitCode() != 0) {
        qDebug() << pGen.readAllStandardOutput();
        QFAIL("Generation step failed !");
    }

    // Configuration step
    QDir buildDir(tmpPath);
    buildDir.mkdir("build");
    buildDir.cd("build");

    args.clear();
    args << tmpPath.absoluteFilePath(name);
    pGen.setWorkingDirectory(buildDir.absolutePath());
    pGen.start("cmake", args);
    if (! pGen.waitForFinished()) {
        pGen.kill();
        QFAIL("Configuration step took too long, killed !");
    }

    if(pGen.exitCode() != 0) {
        qDebug() << pGen.readAllStandardOutput();
        QFAIL("Configuration step failed !");
    }

    // Compilation step
    args.clear();
    args << "--build" << ".";
    pGen.setWorkingDirectory(buildDir.absolutePath());
    pGen.start("cmake", args);
    if (! pGen.waitForFinished()) {
        pGen.kill();
        QFAIL("Compilation step took too long, killed !");
    }

    if(pGen.exitCode() != 0) {
        qDebug() << pGen.readAllStandardOutput();
        QFAIL("Compilation step failed !");
    }
}

QTEST_MAIN(medPluginGeneratorTest)
