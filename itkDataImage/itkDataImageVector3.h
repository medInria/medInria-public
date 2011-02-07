#ifndef _itk_DataImageVector3_h_
#define _itk_DataImageVector3_h_

#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkGlobal.h>

#include "itkDataImagePluginExport.h"

class itkDataImageVector3Private;

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageVector3 : public dtkAbstractDataImage
{
    Q_OBJECT
    
public:
    itkDataImageVector3(void);
    ~itkDataImageVector3(void);
    virtual QString description(void) const;
    static bool registered(void);

    virtual QList<QImage> &thumbnails (void) const;

public slots:
    // derived from dtkAbstractData
    
    void *output(void);
    void *data(void);
    void setData(void* data);
    void update(void);

    void onMetaDataSet(const QString& key, const QString& value);
    void onPropertySet(const QString& key, const QString& value);

public:
    // derived from dtkAbstractDataImage

    int xDimension(void);
    int yDimension(void);
    int zDimension(void);
    int tDimension(void);

    int minRangeValue(void);
    int maxRangeValue(void);

    int scalarValueCount(int value);
    int scalarValueMinCount(void);
    int scalarValueMaxCount(void);
    
private:

    itkDataImageVector3Private* d;

};

dtkAbstractData* createItkDataImageVector3 (void);

#endif
