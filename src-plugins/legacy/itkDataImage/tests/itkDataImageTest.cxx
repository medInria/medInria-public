/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractImageData.h>

#include "dtkCore/dtkPluginManager.h"
#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include "dtkCore/dtkAbstractData.h"
#include <dtkLog/dtkLog.h>
#include <dtkCore/dtkSmartPointer.h>

#include <itkImage.h>
#include <itkRGBAPixel.h>
#include <itkRGBPixel.h>

#include <exception>
#include <string>

typedef itk::RGBAPixel<unsigned char>  RGBAPixelType;
typedef itk::RGBPixel<unsigned char>  RGBPixelType;
typedef itk::Vector<unsigned char, 3> UCharVectorType;

/**
 * Base class for templated test functions
 * Provides a uniform interface for templated functions, see below.
 */
struct testRunBase {
    explicit testRunBase( const QString & dataTypeName) : m_dataTypeName(dataTypeName) {}
    virtual ~testRunBase() {}
    virtual int run() = 0;
    QString m_dataTypeName;
};


/**
 * Implement test for a concrete pixel type and image dimension.
 * */
template< typename TPixel , unsigned int VDimension > struct testRunner : public testRunBase {
    typedef TPixel PixelType;
    enum { ImageDimension = VDimension };

    testRunner( const QString & dataTypeName ) : testRunBase(dataTypeName) {}
    int run();
};

template< typename TPixel , unsigned int VDimension >
int testRunner<TPixel,VDimension>::run() {

    typedef typename itk::Image<PixelType, VDimension> ImageType;

    // create an ITK image and feed a data plugin with it
    typename ImageType::Pointer image(ImageType::New());
    typename ImageType::RegionType region;
    typename ImageType::SizeType   imageSize;
    for (unsigned i=0; i<VDimension; ++i) {
        imageSize[i] = 10 + i;
    }

    typename ImageType::IndexType  index;
    index.Fill(0);

    region.SetSize (imageSize);
    region.SetIndex (index);
    image->SetRegions (region);
    image->Allocate();

    dtkSmartPointer<medAbstractImageData> dataInDtk = medAbstractDataFactory::instance()->createSmartPointer(this->m_dataTypeName);
    if (!dataInDtk) {
        dtkDebug() << "Cannot create data object from plugin";
        return EXIT_FAILURE;
    }

    dataInDtk->setData ( image.GetPointer() );

    typename ImageType::Pointer image2 = dynamic_cast<ImageType*>( (itk::Object*)(dataInDtk->data()) );
    if (image2.IsNull()) {
        dtkDebug() << "Cannot cast data() to ITK image";
        return EXIT_FAILURE;
    }

    if ((int)imageSize[0]!=dataInDtk->xDimension()) {
        dtkDebug() << "Bad X dimension";
        return EXIT_FAILURE;
    }
    if ((int)imageSize[1]!=dataInDtk->yDimension()) {
        dtkDebug() << "Bad Y dimension";
        return EXIT_FAILURE;
    }
    if (VDimension > 2 && (int)imageSize[2]!=dataInDtk->zDimension()) {
        dtkDebug() << "Bad Z dimension";
        return EXIT_FAILURE;
    }

    if (VDimension > 3 && (int)imageSize[3]!=dataInDtk->tDimension()) {
        dtkDebug() << "Bad T dimension";
        return EXIT_FAILURE;
    }

    if ( dataInDtk->identifier() != this->m_dataTypeName ) {
        dtkDebug() << "Bad data identifier";
        return EXIT_FAILURE;
    }

    dataInDtk->release();
    return EXIT_SUCCESS;
}

/**
 * Basic harness to run the tests.
 * */
int itkDataImageTest (int argc, char* argv[])
{
    QCoreApplication testApp( argc, argv );

    // Test container
    QVector<testRunBase *> testsForEachType;
    int ret = EXIT_FAILURE;

    try {
        if (argc<2) {
            throw std::runtime_error( "Please specify plugin path as an argument" );
        }

        dtkPluginManager::instance()->setPath (argv[1]);
        dtkPluginManager::instance()->initialize();

        // Construct and fill test container with instances of the test runner.
        testsForEachType.push_back( new testRunner< RGBAPixelType, 3>("itkDataImageRGBA3") );
        testsForEachType.push_back( new testRunner< RGBPixelType, 3>("itkDataImageRGB3") );
        testsForEachType.push_back( new testRunner< UCharVectorType, 3>("itkDataImageVectorUChar3") );
        testsForEachType.push_back( new testRunner< char, 3>("itkDataImageChar3") );
        testsForEachType.push_back( new testRunner< char, 4>("itkDataImageChar4") );
        testsForEachType.push_back( new testRunner< double, 3>("itkDataImageDouble3") );
        testsForEachType.push_back( new testRunner< double, 4>("itkDataImageDouble4") );
        testsForEachType.push_back( new testRunner< float, 3>("itkDataImageFloat3") );
        testsForEachType.push_back( new testRunner< float, 4>("itkDataImageFloat4") );
        testsForEachType.push_back( new testRunner< int, 3>("itkDataImageInt3") );
        testsForEachType.push_back( new testRunner< int, 4>("itkDataImageInt4") );
        testsForEachType.push_back( new testRunner< long, 3>("itkDataImageLong3") );
        testsForEachType.push_back( new testRunner< long, 4>("itkDataImageLong4") );
        testsForEachType.push_back( new testRunner< short, 3>("itkDataImageShort3") );
        testsForEachType.push_back( new testRunner< short, 4>("itkDataImageShort4") );
        testsForEachType.push_back( new testRunner< unsigned char, 3>("itkDataImageUChar3") );
        testsForEachType.push_back( new testRunner< unsigned char, 4>("itkDataImageUChar4") );
        testsForEachType.push_back( new testRunner< unsigned int, 3>("itkDataImageUInt3") );
        testsForEachType.push_back( new testRunner< unsigned int, 4>("itkDataImageUInt4") );
        testsForEachType.push_back( new testRunner< unsigned long, 3>("itkDataImageULong3") );
        testsForEachType.push_back( new testRunner< unsigned long, 4>("itkDataImageULong4") );
        testsForEachType.push_back( new testRunner< unsigned short, 3>("itkDataImageUShort3") );
        testsForEachType.push_back( new testRunner< unsigned short, 4>("itkDataImageUShort4") );

        foreach( testRunBase * test, testsForEachType ) {

            dtkDebug() << " Testing " << test->m_dataTypeName;
            int testRet = test->run();

            if ( testRet != EXIT_SUCCESS ) {

                std::string msg("Failed while running test for ");
                msg += test->m_dataTypeName.toStdString();
                throw std::runtime_error( msg );
            }
        }
        ret = EXIT_SUCCESS;
    }
    catch ( std::runtime_error & e) {
        dtkError() << e.what();
    }
    catch ( ... ) {
        dtkError() << "An unknown exception was caught";
    }

    // Cleanup.
    qDeleteAll( testsForEachType );

    if ( ret == EXIT_SUCCESS ) {
        dtkDebug() << "itkDataImageTest succeeded";
    }
    return ret;
}
