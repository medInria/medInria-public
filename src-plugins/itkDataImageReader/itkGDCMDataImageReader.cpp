/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkGDCMDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <itkImageFileReader.h>
#include <itkRGBPixel.h>
#include <itkGDCMImageIO.h>
#include <itkMetaDataDictionary.h>
#include <itkObjectFactoryBase.h>
#include <itkMetaDataObject.h>
#include <itkDataImageReaderCommand.h>

#include <gdcmReader.h>
#include <gdcmDirectionCosines.h>
#include <gdcmStringFilter.h>

#include <medCore/medMetaDataKeys.h>

#include <map>

const char itkGDCMDataImageReader::ID[] = "itkGDCMDataImageReader";;

template<typename TYPE>
void Read3DImage(dtkAbstractData* dtkdata,itk::GDCMImageIO::Pointer io,const itkGDCMDataImageReader::FileList& filelist) {
    typename itk::ImageSeriesReader<itk::Image<TYPE,3> >::Pointer reader = itk::ImageSeriesReader<itk::Image<TYPE,3> >::New();
    reader->SetImageIO(io);
    reader->SetFileNames(filelist);
    dtkdata->setData(reader->GetOutput());
    reader->Update();
}

template <typename TYPE>
void Read4DImage(dtkAbstractData* dtkdata, itk::GDCMImageIO::Pointer io, itkGDCMDataImageReader::FileListMapType map) {
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

    for (itkGDCMDataImageReader::FileListMapType::iterator it=map.begin();it!=map.end();++it) {
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
    dtkdata->setData(image);
}

class itkGDCMDataImageReaderPrivate
{
public:
    itkGDCMDataImageReaderPrivate();
    ~itkGDCMDataImageReaderPrivate(){};

    itk::GDCMImageIO::Pointer io;
};

itkGDCMDataImageReaderPrivate::itkGDCMDataImageReaderPrivate()
{
    io = itk::GDCMImageIO::New();
}

itkGDCMDataImageReader::itkGDCMDataImageReader() : dtkAbstractDataReader(), d(new itkGDCMDataImageReaderPrivate)
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


itkGDCMDataImageReader::~itkGDCMDataImageReader()
{
    delete d;
    d = 0;
}


bool itkGDCMDataImageReader::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataReaderType(
                "itkGDCMDataImageReader",
                QStringList() << "itkDataImageDouble3"
                << "itkDataImageFloat3"
                << "itkDataImageULong3"
                << "itkDataImageLong3"
                << "itkDataImageUInt3"
                << "itkDataImageInt3"
                << "itkDataImageInt4"
                << "itkDataImageLong4"
                << "itkDataImageUInt4"
                << "itkDataImageULong4"
                << "itkDataImageUShort3"
                << "itkDataImageUShort4"
                << "itkDataImageShort3"
                << "itkDataImageShort4"
                << "itkDataImageUChar3"
                << "itkDataImageUChar4"
                << "itkDataImageChar3"
                << "itkDataImageChar4"
                << "itkDataImageRGB3",
                createItkGDCMDataImageReader);
}


QStringList itkGDCMDataImageReader::handled() const
{
    return QStringList() << "itkDataImageDouble3"
                         << "itkDataImageFloat3"
                         << "itkDataImageULong3"
                         << "itkDataImageLong3"
                         << "itkDataImageUInt3"
                         << "itkDataImageInt3"
                         << "itkDataImageInt4"
                         << "itkDataImageUInt4"
                         << "itkDataImageULong4"
                         << "itkDataImageLong4"
                         << "itkDataImageUShort3"
                         << "itkDataImageUShort4"
                         << "itkDataImageShort3"
                         << "itkDataImageShort4"
                         << "itkDataImageUChar3"
                         << "itkDataImageUChar4"
                         << "itkDataImageChar3"
                         << "itkDataImageChar4"
                         << "itkDataImageRGB3";
}

QString itkGDCMDataImageReader::identifier() const {
    return ID;
}

