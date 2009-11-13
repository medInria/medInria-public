#ifndef _itk_DataImageFloat3_h_
#define _itk_DataImageFloat3_h_

#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkGlobal.h>

#include "itkDataImagePluginExport.h"


class itkDataImageFloat3Private;

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageFloat3 : public dtkAbstractDataImage
{
  Q_OBJECT
    
 public:
    itkDataImageFloat3(void);
    ~itkDataImageFloat3(void);
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

    itkDataImageFloat3Private* d;
  
};

dtkAbstractData* createItkDataImageFloat3 (void);

#endif
