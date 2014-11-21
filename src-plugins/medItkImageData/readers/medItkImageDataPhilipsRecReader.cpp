/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageDataPhilipsRecReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkPhilipsRECImageIO.h>
#include <itkPhilipsPAR.h>
#include <itkMetaDataObject.h>

const char medItkImageDataPhilipsRecReader::ID[] = "medItkImageDataPhilipsRecReader";

medItkImageDataPhilipsRecReader::medItkImageDataPhilipsRecReader(): medItkImageDataReaderBase() {
    this->io = itk::PhilipsRECImageIO::New();
}

medItkImageDataPhilipsRecReader::~medItkImageDataPhilipsRecReader() { }

QStringList medItkImageDataPhilipsRecReader::handled() const {
    return s_handled();
}

QStringList medItkImageDataPhilipsRecReader::s_handled() {
    return QStringList() << "medItkChar3ImageData" << "medItkChar4ImageData" << "medItkUChar3ImageData" << "medItkUChar4ImageData" << "medItkShort3ImageData" << "medItkShort4ImageData" << "medItkUShort3ImageData" << "medItkUShort4ImageData" << "medItkInt3ImageData" << "medItkInt4ImageData" << "medItkUInt3ImageData" << "medItkUInt4ImageData" << "medItkLong3ImageData" << "medItkLong4ImageData" << "medItkULong3ImageData" << "medItkULong4ImageData" << "medItkFloat3ImageData" << "medItkFloat4ImageData" << "medItkDouble3ImageData" << "medItkDouble4ImageData";
}

bool medItkImageDataPhilipsRecReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),create);
}

QString medItkImageDataPhilipsRecReader::identifier() const {
    return ID;
}

QString medItkImageDataPhilipsRecReader::description() const {
    return "Reader for Philips REC images";
}

bool medItkImageDataPhilipsRecReader::read(const QString& path)
{
    if (!medItkImageDataReaderBase::read(path))
    {
        return false;
    }
    //Find the correct origin and orientation:
    //the itk implementation is flawed
    FloatImageType::DirectionType correctdirection;
    FloatImageType::PointType correctorigin;
    correctdirection = this->ExtractPARRECImageOrientation();
    correctorigin = this->ExtractPARRECImageOrigin (correctdirection);

    //apply the corrections to the image
    QString type = data()->identifier();
    if (type == "medItkUChar3ImageData")
    {
        applyCorrection<3,unsigned char>(correctorigin,
                                          correctdirection);
    }
    else if (type == "medItkUChar4ImageData")
    {
        applyCorrection<4,unsigned char>(correctorigin,
                                          correctdirection);
    }
    else if (type == "medItkUShort3ImageData")
    {
        applyCorrection<3,unsigned short>(correctorigin,
                                           correctdirection);
    }
    else if (type == "medItkUShort4ImageData")
    {
        applyCorrection<4,unsigned short>(correctorigin,
                                           correctdirection);
    }
    else if (type == "medItkUInt3ImageData")
    {
        applyCorrection<3,unsigned int>(correctorigin,
                                         correctdirection);
    }
    else if (type == "medItkUInt4ImageData")
    {
        applyCorrection<4,unsigned int>(correctorigin,
                                         correctdirection);
    }
    else if (type == "medItkULong3ImageData")
    {
        applyCorrection<3,unsigned long>(correctorigin,
                                          correctdirection);
    }
    else if (type == "medItkULong4ImageData")
    {
        applyCorrection<4,unsigned long>(correctorigin,
                                          correctdirection);
    }
    if (type == "medItkChar3ImageData")
    {
        applyCorrection<3, char>(correctorigin,
                                  correctdirection);
    }
    else if (type == "medItkChar4ImageData")
    {
        applyCorrection<4, char>(correctorigin,
                                  correctdirection);
    }
    else if (type == "medItkShort3ImageData")
    {
        applyCorrection<3, short>(correctorigin,
                                   correctdirection);
    }
    else if (type == "medItkShort4ImageData")
    {
        applyCorrection<4, short>(correctorigin,
                                   correctdirection);
    }
    else if (type == "medItkInt3ImageData")
    {
        applyCorrection<3, int>(correctorigin,
                                 correctdirection);
    }
    else if (type == "medItkInt4ImageData")
    {
        applyCorrection<4, int>(correctorigin,
                                 correctdirection);
    }
    else if (type == "medItkLong3ImageData")
    {
        applyCorrection<3, long>(correctorigin,
                                  correctdirection);
    }
    else if (type == "medItkLong4ImageData")
    {
        applyCorrection<4, long>(correctorigin,
                                  correctdirection);
    }
    else if (type == "medItkFloat3ImageData")
    {
        applyCorrection<3, float>(correctorigin,
                                   correctdirection);
    }
    else if (type == "medItkFloat4ImageData")
    {
        applyCorrection<4, float>(correctorigin,
                                   correctdirection);
    }
    else if (type == "medItkDouble3ImageData")
    {
        applyCorrection<3, double>(correctorigin,
                                    correctdirection);
    }
    else if (type == "medItkDouble4ImageData")
    {
        applyCorrection<4, double>(correctorigin,
                                    correctdirection);
    }
    else
    {
        qWarning() << "Unrecognized type for Philips Rec: "
                      << type;
        return false;
    }
    return true;
}

