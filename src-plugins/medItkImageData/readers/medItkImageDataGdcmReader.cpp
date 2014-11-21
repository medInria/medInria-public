/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageDataGdcmReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <itkImageFileReader.h>
#include <itkRGBPixel.h>
#include <itkGDCMImageIO.h>
#include <itkMetaDataDictionary.h>
#include <itkObjectFactoryBase.h>
#include <itkMetaDataObject.h>
#include <medItkImageDataReaderCommand.h>

#include <gdcmReader.h>
#include <gdcmDirectionCosines.h>
#include <gdcmStringFilter.h>

#include <medMetaDataKeys.h>

#include <map>

const char medItkImageDataGdcmReader::ID[] = "medItkImageDataGdcmReader";

template<typename TYPE>
void Read3DImage(medAbstractData* medData,itk::GDCMImageIO::Pointer io,const medItkImageDataGdcmReader::FileList& filelist) {
    typename itk::ImageSeriesReader<itk::Image<TYPE,3> >::Pointer reader = itk::ImageSeriesReader<itk::Image<TYPE,3> >::New();
    reader->SetImageIO(io);
    reader->SetFileNames(filelist);
    medData->setData(reader->GetOutput());
    reader->Update();
}

template <typename TYPE>
void Read4DImage(medAbstractData* medData, itk::GDCMImageIO::Pointer io, medItkImageDataGdcmReader::FileListMapType map) {
    typedef itk::Image<TYPE,4>                   ImageType;
    typedef itk::Image<TYPE,3>                   SubImageType;
    typedef itk::ImageSeriesReader<SubImageType> SeriesReaderType;
    typedef typename ImageType::RegionType                RegionType;
    typedef typename ImageType::SpacingType               SpacingType;
    typedef typename ImageType::PointType                 PointType;
    typedef typename ImageType::DirectionType             DirectionType;
    typedef itk::ImageRegionIterator<ImageType>  IteratorType;
    typedef typename IteratorType::IndexType              IndexType;

    typename ImageType::Pointer image = ImageType::New();

    bool metadatacopied = false;
    IteratorType itOut;

    std::cout << "Building volume containing\t " << map.size() << "\t subvolumes..." << std::flush;

    for (medItkImageDataGdcmReader::FileListMapType::iterator it=map.begin();it!=map.end();++it) {
        typename SeriesReaderType::Pointer seriesreader = SeriesReaderType::New();
        seriesreader->UseStreamingOn();

        typename SubImageType::Pointer t_image = 0;

        seriesreader->SetFileNames(it->second);
        seriesreader->SetImageIO(io);
        seriesreader->Update();

        t_image = seriesreader->GetOutput();

        if (!metadatacopied) {
            RegionType region;
            region.SetSize(0,t_image->GetLargestPossibleRegion().GetSize()[0]);
            region.SetSize(1,t_image->GetLargestPossibleRegion().GetSize()[1]);
            region.SetSize(2,t_image->GetLargestPossibleRegion().GetSize()[2]);
            region.SetSize(3,map.size());
            image->SetRegions(region);
            image->Allocate();

            SpacingType spacing;
            spacing[0] = t_image->GetSpacing()[0];
            spacing[1] = t_image->GetSpacing()[1];
            spacing[2] = t_image->GetSpacing()[2];
            spacing[3] = 1;
            image->SetSpacing(spacing);

            PointType origin;
            origin[0] = t_image->GetOrigin()[0];
            origin[1] = t_image->GetOrigin()[1];
            origin[2] = t_image->GetOrigin()[2];
            origin[3] = 0;
            image->SetOrigin(origin);

            DirectionType direction;
            for (unsigned int i=0; i<4; i++)
                for (unsigned int j=0; j<4; j++) {
                    if ((i < 3) && (j < 3))
                        direction[i][j] = t_image->GetDirection()[i][j];
                    else
                        direction[i][j] = (i == j) ? 1 : 0;
                }
            image->SetDirection(direction);
            itOut = IteratorType(image,region);

            image->SetMetaDataDictionary(io->GetMetaDataDictionary());

            metadatacopied = true;
        }

        itk::ImageRegionIterator<SubImageType> itIn(t_image,t_image->GetLargestPossibleRegion());
        while (!itIn.IsAtEnd()) {
            itOut.Set(itIn.Get());
            ++itIn;
            ++itOut;
        }
    }
    std::cout << "done" << std::endl;
    medData->setData(image);
}

