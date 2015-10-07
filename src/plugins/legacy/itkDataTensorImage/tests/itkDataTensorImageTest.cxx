/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtTest/QtTest>

// file configured by cmake
// with required paths
#include "itkDataTensorImageTestConfig.h"

#include <dtkCore/dtkPluginManager.h>
#include <dtkCore/dtkSmartPointer.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include <medDbControllerFactory.h>
#include <medDatabaseNonPersistentController.h>
#include <medDataManager.h>
#include <medDataIndex.h>
#include <medDatabaseNonPersistentReader.h>
#include <medAbstractDataImage.h>

#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkTensor.h"

class itkDataTensorImageTest: public QObject
{
    Q_OBJECT

public:
    itkDataTensorImageTest();
    ~itkDataTensorImageTest();

private:
    QString dataPath;
    medDataIndex index;

    template <class ScalarType>
    int checkTensorPositivity ( itk::Image< itk::Tensor<ScalarType, 3>, 3> *dataPath);

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
    // in this case "run_data()" will prepare some data
    // to run "run()" different times
    // with different input values
    void run_data();
    void run();
};

itkDataTensorImageTest::itkDataTensorImageTest()
{
    medDbControllerFactory::instance()
            ->registerDbController("NonPersistentDbController", createNonPersistentDbController);
}

itkDataTensorImageTest::~itkDataTensorImageTest()
{
}

void itkDataTensorImageTest::initTestCase()
{
    QString pluginsPath = MEDINRIA_PLUGINS_PATH;

    this->dataPath = MEDINRIA_TEST_DATA_ROOT;
    QDir dataDir(this->dataPath);
    if (!dataDir.exists())
        QFAIL("Medinria data directory is required to run this test.");

     // load medInria plugins, we will need data types, readers and writers
     dtkPluginManager::instance()->setPath(pluginsPath);
     dtkPluginManager::instance()->initialize();
}

void itkDataTensorImageTest::init()
{
    QFETCH(QString, filePath);

    medDatabaseNonPersistentReader *reader =
            new medDatabaseNonPersistentReader(this->dataPath + QDir::separator() + filePath);
    reader->run();

    medDataIndex index = reader->index();
    if (!index.isValid())
        QFAIL("Reading fail (invalid medDataIndex)");

    this->index = index;

    delete reader;
}

void itkDataTensorImageTest::cleanup()
{
    medDataManager::instance()->removeData(this->index);
}

void itkDataTensorImageTest::cleanupTestCase()
{
    dtkPluginManager::instance()->uninitialize();
}

void itkDataTensorImageTest::run_data()
{
    QTest::addColumn<QString>("filePath");

    QTest::newRow("xDTICUBE-4D-Tensors.nii.gz") << QString("xDTICUBE-4D") + QDir::separator() + QString("xDTICUBE-4D-tensors.nii.gz");
}

void itkDataTensorImageTest::run()
{
    dtkSmartPointer<dtkAbstractData> data = medDataManager::instance()->data(this->index);

    if (data.isNull())
        QFAIL("Access to data via data manager failed (null pointer)");

    if (data->identifier()!="itkDataTensorImageDouble3" &&
            data->identifier()!="itkDataTensorImageFloat3") {
        QFAIL("Data object is not of type itkDataTensorImageDouble/Float3");
    }

    if (data->identifier()=="itkDataTensorImageDouble3") {
        itk::Image<itk::Tensor<double, 3>, 3> * tensors = static_cast<itk::Image<itk::Tensor<double, 3>, 3> *> (data->data());
        QVERIFY(checkTensorPositivity(tensors)==0);
    }
    else if (data->identifier()=="itkDataTensorImageFloat3") {
        itk::Image<itk::Tensor<float, 3>, 3> * tensors = static_cast<itk::Image<itk::Tensor<float, 3>, 3> *> (data->data());
        QVERIFY(checkTensorPositivity(tensors)==0);
    }
}

template <class ScalarType>
int itkDataTensorImageTest::checkTensorPositivity(itk::Image<itk::Tensor<ScalarType, 3>, 3> *tensors)
{
    int value = -1;

    if (!tensors)
        return value;

    typedef itk::Image<itk::Tensor<ScalarType, 3>, 3> TensorImageType;

    typedef itk::ImageRegionIteratorWithIndex<TensorImageType> IteratorType;

    IteratorType it (tensors, tensors->GetLargestPossibleRegion());

    while (!it.IsAtEnd()) {
        if (!it.Value().IsZero()) {
            if (!it.Value().IsPositive())
                return -1;
        }

        ++it;
    }

    return 0;
}
//we don't need graphical ties, no X needed
QTEST_APPLESS_MAIN(itkDataTensorImageTest)
//QTEST_MAIN(itkDataTensorImageTest)
#include "moc_itkDataTensorImageTest.cxx"