template <unsigned dim,typename T> void
medItkImageDataPhilipsRecReader::applyCorrection(FloatImageType::PointType correctorigin,
                                              FloatImageType::DirectionType correctdirection)
{
    // To take care of dimensions 3 and 4,
    // we need new direction and point types.
    typedef itk::Image<T,dim> ImageType;
    typedef typename ImageType::DirectionType correctDirectionType;
    typedef typename ImageType::PointType correctPointType;


    correctDirectionType direction;
    direction.SetIdentity();
    for (unsigned int i=0; i<3; i++)
        for (unsigned int j=0; j<3; j++)
            direction[i][j] = correctdirection[i][j];
    correctPointType origin;
    origin.Fill(0.0);
    for (unsigned int i=0; i<3; i++)
        origin[i] = correctorigin[i];

    ImageType* img = (ImageType *) (data()->data());
    img->SetDirection(direction);
    img->SetOrigin(origin);
}

medItkImageDataPhilipsRecReader::FloatImageType::PointType medItkImageDataPhilipsRecReader::ExtractPARRECImageOrigin (
        FloatImageType::DirectionType direction)
{
    typedef FloatImageType::PointType PointType;
    PointType nullorigin;
    nullorigin[0] = nullorigin[1] = nullorigin[2] = 0.0;

    if (!this->io->GetFileName())
    {
        qWarning() << "io doesn't have a filename, call readInformation first";
        return nullorigin;
    }

    itk::MetaDataDictionary PARheader = this->io->GetMetaDataDictionary();

    typedef itk::PhilipsRECImageIO::OffCentreMidSliceType OffCentreType;

    OffCentreType offcenter;

    bool valid = itk::ExposeMetaData<OffCentreType>(PARheader, "PAR_OffCentreMidSlice", offcenter);
    if (!valid)
    {
        qWarning() << "cannot find off-center information in PAR header, no correction";
        return nullorigin;
    }

    double dimensions[3];
    dimensions[0] = this->io->GetDimensions (0);
    dimensions[1] = this->io->GetDimensions (1);
    dimensions[2] = this->io->GetDimensions (2);

    FloatImageType::SpacingType midoffset;
    midoffset[0] = this->io->GetSpacing (0) * (dimensions[0] - 1) / 2.0;
    midoffset[1] = this->io->GetSpacing (1) * (dimensions[1] - 1) / 2.0;
    midoffset[2] = this->io->GetSpacing (2) * (dimensions[2] - 1) / 2.0;
    midoffset = direction * midoffset;

    PointType offcenterpoint;
    offcenterpoint[0] = offcenter[0];
    offcenterpoint[1] = offcenter[1];
    offcenterpoint[2] = offcenter[2];

    FloatImageType::DirectionType AFRtoLPS;
    AFRtoLPS.Fill (0);
    AFRtoLPS[0][2] = 1;
    AFRtoLPS[1][0] = 1;
    AFRtoLPS[2][1] = 1;

    offcenterpoint = AFRtoLPS * offcenterpoint;
    offcenterpoint -= midoffset;

    return offcenterpoint;
}