class medItkImageDataGdcmReaderPrivate
{
public:
    medItkImageDataGdcmReaderPrivate();
    ~medItkImageDataGdcmReaderPrivate(){};

    itk::GDCMImageIO::Pointer io;
};

medItkImageDataGdcmReaderPrivate::medItkImageDataGdcmReaderPrivate()
{
    io = itk::GDCMImageIO::New();
}

medItkImageDataGdcmReader::medItkImageDataGdcmReader() : dtkAbstractDataReader(), d(new medItkImageDataGdcmReaderPrivate)
{
    this->m_Scanner.AddTag( gdcm::Tag(0x0010,0x0010) );
    this->m_Scanner.AddTag( gdcm::Tag(0x0008,0x0130) );
    this->m_Scanner.AddTag( gdcm::Tag(0x0008,0x103e) );
    this->m_Scanner.AddTag( gdcm::Tag(0x0020,0x000d) );
    this->m_Scanner.AddTag( gdcm::Tag(0x0020,0x000e) );
    this->m_Scanner.AddTag( gdcm::Tag(0x0020,0x0037) );
    this->m_Scanner.AddTag( gdcm::Tag(0x0020,0x0011) );
    this->m_Scanner.AddTag( gdcm::Tag(0x0018,0x0024) );
    this->m_Scanner.AddTag( gdcm::Tag(0x0018,0x0050) );
    this->m_Scanner.AddTag( gdcm::Tag(0x0028,0x0010) );
    this->m_Scanner.AddTag( gdcm::Tag(0x0028,0x0011) );

    this->m_Scanner.AddTag( gdcm::Tag(0x0020,0x0032) );
    this->m_Scanner.AddTag( gdcm::Tag(0x0020,0x0037) );

}


medItkImageDataGdcmReader::~medItkImageDataGdcmReader()
{
    delete d;
    d = 0;
}


bool medItkImageDataGdcmReader::registered()
{
    return medAbstractDataFactory::instance()->registerDataReaderType(
                "medItkImageDataGdcmReader",
                QStringList() << "medItkDouble3ImageData"
                << "medItkFloat3ImageData"
                << "medItkULong3ImageData"
                << "medItkLong3ImageData"
                << "medItkUInt3ImageData"
                << "medItkInt3ImageData"
                << "medItkInt4ImageData"
                << "medItkLong4ImageData"
                << "medItkUInt4ImageData"
                << "medItkULong4ImageData"
                << "medItkUShort3ImageData"
                << "medItkUShort4ImageData"
                << "medItkShort3ImageData"
                << "medItkShort4ImageData"
                << "medItkUChar3ImageData"
                << "medItkUChar4ImageData"
                << "medItkChar3ImageData"
                << "medItkChar4ImageData"
                << "medItkRgb3ImageData",
                createmedItkImageDataGdcmReader);
}


QStringList medItkImageDataGdcmReader::handled() const
{
    return QStringList() << "medItkDouble3ImageData"
                         << "medItkFloat3ImageData"
                         << "medItkULong3ImageData"
                         << "medItkLong3ImageData"
                         << "medItkUInt3ImageData"
                         << "medItkInt3ImageData"
                         << "medItkInt4ImageData"
                         << "medItkUInt4ImageData"
                         << "medItkULong4ImageData"
                         << "medItkLong4ImageData"
                         << "medItkUShort3ImageData"
                         << "medItkUShort4ImageData"
                         << "medItkShort3ImageData"
                         << "medItkShort4ImageData"
                         << "medItkUChar3ImageData"
                         << "medItkUChar4ImageData"
                         << "medItkChar3ImageData"
                         << "medItkChar4ImageData"
                         << "medItkRgb3ImageData";
}

QString medItkImageDataGdcmReader::identifier() const {
    return ID;
}

QString medItkImageDataGdcmReader::description() const {
    return "medItkImageDataGdcmReader";
}

bool medItkImageDataGdcmReader::canRead(const QString &path) {
    return d->io->CanReadFile(path.toAscii().constData());
}

