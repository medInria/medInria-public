/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medPluginGeneratorTest.h>

#include <QProcess>
#include <QDebug>

bool removeDir(const QString & dirName)
{
    bool result = false;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        foreach(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}


void medPluginGeneratorTest::initTestCase()
{
    QString subdir("medPluginGeneratorTmp");
    _tmpPath = QDir::tempPath();

    if (QFile::exists(_tmpPath.absoluteFilePath(subdir)))
    {
        if ( ! removeDir(_tmpPath.absoluteFilePath(subdir)))
            QFAIL("Can't setup the test directory");
    }

    if ( ! _tmpPath.mkpath(subdir) )
        QFAIL("Can't mkpath the test directory");

    if ( ! _tmpPath.cd(subdir) )
        QFAIL("Can't cd to the test directory");
}


void medPluginGeneratorTest::cleanupTestCase()
{
    if ( ! removeDir(_tmpPath.absolutePath()))
        QFAIL("Can't cleanup the test directory");
}


void medPluginGeneratorTest::testCompile_data()
{
    QTest::addColumn<QString>("type");
    QTest::addColumn<QString>("name");

    QTest::newRow("data") << "1" << "TestGenericData";

    QTest::newRow("view") << "3" << "TestGenericView";
    QTest::newRow("layeredView") << "4" << "TestGenericLayeredView";
    QTest::newRow("imageView") << "5" << "TestGenericImageView";

    QTest::newRow("viewInteractor") << "7" << "TestGenericViewInteractor";
    QTest::newRow("layeredViewInteractor") << "8" << "TestGenericLayeredViewInteractor";
    QTest::newRow("imageViewInteractor") << "9" << "TestGenericImageViewInteractor";
    QTest::newRow("extraInteractor") << "10" << "TestExtraInteractor";

    QTest::newRow("viewNavigator") << "12" << "TestGenericViewNavigator";
    QTest::newRow("layeredViewNavigator") << "13" << "TestGenericLayeredViewNavigator";
    QTest::newRow("imageViewNavigator") << "14" << "TestGenericImageViewNavigator";
    QTest::newRow("extraNavigator") << "15" << "TestExtraNavigator";

    QTest::newRow("filtering") << "17" << "TestFiltering";
    QTest::newRow("registration") << "18" << "TestRegistration";
    //QTest::newRow("diffusion") << "19" << "TestFiltering";

    QTest::newRow("datareader") << "21" << "TestReader";
    QTest::newRow("datawriter") << "22" << "TestWriter";

    QTest::newRow("workspace") << "24" << "TestWorkspace";
}


void medPluginGeneratorTest::testCompile()
{
    QFETCH(QString, type);
    QFETCH(QString, name);

#if defined(Q_OS_MAC)
    QString binaryPath = QDir(CMAKE_BINARY_DIR).absoluteFilePath("bin/medPluginGenerator.app/Contents/MacOS/medPluginGenerator");
#elif defined(Q_OS_WIN)
    QString binaryPath = QDir(qApp->applicationDirPath()).absoluteFilePath("medPluginGenerator.exe");
#else
    QString binaryPath = QDir(CMAKE_BINARY_DIR).absoluteFilePath("bin/medPluginGenerator");
#endif

    QVERIFY2(QFile::exists(binaryPath), "Cannot find the path to the medPluginGenerator binary !");

    // Generation step
    QStringList args;
    args << "--console" << "--output" << _tmpPath.absolutePath()
         << "--name" << name << "--type" << type;
    qDebug() << args;

    QProcess pGen;
    pGen.setProcessChannelMode(QProcess::MergedChannels);
    pGen.start(binaryPath, args);
    if (! pGen.waitForFinished(-1)) {
        pGen.kill();
        QFAIL("Generation step took too long, killed !");
    }

    if(pGen.exitCode() != 0) {
        qDebug() << pGen.readAllStandardOutput();
        QFAIL("Generation step failed !");
    }

    qDebug() << pGen.readAllStandardOutput();

    // Configuration step
    QString subdir("build-"+name);
    QDir buildDir(_tmpPath);
    buildDir.mkdir(subdir);
    buildDir.cd(subdir);

    QString nameFirstLower = name.replace(0, 1, name.left(1).toLower());

    args.clear();
    args << QString("-DMEDINRIA_DIR:PATH=%1").arg(CMAKE_BINARY_DIR)
         << QString("-Ddtk_DIR:PATH=%1").arg(CMAKE_DTK_DIR)
         << QString("-DITK_DIR:PATH=%1").arg(CMAKE_ITK_DIR)
         << QString("-DRPI_DIR:PATH=%1").arg(CMAKE_RPI_DIR)
         << _tmpPath.absoluteFilePath(nameFirstLower);
    pGen.setWorkingDirectory(buildDir.absolutePath());
    pGen.start("cmake", args);
    if (! pGen.waitForFinished(-1)) {
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
    if (! pGen.waitForFinished(-1)) {
        pGen.kill();
        QFAIL("Compilation step took too long, killed !");
    }

    if(pGen.exitCode() != 0) {
        qDebug() << pGen.readAllStandardOutput();
        QFAIL("Compilation step failed !");
    }
}

QTEST_MAIN(medPluginGeneratorTest)
