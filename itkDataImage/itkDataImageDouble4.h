#ifndef _itk_DataImageDouble4_h_
#define _itk_DataImageDouble4_h_

#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkGlobal.h>

#include "itkDataImagePluginExport.h"

class itkDataImageDouble4Private;

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageDouble4 : public dtkAbstractDataImage
{
    Q_OBJECT

public:
    itkDataImageDouble4(void);
    ~itkDataImageDouble4(void);
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
    void computeRange(void);
    void computeValueCounts(void);

    itkDataImageDouble4Private* d;

};

dtkAbstractData* createItkDataImageDouble4 (void);

#endif