QString itkGDCMDataImageReader::description() const {
    return "itkGDCMDataImageReader";
}

bool itkGDCMDataImageReader::canRead(QString path) {
    return d->io->CanReadFile(path.toAscii().constData());
}

bool itkGDCMDataImageReader::canRead(QStringList paths) {
    for (int i=0; i<paths.size(); i++)
        if (!d->io->CanReadFile(paths[i].toAscii().constData()))
            return false;
    return true;
}

void itkGDCMDataImageReader::readInformation(QString path) {
    QStringList paths;
    paths << path;
    readInformation(paths);
}

void itkGDCMDataImageReader::readInformation(QStringList paths)
{
    if (paths.size()==0)
        return;

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
        return;
    }

    dtkSmartPointer<dtkAbstractData> dtkdata = this->data();

    if (!dtkdata)
    {
        unsigned int imagedimension = 3;

        if (map.size() > 1)
        {
            std::cout<<"4th dimension encountered"<<std::endl;
            imagedimension = 4;
        }

        std::ostringstream imagetypestring;
        imagetypestring << "itkDataImage";

        if (d->io->GetPixelType() != itk::ImageIOBase::SCALAR)
        {
            qDebug() << "Unsupported pixel type";
            return;
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
            return;
        }

        imagetypestring << imagedimension;
        if (imagedimension == 4)
            qDebug() << "image type given :\t" << imagetypestring.str().c_str();

        dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer(imagetypestring.str().c_str());
        if (dtkdata)
            this->setData(dtkdata);
    }

    if (dtkdata)
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

        if (!dtkdata->hasMetaData(medMetaDataKeys::PatientName.key()))
            dtkdata->addMetaData(medMetaDataKeys::PatientName.key(),patientName);
        else
            dtkdata->setMetaData(medMetaDataKeys::PatientName.key(),patientName);

        if (!dtkdata->hasMetaData(medMetaDataKeys::StudyDescription.key()))
            dtkdata->addMetaData(medMetaDataKeys::StudyDescription.key(),studyName);
        else
            dtkdata->setMetaData(medMetaDataKeys::StudyDescription.key(),studyName);

        if (!dtkdata->hasMetaData(medMetaDataKeys::SeriesDescription.key()))
            dtkdata->addMetaData(medMetaDataKeys::SeriesDescription.key(),seriesName);
        else
            dtkdata->setMetaData(medMetaDataKeys::SeriesDescription.key(),seriesName);

        dtkdata->setMetaData(medMetaDataKeys::StudyID.key(),studyId);
        dtkdata->setMetaData(medMetaDataKeys::SeriesID.key(),seriesId);
        dtkdata->setMetaData(medMetaDataKeys::Orientation.key(),orientation);
        dtkdata->setMetaData(medMetaDataKeys::SeriesNumber.key(),seriesNumber);
        dtkdata->setMetaData(medMetaDataKeys::SequenceName.key(),sequenceName);
        dtkdata->setMetaData(medMetaDataKeys::SliceThickness.key(),sliceThickness);
        dtkdata->setMetaData(medMetaDataKeys::Rows.key(),rows);
        dtkdata->setMetaData(medMetaDataKeys::Columns.key(),columns);

        FileList orderedfilelist = this->unfoldMap(map);
        for (unsigned int i=0; i<orderedfilelist.size(); i++)
            filePaths << orderedfilelist[i].c_str();

        dtkdata->addMetaData(medMetaDataKeys::FilePaths.key(),filePaths);

    }
}

bool itkGDCMDataImageReader::read(QString path) {
    QStringList paths;
    paths << path;
    return read(paths);
}

