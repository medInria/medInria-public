#ifndef _med_ItkDataImageReaderMacros_h_
#define _med_ItkDataImageReaderMacros_h_


#define medImplementItkDataImageReader(type, dimension, suffix)		\
  class itkDataImage##suffix##ReaderPrivate				\
  {									\
  public:								\
  };									\
  itkDataImage##suffix##Reader::itkDataImage##suffix##Reader(void) : dtkAbstractDataReader(), d (new itkDataImage##suffix##ReaderPrivate) \
  {}									\
  itkDataImage##suffix##Reader::~itkDataImage##suffix##Reader(void)	\
  {}									\
  bool itkDataImage##suffix##Reader::registered(void)			\
  {									\
    return dtkAbstractDataFactory::instance()->registerDataReaderType("itkDataImage"#suffix"Reader", QStringList() << "itkDataImage"#suffix, createItkDataImage##suffix##Reader); \
  }									\
  QString itkDataImage##suffix##Reader::description(void) const		\
  {									\
    return "itkDataImage"#suffix"Reader";				\
  }									\
  QStringList itkDataImage##suffix##Reader::handled(void) const		\
  {									\
    return QStringList() << "itkDataImage"#suffix;			\
  }									\
  bool itkDataImage##suffix##Reader::read(QString path)			\
  {									\
    typedef type         PixelType;					\
    typedef itk::Image<PixelType, dimension> ImageType;			\
    typedef itk::ImageFileReader<ImageType> ReaderType;			\
    ReaderType::Pointer reader = ReaderType::New();			\
    reader->SetFileName (path.toAscii().constData());			\
    try{								\
      reader->Update();							\
    }									\
    catch(itk::ExceptionObject &ex) {					\
      std::cerr << ex << std::endl;					\
      return false;							\
    }									\
    std::cout << "Was able to read, with ITK IO: " << reader->GetImageIO()->GetNameOfClass() << std::endl; \
    if( dtkAbstractData *dtkdata = this->data() )			\
      dtkdata->setData( reader->GetOutput() );				\
    return true;							\
  }									\
  dtkAbstractDataReader *createItkDataImage##suffix##Reader(void)	\
  {									\
    return new itkDataImage##suffix##Reader;				\
  }

#endif
