#ifndef _itk_DataImageUShort4_h_
#define _itk_DataImageUShort4_h_

#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkGlobal.h>

#include "itkDataImagePluginExport.h"




class itkDataImageUShort4Private;

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageUShort4 : public dtkAbstractDataImage
{
  Q_OBJECT
    
 public:
    itkDataImageUShort4(void);
    ~itkDataImageUShort4(void);
    virtual QString description(void) const;
    static bool registered(void);

    virtual QImage        &thumbnail(void)   const;
    virtual QList<QImage> &thumbnails (void) const;

 public slots:
    // derived from dtkAbstractData
    
    void *output(void);
    void *data(void);
    void setData(void* data);
    void update(void);

    void onMetaDataSet(QString key, QString value);
    void onPropertySet(QString key, QString value);

 public:
    // derived from dtkAbstractDataImage

    int xDimension(void);
    int yDimension(void);
    int zDimension(void);

    int minRangeValue(void);
    int maxRangeValue(void);

    int scalarValueCount(int value);
    int scalarValueMinCount(void);
    int scalarValueMaxCount(void);
    
 private:

    itkDataImageUShort4Private* d;
  
};

dtkAbstractData* createItkDataImageUShort4 (void);

#endif
