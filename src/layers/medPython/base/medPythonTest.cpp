/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreAPI.h"

#include "medPythonTest.h"

#include <QDataStream>
#include <QDebug>

#include "medExternalResources.h"

#include "medPython.h"
#include "medPythonCoreInit.h"

namespace med::python::test
{

namespace
{

void print(QString message)
{
    qInfo() << message;
}

void printIndented(QString message)
{
    print(QString("      ") + message);
}

void printEmptyLine()
{
    print("----");
}

bool isEmbeddedPath(QString path)
{
    static QStringList embeddedPathRoots = getTemporaryDirectories() << med::getExternalResourcesDirectory(TARGET_NAME);

    foreach (QString pathRoot, embeddedPathRoots)
    {
        if (path.startsWith(pathRoot))
        {
            return true;
        }
    }

    return false;
}

void testLazyLoading()
{
    print("Testing lazy loading of Python by creating an object");
    Object o = 1;
}

void testConversion()
{
    print("Testing conversion of QList<double> to and from Python");
    QList<double> qList1 = {1.5, 2.7, 3.4};
    Object pythonList = list(qList1);
    QList<double> qList2 = pythonList.convert<QList<double> >();

    if (qList1 != qList2)
    {
        throw BaseException("Conversion of QList<double> failed");
    }
}

void testFunctionCall()
{
    print("Testing function call");
    Object foo = "foobarbaz";
    Object index = foo.callMethod("find", Object("bar"));

    if (index.convert<long>() != 3)
    {
        throw BaseException("Function call failed");
    }
}

void testContainerAccess()
{
    print("Testing container access");
    Object listObject = list();
    listObject.append(Object(42));
    listObject.append(Object("foo"));
    listObject.append(Object(5.7));
    Object foo = listObject[1];

    if (foo.convert<QString>() != "foo")
    {
        throw BaseException("Container access failed");
    }
}

void testModuleImport(QString name)
{
    print(QString("Testing the import of the %1 module").arg(name));
    Module module = import(name);
}

void testSysPath()
{
    print("Testing sys.path");
    QStringList sysPath = import("sys").attribute("path").convert<QStringList>();

    foreach (QString path, sysPath)
    {
        printIndented(path);

        if (!isEmbeddedPath(path))
        {
            QString message = QString("Path is not from the embedded Python: %1").arg(path);
            throw BaseException(message);
        }
    }

    print("All paths in sys.path are from the embedded Python");
}

void testSysModules()
{
    print("Testing sys.modules");
    QHash<QString, Object> sysModules = import("sys").attribute("modules").convert<QHash<QString, Object> >();

    foreach (Object moduleObject, sysModules)
    {
        // Strangely, sys.modules can contain non-modules.
        //
        if (PyModule_Check(*moduleObject))
        {
            Module module = moduleObject;

            if (module.hasAttribute("__file__"))
            {
                QString file = module.attribute("__file__").convert<QString>();

                if (isEmbeddedPath(file))
                {
                    printIndented(module.name() + ": " + file);
                }
                else
                {
                    QString message = QString("Module %1 was not loaded from the embedded Python: %2").arg(module.name(), file);
                    throw BaseException(message);
                }
            }
            else
            {
                printIndented(module.name() + " (built-in)");
            }
        }
    }

    print("All modules were loaded from the embedded Python");
}

void testExceptionWrappers()
{
    print("Testing the exception wrappers");
    QString message = "exception test";
    printIndented(QString("Throwing an exception of type MemoryError with the message \"%1\"").arg(message));

    try
    {
        throw MemoryError(message);
    }
    catch (MemoryError& e)
    {
        printIndented(QString("Caught exception of type MemoryError with the message: ") + e.what());
        return;
    }

    throw BaseException("Error: no exception was thrown");
}

void testNativeErrorPropagation()
{
    print("Testing the propagation of native errors");
    printIndented(QString("Causing an error by trying to access attribute foo of object None"));

    try
    {
        PyObject_GetAttrString(Py_None, "foo");
        propagateErrorIfOccurred();
    }
    catch (AttributeError& e)
    {
        printIndented(QString("Caught exception of type AttributeError with the message: ") + e.what());
        return;
    }

    throw BaseException("Error: no exception was thrown");
}

void runEmbeddedPythonTest()
{
    testLazyLoading();
    printEmptyLine();
    testConversion();
    printEmptyLine();
    testFunctionCall();
    printEmptyLine();
    testContainerAccess();
    printEmptyLine();
    testModuleImport("sys");
    printEmptyLine();
    testModuleImport("json");
    printEmptyLine();
    //testSysPath();
    printEmptyLine();
    testSysModules();
    printEmptyLine();
    testExceptionWrappers();
    printEmptyLine();
    testNativeErrorPropagation();
}

} // namespace

int testEmbeddedPython(bool withCrash)
{
    printEmptyLine();
    print("---- START OF MEDPYTHON TEST ----");
    printEmptyLine();

    if (withCrash)
    {
        runEmbeddedPythonTest();
    }
    else
    {
        try
        {
            runEmbeddedPythonTest();
        }
        catch (BaseException& e)
        {
            print("An error was encountered while testing medPython:");
            print(e.what());
        }
    }

    printEmptyLine();
    print("---- END OF MEDPYTHON TEST ----");
    printEmptyLine();

    return 0;
}

} // namespace med::python::test
