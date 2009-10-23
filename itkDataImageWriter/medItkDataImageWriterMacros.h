#ifndef _med_ItkDataImageWriterMacros_h_
#define _med_ItkDataImageWriterMacros_h_


#define medImplementItkDataImageWriterMacro(type, dimension, suffix)	\
  class itkDataImage##suffix##WriterPrivate				\
  {									\
  public:								\
  };									\
  itkDataImage##suffix##Writer::itkDataImage##suffix##Writer(void) : dtkAbstractDataWriter(), d(new itkDataImage##suffix##WriterPrivate) \
  {									\
  }									\
  itkDataImage##suffix##Writer::~itkDataImage##suffix##Writer(void)	\
  {									\
  }									\
  bool itkDataImage##suffix##Writer::registered(void)			\
  {									\
    return dtkAbstractDataFactory::instance()->registerDataWriterType("itkDataImage"#suffix"Writer", QStringList() << "itkDataImage"#suffix, createItkDataImage##suffix##Writer); \
  }									\
  QString itkDataImage##suffix##Writer::description(void) const		\
  {									\
    return "itkDataImage"#suffix"Writer";				\
  }									\
  QStringList itkDataImage##suffix##Writer::handled(void) const		\
  {									\
    return QStringList() << "itkDataImage"#suffix;			\
  }									\
  bool itkDataImage##suffix##Writer::canWrite (QString path)		\
  {									\
    if (dtkAbstractData *data = this->data()) {				\
      if (data->description()=="itkDataImage"#suffix) {			\
	itk::ImageIOBase::Pointer io = itk::ImageIOFactory::CreateImageIO( path.toAscii().constData(), \
									   itk::ImageIOFactory::WriteMode ); \
	return !io.IsNull();						\
      }									\
    }									\
    return false;							\
  }									\
  bool itkDataImage##suffix##Writer::write (QString path)		\
  {									\
    if (!this->data())							\
      return false;							\
    typedef type                             PixelType;			\
    typedef itk::Image<PixelType, dimension> ImageType;			\
    ImageType::Pointer image = dynamic_cast<ImageType*>( (itk::Object*)(this->data()->output()) ); \
    if (image.IsNull())							\
      return false;							\
    typedef itk::ImageFileWriter<ImageType> WriterType;			\
    WriterType::Pointer writer = WriterType::New();			\
    writer->SetFileName ( path.toAscii().constData() );			\
    writer->SetInput ( image );						\
    try {								\
      writer->Update();							\
    }									\
    catch(itk::ExceptionObject &ex) {					\
      std::cerr << ex << std::endl;					\
      return false;							\
    }									\
    std::cout << "Was able to writer, with ITK IO: " << writer->GetImageIO()->GetNameOfClass() << std::endl; \
    return true;							\
  }									\
  dtkAbstractDataWriter *createItkDataImage##suffix##Writer(void)	\
  {									\
    return new itkDataImage##suffix##Writer;				\
  }									\
  

#endif