bool itkGDCMDataImageReader::read (QStringList paths)
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

    if (dtkAbstractData *dtkdata = this->data()) {
        QStringList qfilelist = dtkdata->metaDataValues("FilePaths");
        FileList filelist;
        for (int i=0;i<qfilelist.size();i++)
            filelist.push_back(qfilelist[i].toAscii().constData());

        std::cout << "reading : "    << dtkdata->identifier().toAscii().constData() << std::endl;
        std::cout << "containing : " << map.size() << " volumes" << std::endl;

        try {
            if      (dtkdata->identifier()=="itkDataImageUChar3")  { Read3DImage<unsigned char>(dtkdata,d->io,filelist);  }
            else if (dtkdata->identifier()=="itkDataImageChar3")   { Read3DImage<char>(dtkdata,d->io,filelist);           }
            else if (dtkdata->identifier()=="itkDataImageUShort3") { Read3DImage<unsigned short>(dtkdata,d->io,filelist); }
            else if (dtkdata->identifier()=="itkDataImageShort3")  { Read3DImage<short>(dtkdata,d->io,filelist);          }
            else if (dtkdata->identifier()=="itkDataImageUInt3")   { Read3DImage<unsigned int>(dtkdata,d->io,filelist);   }
            else if (dtkdata->identifier()=="itkDataImageInt3")    { Read3DImage<int>(dtkdata,d->io,filelist);            }
            else if (dtkdata->identifier()=="itkDataImageULong3")  { Read3DImage<unsigned long>(dtkdata,d->io,filelist);  }
            else if (dtkdata->identifier()=="itkDataImageLong3")   { Read3DImage<long>(dtkdata,d->io,filelist);           }
            else if (dtkdata->identifier()=="itkDataImageFloat3")  { Read3DImage<float>(dtkdata,d->io,filelist);          }
            else if (dtkdata->identifier()=="itkDataImageDouble3") { Read3DImage<double>(dtkdata,d->io,filelist);         }
            else if (dtkdata->identifier()=="itkDataImageUChar4")  { Read4DImage<unsigned char>(dtkdata,d->io,map);          }
            else if (dtkdata->identifier()=="itkDataImageUShort4") { Read4DImage<unsigned short>(dtkdata,d->io,map);         }
            else if (dtkdata->identifier()=="itkDataImageShort4")  { Read4DImage<short>(dtkdata,d->io,map);                  }
            else if (dtkdata->identifier()=="itkDataImageUInt4")   { Read4DImage<unsigned int>(dtkdata,d->io,map);           }
            else if (dtkdata->identifier()=="itkDataImageULong4")  { Read4DImage<unsigned long>(dtkdata,d->io,map);          }
            else if (dtkdata->identifier()=="itkDataImageInt4")    { Read4DImage<int>(dtkdata,d->io,map);                    }
            else if (dtkdata->identifier()=="itkDataImageLong4")   { Read4DImage<long>(dtkdata,d->io,map);                   }
            else if (dtkdata->identifier()=="itkDataImageChar4")   { Read4DImage<char>(dtkdata,d->io,map);                   }
            else if (dtkdata->identifier()=="itkDataImageDouble4") {
                /**
                @todo Handle properly double pixel values.
                For the moment it is only handled in 3D, not in 4D, and it is very
                common to have 4D double images (cardiac).
                This hack just downcast images in short when the dimension is 4.
                which is WRONG.
                 */
                Read4DImage<short>(dtkdata,d->io,map);
            } else {
                qDebug() << "Unhandled dtkdata type : " << dtkdata->identifier();
                return false;
            }
        } catch (itk::ExceptionObject &e) {
            qDebug() << e.GetDescription();
            return false;
        }


    }


    if (dtkAbstractData *dtkdata = this->data())
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
                    dtkdata->addMetaData( it->first.c_str(), values[i].c_str());
            }
            ++it;
        }
    }


    d->io->RemoveAllObservers ();

    return true;
}

itkGDCMDataImageReader::FileListMapType itkGDCMDataImageReader::sort (FileList filelist)
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

itkGDCMDataImageReader::FileList itkGDCMDataImageReader::unfoldMap (FileListMapType map)
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


void itkGDCMDataImageReader::setProgress (int value)
{
    emit progressed (value);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkGDCMDataImageReader()
{
    return new itkGDCMDataImageReader;
}

