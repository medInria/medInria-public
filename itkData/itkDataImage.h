#ifndef _itk_DataImage_h_
#define _itk_DataImage_h_

#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkGlobal.h>

#include "itkDataPluginExport.h"


class itkDataImageChar3Private;

class ITKDATAPLUGIN_EXPORT itkDataImageChar3 : public dtkAbstractDataImage
{
  Q_OBJECT
    
 public:
    itkDataImageChar3(void);
    ~itkDataImageChar3(void);
    virtual QString description(void) const;
    static bool registered(void);

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

    itkDataImageChar3Private* d;
  
};

dtkAbstractData* createItkDataImageChar3 (void);





class itkDataImageUChar3Private;

class ITKDATAPLUGIN_EXPORT itkDataImageUChar3 : public dtkAbstractDataImage
{
  Q_OBJECT
    
 public:
    itkDataImageUChar3(void);
    ~itkDataImageUChar3(void);
    virtual QString description(void) const;
    static bool registered(void);

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

    itkDataImageUChar3Private* d;
  
};

dtkAbstractData* createItkDataImageUChar3 (void);




class itkDataImageShort3Private;

class ITKDATAPLUGIN_EXPORT itkDataImageShort3 : public dtkAbstractDataImage
{
  Q_OBJECT
    
 public:
    itkDataImageShort3(void);
    ~itkDataImageShort3(void);
    virtual QString description(void) const;
    static bool registered(void);

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

    itkDataImageShort3Private* d;
  
};

dtkAbstractData* createItkDataImageShort3 (void);



class itkDataImageUShort3Private;

class ITKDATAPLUGIN_EXPORT itkDataImageUShort3 : public dtkAbstractDataImage
{
  Q_OBJECT
    
 public:
    itkDataImageUShort3(void);
    ~itkDataImageUShort3(void);
    virtual QString description(void) const;
    static bool registered(void);

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

    itkDataImageUShort3Private* d;
  
};

dtkAbstractData* createItkDataImageUShort3 (void);



class itkDataImageInt3Private;

class ITKDATAPLUGIN_EXPORT itkDataImageInt3 : public dtkAbstractDataImage
{
  Q_OBJECT
    
 public:
    itkDataImageInt3(void);
    ~itkDataImageInt3(void);
    virtual QString description(void) const;
    static bool registered(void);

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

    itkDataImageInt3Private* d;
  
};

dtkAbstractData* createItkDataImageInt3 (void);



class itkDataImageUInt3Private;

class ITKDATAPLUGIN_EXPORT itkDataImageUInt3 : public dtkAbstractDataImage
{
  Q_OBJECT
    
 public:
    itkDataImageUInt3(void);
    ~itkDataImageUInt3(void);
    virtual QString description(void) const;
    static bool registered(void);

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

    itkDataImageUInt3Private* d;
  
};

dtkAbstractData* createItkDataImageUInt3 (void);



class itkDataImageLong3Private;

class ITKDATAPLUGIN_EXPORT itkDataImageLong3 : public dtkAbstractDataImage
{
  Q_OBJECT
    
 public:
    itkDataImageLong3(void);
    ~itkDataImageLong3(void);
    virtual QString description(void) const;
    static bool registered(void);

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

    itkDataImageLong3Private* d;
  
};

dtkAbstractData* createItkDataImageLong3 (void);



class itkDataImageULong3Private;

class ITKDATAPLUGIN_EXPORT itkDataImageULong3 : public dtkAbstractDataImage
{
  Q_OBJECT
    
 public:
    itkDataImageULong3(void);
    ~itkDataImageULong3(void);
    virtual QString description(void) const;
    static bool registered(void);

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

    itkDataImageULong3Private* d;
  
};

dtkAbstractData* createItkDataImageULong3 (void);




class itkDataImageFloat3Private;

class ITKDATAPLUGIN_EXPORT itkDataImageFloat3 : public dtkAbstractDataImage
{
  Q_OBJECT
    
 public:
    itkDataImageFloat3(void);
    ~itkDataImageFloat3(void);
    virtual QString description(void) const;
    static bool registered(void);

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




class itkDataImageDouble3Private;

class ITKDATAPLUGIN_EXPORT itkDataImageDouble3 : public dtkAbstractDataImage
{
  Q_OBJECT
    
 public:
    itkDataImageDouble3(void);
    ~itkDataImageDouble3(void);
    virtual QString description(void) const;
    static bool registered(void);

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