bool medItkImageDataGdcmReader::canRead(const QStringList &paths) {
    for (int i=0; i<paths.size(); i++)
        if (!d->io->CanReadFile(paths[i].toAscii().constData()))
            return false;
    return true;
}

bool medItkImageDataGdcmReader::readInformation(const QString &path) {
    QStringList paths;
    paths << path;
    return readInformation(paths);
}

bool medItkImageDataGdcmReader::readInformation(const QStringList &paths)
{
    if (paths.size()==0)
        return false;

    FileList filenames;
    for (int i=0; i<paths.size(); i++)
        filenames.push_back(paths[i].toAscii().constData());

    FileListMapType map = this->sort(filenames);

    std::string firstfilename = (*map.begin()).second[0];

    d->io->SetFileName(firstfilename.c_str());
    try
    {
        d->io->ReadImageInformation();
    }
    catch(itk::ExceptionObject &e)
    {
        qDebug() << e.GetDescription();
        return false;
    }

    dtkSmartPointer<medAbstractData> medData = dynamic_cast<medAbstractData*>(this->data());

    if (!medData)
    {
        unsigned int imagedimension = 3;

        if (map.size() > 1)
        {
            std::cout<<"4th dimension encountered"<<std::endl;
            imagedimension = 4;
        }

        std::ostringstream imagetypestring;
        imagetypestring << "medItkImageData";

        if (d->io->GetPixelType() != itk::ImageIOBase::SCALAR)
        {
            qDebug() << "Unsupported pixel type";
            return false;
        }

        switch (d->io->GetComponentType())
        {
        case itk::ImageIOBase::UCHAR:
            imagetypestring << "UChar";
            break;
        case itk::ImageIOBase::CHAR:
            imagetypestring << "Char";
            break;
        case itk::ImageIOBase::USHORT:
            imagetypestring << "UShort";
            break;
        case itk::ImageIOBase::SHORT:
            imagetypestring << "Short";
            break;
        case itk::ImageIOBase::UINT:
            imagetypestring << "UInt";
            break;
        case itk::ImageIOBase::INT:
            imagetypestring << "Int";
            break;
        case itk::ImageIOBase::ULONG:
            imagetypestring << "ULong";
            break;
        case itk::ImageIOBase::LONG:
            imagetypestring << "Long";
            break;
        case itk::ImageIOBase::FLOAT:
            imagetypestring << "Float";
            break;
        case itk::ImageIOBase::DOUBLE:
            /**
      @todo Handle properly double pixel values.
      For the moment it is only handled in 3D, not in 4D, and it is very
      common to have 4D double images (cardiac).
      This hack just downcast images in short when the dimension is 4.
      which is WRONG.
   */
            if (imagedimension == 4)
                imagetypestring << "Short";
            else
                imagetypestring << "Double";
            break;
        default:
            qDebug() << "Unrecognized component type:\t " << d->io->GetComponentType();
            return false;
        }

        imagetypestring << imagedimension;
        if (imagedimension == 4)
            qDebug() << "image type given :\t" << imagetypestring.str().c_str();

        medData = medAbstractDataFactory::instance()->createSmartPointer(imagetypestring.str().c_str());
        if (medData)
            this->setData(medData);
    }

    if (medData)
    {
        QStringList patientName;
        QStringList studyName;
        QStringList seriesName;
        QStringList studyId;
        QStringList seriesId;
        QStringList orientation;
        QStringList seriesNumber;
        QStringList sequenceName;
        QStringList sliceThickness;
        QStringList rows;
        QStringList columns;
        QStringList filePaths;

        patientName    << this->m_Scanner.GetValue(firstfilename.c_str(), gdcm::Tag(0x0010,0x0010));
        studyName      << this->m_Scanner.GetValue(firstfilename.c_str(), gdcm::Tag(0x0008,0x0130));
        seriesName     << this->m_Scanner.GetValue(firstfilename.c_str(), gdcm::Tag(0x0008,0x103e));
        studyId        << this->m_Scanner.GetValue(firstfilename.c_str(), gdcm::Tag(0x0020,0x000d));
        seriesId       << this->m_Scanner.GetValue(firstfilename.c_str(), gdcm::Tag(0x0020,0x000e));
        orientation    << this->m_Scanner.GetValue(firstfilename.c_str(), gdcm::Tag(0x0020,0x0037));
        seriesNumber   << this->m_Scanner.GetValue(firstfilename.c_str(), gdcm::Tag(0x0020,0x0011));
        sequenceName   << this->m_Scanner.GetValue(firstfilename.c_str(), gdcm::Tag(0x0018,0x0024));
        sliceThickness << this->m_Scanner.GetValue(firstfilename.c_str(), gdcm::Tag(0x0018,0x0050));
        rows           << this->m_Scanner.GetValue(firstfilename.c_str(), gdcm::Tag(0x0028,0x0010));
        columns        << this->m_Scanner.GetValue(firstfilename.c_str(), gdcm::Tag(0x0028,0x0011));

        //  TODO: Use the medMetaDataKeys iteration scheme...

        if (!medData->hasMetaData(medMetaDataKeys::PatientName.key()))
            medData->addMetaData(medMetaDataKeys::PatientName.key(),patientName);
        else
            medData->setMetaData(medMetaDataKeys::PatientName.key(),patientName);

        if (!medData->hasMetaData(medMetaDataKeys::StudyDescription.key()))
            medData->addMetaData(medMetaDataKeys::StudyDescription.key(),studyName);
        else
            medData->setMetaData(medMetaDataKeys::StudyDescription.key(),studyName);

        if (!medData->hasMetaData(medMetaDataKeys::SeriesDescription.key()))
            medData->addMetaData(medMetaDataKeys::SeriesDescription.key(),seriesName);
        else
            medData->setMetaData(medMetaDataKeys::SeriesDescription.key(),seriesName);

        medData->setMetaData(medMetaDataKeys::StudyID.key(),studyId);
        medData->setMetaData(medMetaDataKeys::SeriesID.key(),seriesId);
        medData->setMetaData(medMetaDataKeys::Orientation.key(),orientation);
        medData->setMetaData(medMetaDataKeys::SeriesNumber.key(),seriesNumber);
        medData->setMetaData(medMetaDataKeys::SequenceName.key(),sequenceName);
        medData->setMetaData(medMetaDataKeys::SliceThickness.key(),sliceThickness);
        medData->setMetaData(medMetaDataKeys::Rows.key(),rows);
        medData->setMetaData(medMetaDataKeys::Columns.key(),columns);

        FileList orderedfilelist = this->unfoldMap(map);
        for (unsigned int i=0; i<orderedfilelist.size(); i++)
            filePaths << orderedfilelist[i].c_str();

        medData->addMetaData(medMetaDataKeys::FilePaths.key(),filePaths);

    }

    return true;
}