medItkImageDataPhilipsRecReader::FloatImageType::DirectionType
medItkImageDataPhilipsRecReader::ExtractPARRECImageOrientation ()
{

    typedef FloatImageType::DirectionType DirectionType;

    DirectionType eyedir;
    eyedir.SetIdentity();
    if (!this->io->GetFileName())
    {
        qWarning() << "io doesn't have a filename, call readInformation first";
        return eyedir;
    }

    itk::MetaDataDictionary PARheader = this->io->GetMetaDataDictionary();

    typedef itk::PhilipsRECImageIO::AngulationMidSliceType AngulationType;

    AngulationType angulation;
    int sliceorientation = 0;

    bool valid = itk::ExposeMetaData<AngulationType>(PARheader, "PAR_AngulationMidSlice", angulation);
    if (!valid)
    {
        qWarning() <<"cannot find angulation in PAR header, no correction";
        return eyedir;
    }

    valid = itk::ExposeMetaData<int>(PARheader, "PAR_SliceOrientation", sliceorientation);
    if (!valid)
    {
        qWarning() << "cannot find slice orientation in PAR header, no correction";
        return eyedir;
    }

    DirectionType AFRtoLPS;
    AFRtoLPS.Fill (0);
    AFRtoLPS[0][2] = 1;
    AFRtoLPS[1][0] = 1;
    AFRtoLPS[2][1] = 1;

    DirectionType magicmatrix;
    magicmatrix.Fill (0);
    magicmatrix [0][0] = -1;
    magicmatrix [1][2] = 1;
    magicmatrix [2][1] = -1;

    DirectionType TRA;
    TRA.Fill (0);
    TRA [0][1] = 1;
    TRA [1][0] = -1;
    TRA [2][2] = -1;
    DirectionType SAG;
    SAG.Fill (0);
    SAG [0][0] = -1;
    SAG [1][2] = 1;
    SAG [2][1] = -1;
    DirectionType COR;
    COR.Fill (0);
    COR [0][1] = 1;
    COR [1][2] = 1;
    COR [2][0] = -1;


    DirectionType Torientation;

    switch(sliceorientation)
    {

    case PAR_SLICE_ORIENTATION_TRANSVERSAL:
        // Transverse - the REC data appears to be stored as right-left,
        // anterior-posterior, and inferior-superior.
        // Verified using a marker on right side of brain.
        Torientation = TRA;
        break;
    case PAR_SLICE_ORIENTATION_SAGITTAL:
        // Sagittal - the REC data appears to be stored as anterior-posterior,
        // superior-inferior, and right-left.
        // Verified using marker on right side of brain.
        Torientation = SAG;
        break;
    case PAR_SLICE_ORIENTATION_CORONAL:
        // Coronal - the REC data appears to be stored as right-left,
        // superior-inferior, and anterior-posterior.
        // Verified using marker on right side of brain.
        // fall thru
    default:
        Torientation = COR;

    }


    double ap = angulation[0] * vnl_math::pi / 180.0;
    double fh = angulation[1] * vnl_math::pi / 180.0;
    double rl = angulation[2] * vnl_math::pi / 180.0;

    DirectionType Tap;
    Tap.Fill (0);
    Tap[0][0] = 1;
    Tap[1][1] = std::cos (ap);
    Tap[1][2] = - std::sin (ap);
    Tap[2][1] = std::sin (ap);
    Tap[2][2] = std::cos (ap);

    DirectionType Tfh;
    Tfh.Fill (0);
    Tfh[1][1] = 1;
    Tfh[0][0] = std::cos (fh);
    Tfh[0][2] = std::sin (fh);
    Tfh[2][0] = - std::sin (fh);
    Tfh[2][2] = std::cos (fh);

    DirectionType Trl;
    Trl.Fill (0);
    Trl[2][2] = 1;
    Trl[0][0] = std::cos (rl);
    Trl[0][1] = - std::sin (rl);
    Trl[1][0] = std::sin (rl);
    Trl[1][1] = std::cos (rl);

    DirectionType TR = AFRtoLPS * Trl * Tap * Tfh * magicmatrix.GetTranspose() * Torientation.GetTranspose();
    DirectionType retval;
    retval.SetIdentity();

    for (unsigned int i=0; i<3; i++)
        for (unsigned int j=0; j<3; j++)
            retval[i][j] = TR[i][j];

    return retval;
}

//medItkImageDataPhilipsRecReader::VectorListType medItkImageDataPhilipsRecReader::ExtractPARRECGradientDirections
//(FloatImageType::DirectionType direction)
//{

//    VectorListType gradients;
//    if (!this->io->GetFileName())
//    {
//        qWarning() << "io doesn't have a filename, call readInformation first.";
//        return gradients;
//    }

//    itk::MetaDataDictionary PARheader = this->io->GetMetaDataDictionary();

//    typedef itk::PhilipsRECImageIO::GradientDirectionType GradientDirectionType;
//    typedef itk::PhilipsRECImageIO::GradientDirectionContainerType GradientDirectionContainerType;

//    GradientDirectionContainerType::Pointer parrecgradients = GradientDirectionContainerType::New();

//    bool valid = itk::ExposeMetaData<GradientDirectionContainerType::Pointer>(PARheader, "PAR_GradientDirectionValues", parrecgradients);
//    if (!valid)
//    {
//        qWarning() <<"cannot find gradient information in PAR header...";
//        return gradients;
//    }

//    FloatImageType::DirectionType inverse =  FloatImageType::DirectionType (direction.GetInverse());

//    FloatImageType::DirectionType AFRtoLPS;
//    AFRtoLPS.Fill (0);
//    AFRtoLPS[0][2] = 1;
//    AFRtoLPS[1][0] = 1;
//    AFRtoLPS[2][1] = 1;

//    for (unsigned int i=0; i<parrecgradients->Size(); i++)
//    {
//        GradientDirectionType parrecgradient = parrecgradients->ElementAt (i);
//        VectorType gradient;
//        for (unsigned int j=0; j<3; j++) gradient[j] = parrecgradient[j];
//        gradient = inverse * AFRtoLPS * gradient;
//        gradients.push_back (gradient);
//    }

//    return gradients;
//}





// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader* medItkImageDataPhilipsRecReader::create() {
    return new medItkImageDataPhilipsRecReader;
}
