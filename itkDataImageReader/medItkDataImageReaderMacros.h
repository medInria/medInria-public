#ifndef _med_ItkDataImageReaderMacros_h_
#define _med_ItkDataImageReaderMacros_h_


#define medImplementItkDataImageReader(type, dimension, suffix, itkcomponent, itktype) \
  class itkDataImage##suffix##ReaderPrivate				\
  {									\
  public:								\
  typedef type         PixelType;					\
  typedef itk::Image<PixelType, dimension> ImageType;			\
  typedef itk::ImageFileReader<ImageType> ReaderType;			\
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
  bool itkDataImage##suffix##Reader::canRead (QStringList paths)	\
  {									\
    if (paths.count()==1)						\
      return this->canRead (paths[0]);					\
    return false;							\
  }									\
  bool itkDataImage##suffix##Reader::canRead (QString path)		\
  {									\
    typedef type                             PixelType;			\
    typedef itk::Image<PixelType, dimension> ImageType;			\
    typedef itk::ImageFileReader<ImageType> ReaderType;			\
    ReaderType::Pointer reader = ReaderType::New();			\
    reader->SetFileName (path.toAscii().constData());			\
    try									\
    {									\
      reader->GenerateOutputInformation();				\
    }									\
    catch(itk::ExceptionObject &ex) {					\
      std::cerr << ex << std::endl;					\
      return false;							\
    }									\
    if ( strcmp (reader->GetImageIO()->GetNameOfClass(), "GDCMImageIO")==0 || \
	 reader->GetImageIO()->GetComponentType()!=itk::ImageIOBase::itkcomponent || \
	 reader->GetImageIO()->GetPixelType()!=itk::ImageIOBase::itktype) \
      return false;							\
    return true;							\
  }									\
  void itkDataImage##suffix##Reader::readInformation (QString path)	\
  {									\
    dtkAbstractData* dtkdata = this->data();				\
    if (!dtkdata) {							\
      dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataImage"#suffix); \
      if (dtkdata)							\
	this->setData ( dtkdata );					\
    }									\
  }									\
  bool itkDataImage##suffix##Reader::read(QStringList paths)		\
  {									\
    if (paths.count()==1)						\
      return this->read (paths[0]);					\
    return false;							\
  }									\
  bool itkDataImage##suffix##Reader::read(QString path)			\
  {									\
    if (!this->data())							\
      this->readInformation (path);					\
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