bool medItkImageDataGdcmReader::read(const QString &path) {
    QStringList paths;
    paths << path;
    return read(paths);
}

bool medItkImageDataGdcmReader::read (const QStringList &paths)
{
    if (paths.size()==0)
        return false;

    this->readInformation(paths);

    FileList filenames;
    for (int i=0;i<paths.size();i++)
        filenames.push_back(paths[i].toAscii().constData());

    FileListMapType map = this->sort(filenames);

    if (!map.size()) {
        qDebug() << "No image can be build from file list (empty map)";
        return false;
    }

    itk::DataImageReaderCommand::Pointer command = itk::DataImageReaderCommand::New();
    command->SetDataImageReader(this);
    d->io->AddObserver(itk::ProgressEvent(),command);

    if (medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data())) {
        QStringList qfilelist = medData->metaDataValues("FilePaths");
        FileList filelist;
        for (int i=0;i<qfilelist.size();i++)
            filelist.push_back(qfilelist[i].toAscii().constData());

        std::cout << "reading : "    << medData->identifier().toAscii().constData() << std::endl;
        std::cout << "containing : " << map.size() << " volumes" << std::endl;

        try {
            if      (medData->identifier()=="medItkUChar3ImageData")  { Read3DImage<unsigned char>(medData,d->io,filelist);  }
            else if (medData->identifier()=="medItkChar3ImageData")   { Read3DImage<char>(medData,d->io,filelist);           }
            else if (medData->identifier()=="medItkUShort3ImageData") { Read3DImage<unsigned short>(medData,d->io,filelist); }
            else if (medData->identifier()=="medItkShort3ImageData")  { Read3DImage<short>(medData,d->io,filelist);          }
            else if (medData->identifier()=="medItkUInt3ImageData")   { Read3DImage<unsigned int>(medData,d->io,filelist);   }
            else if (medData->identifier()=="medItkInt3ImageData")    { Read3DImage<int>(medData,d->io,filelist);            }
            else if (medData->identifier()=="medItkULong3ImageData")  { Read3DImage<unsigned long>(medData,d->io,filelist);  }
            else if (medData->identifier()=="medItkLong3ImageData")   { Read3DImage<long>(medData,d->io,filelist);           }
            else if (medData->identifier()=="medItkFloat3ImageData")  { Read3DImage<float>(medData,d->io,filelist);          }
            else if (medData->identifier()=="medItkDouble3ImageData") { Read3DImage<double>(medData,d->io,filelist);         }
            else if (medData->identifier()=="medItkUChar4ImageData")  { Read4DImage<unsigned char>(medData,d->io,map);          }
            else if (medData->identifier()=="medItkUShort4ImageData") { Read4DImage<unsigned short>(medData,d->io,map);         }
            else if (medData->identifier()=="medItkShort4ImageData")  { Read4DImage<short>(medData,d->io,map);                  }
            else if (medData->identifier()=="medItkUInt4ImageData")   { Read4DImage<unsigned int>(medData,d->io,map);           }
            else if (medData->identifier()=="medItkULong4ImageData")  { Read4DImage<unsigned long>(medData,d->io,map);          }
            else if (medData->identifier()=="medItkInt4ImageData")    { Read4DImage<int>(medData,d->io,map);                    }
            else if (medData->identifier()=="medItkLong4ImageData")   { Read4DImage<long>(medData,d->io,map);                   }
            else if (medData->identifier()=="medItkChar4ImageData")   { Read4DImage<char>(medData,d->io,map);                   }
            else if (medData->identifier()=="medItkDouble4ImageData") {
                /**
                @todo Handle properly double pixel values.
                For the moment it is only handled in 3D, not in 4D, and it is very
                common to have 4D double images (cardiac).
                This hack just downcast images in short when the dimension is 4.
                which is WRONG.
                 */
                Read4DImage<short>(medData,d->io,map);
            } else {
                qDebug() << "Unhandled medData type : " << medData->identifier();
                return false;
            }
        } catch (itk::ExceptionObject &e) {
            qDebug() << e.GetDescription();
            return false;
        }


    }


    if (medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data()))
    {

        // copy over the dicom dictionary into metadata
        typedef itk::MetaDataObject <std::string>                 MetaDataStringType;
        typedef itk::MetaDataObject <std::vector<std::string> >   MetaDataVectorStringType;
        typedef std::vector<std::string>                     StringVectorType;

        const itk::MetaDataDictionary& dictionary = d->io->GetMetaDataDictionary();
        itk::MetaDataDictionary::ConstIterator it = dictionary.Begin();
        while(it!=dictionary.End()) {
            if( MetaDataVectorStringType* metaData = dynamic_cast<MetaDataVectorStringType*>( it->second.GetPointer() ) ) {
                const StringVectorType &values = metaData->GetMetaDataObjectValue();
                for (unsigned int i=0; i<values.size(); i++)
                    medData->addMetaData( it->first.c_str(), values[i].c_str());
            }
            ++it;
        }
    }


    d->io->RemoveAllObservers ();

    return true;
}

