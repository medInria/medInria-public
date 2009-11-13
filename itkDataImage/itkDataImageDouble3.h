#ifndef _itk_DataImageDouble3_h_
#define _itk_DataImageDouble3_h_

#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkGlobal.h>

#include "itkDataImagePluginExport.h"


class itkDataImageDouble3Private;

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageDouble3 : public dtkAbstractDataImage
{
  Q_OBJECT
    
 public:
    itkDataImageDouble3(void);
    ~itkDataImageDouble3(void);
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

    itkDataImageDouble3Private* d;
  
};

dtkAbstractData* createItkDataImageDouble3 (void);

#endif