medItkImageDataGdcmReader::FileListMapType medItkImageDataGdcmReader::sort (FileList filelist)
{

    this->m_Scanner.Scan(filelist);
    FileListMapType ret;

    if (!filelist.size())
        return ret;

    const char *reference = filelist[0].c_str();

    if (filelist.size() == 1)
    {
        FileListMapType::value_type newpair("unique_file",filelist);
        ret.insert(newpair);
        return ret;
    }

    const gdcm::Tag orientationtag(0x20,0x37);

    gdcm::Scanner::ValuesType orientations = this->m_Scanner.GetValues(orientationtag);
    if (orientations.size() != 1)
    {
        qDebug() <<"More than one Orientation in filenames (or no Orientation)";
        return ret;
    }

    gdcm::Scanner::TagToValue const &t2v = this->m_Scanner.GetMapping(reference);
    gdcm::Scanner::TagToValue::const_iterator firstit = t2v.find(orientationtag);
    if ((*firstit).first != orientationtag)
    {
        qDebug() <<"Could not find any orientation information in the header of the reference file";
        return ret;
    }

    const char *dircos = (*firstit).second;
    std::stringstream ss;
    ss.str(dircos);

    gdcm::Element<gdcm::VR::DS,gdcm::VM::VM6> cosines;
    cosines.Read(ss);

    // http://www.itk.org/pipermail/insight-users/2003-September/004762.html
    // Compute normal:
    // The steps I take when reconstructing a volume are these: First,
    // calculate the slice normal from IOP:
    double normal[3];
    normal[0] = cosines[1]*cosines[5] - cosines[2]*cosines[4];
    normal[1] = cosines[2]*cosines[3] - cosines[0]*cosines[5];
    normal[2] = cosines[0]*cosines[4] - cosines[1]*cosines[3];

    SortedMapType sorted;

    FileList::const_iterator it;

    for(it = filelist.begin(); it != filelist.end(); ++it)
    {
        const char *filename = (*it).c_str();
        bool iskey = this->m_Scanner.IsKey(filename);
        if (iskey)
        {
            const char *value = this->m_Scanner.GetValue(filename,gdcm::Tag(0x20,0x32));
            if (value)
            {
                gdcm::Element<gdcm::VR::DS,gdcm::VM::VM3> ipp;
                std::stringstream ss;
                ss.str(value);
                ipp.Read(ss);
                double dist = 0;
                for (int i = 0; i < 3; ++i)
                    dist += normal[i]*ipp[i];

                bool found = 0;
                SortedMapType::iterator finder;
                for (finder = sorted.begin(); finder != sorted.end(); ++finder)
                    if (std::abs((*finder).first - dist) < itk::NumericTraits<double>::min())
                    {
                        found = 1;
                        break;
                    }

                if (!found)
                {
                    FileList newlist;
                    newlist.push_back(filename);
                    SortedMapType::value_type newpair(dist,newlist);
                    sorted.insert(newpair);
                }
                else
                    (*finder).second.push_back(filename);
            }
        }
        else
        {
            qDebug() << "The file "
                     << filename
                     <<" does not appear in the scanner mappings, skipping. ";
        }
    }

    if ((filelist.size() % sorted.size()) != 0)
    {
        qDebug() << "There appears to be inconsistent file list sizes\n "
                 << "Scanner outputs "<<sorted.size()<<" different image positions\n "
                 << "within a total list of "<<filelist.size()<<" files.\n"
                 << "no sorting will be performed\n";
        return ret;

        ///\todo Here we could take into account the files that are consistent
        /// to each other and leave the rest, to build a truncated volume.
    }

    unsigned int nb_of_volumes = filelist.size() / sorted.size();

    for (unsigned int i=0; i<nb_of_volumes; i++)
    {
        std::ostringstream os;
        os<<".";
        if (i < 100)
            os << "0";
        if (i < 10)
            os << "0";
        os<<i;

        FileList newfilelist;

        SortedMapType::const_iterator toinsert;

        for (toinsert = sorted.begin(); toinsert != sorted.end(); ++toinsert)
        {
            newfilelist.push_back((*toinsert).second[i]);
        }

        FileListMapType::value_type newpair(os.str(),newfilelist);
        ret.insert(newpair);
    }

    return ret;
}

medItkImageDataGdcmReader::FileList medItkImageDataGdcmReader::unfoldMap (FileListMapType map)
{
    FileList ret;

    FileListMapType::const_iterator it;
    for (it = map.begin(); it != map.end(); ++it)
    {
        FileList filelist = (*it).second;
        for (unsigned int i=0; i<filelist.size(); i++)
            ret.push_back(filelist[i]);
    }

    return ret;
}


void medItkImageDataGdcmReader::setProgress (int value)
{
    emit progressed (value);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createmedItkImageDataGdcmReader()
{
    return new medItkImageDataGdcmReader;
}

