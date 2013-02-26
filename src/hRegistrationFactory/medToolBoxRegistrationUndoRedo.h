#ifndef MEDTOOLBOXREGISTRATIONUNDOREDO_H
#define MEDTOOLBOXREGISTRATIONUNDOREDO_H

#include <itkRegistrationFactory.h>
#include <medToolBox.h>
#include "hRegistrationFactoryExport.h"

class medToolBoxRegistrationUndoRedoPrivate;
    
class HREGISTRATIONFACTORY_EXPORT medToolBoxRegistrationUndoRedo : public medToolBox
{
    Q_OBJECT
    
public:
    typedef float ScalarType;
  ///\todo The orientedimage is really something really interesting to look at very closely...
/*   typedef itk::OrientedImage<ScalarType, 3> ImageType; */
    //typedef itk::Image<ScalarType, 3> ImageType;
    //typedef itk::ImageRegistrationFactory<ImageType> RegistrationFactoryType;
    medToolBoxRegistrationUndoRedo(QWidget *parent = 0);
    ~medToolBoxRegistrationUndoRedo(void);

    //RegistrationFactoryType::Pointer GetFactory(void);
    void RefreshGUIToFactory(void);
    void SetMovingImageFromFactoryOutput (void);
    void SetUpFusionPipeline(bool doreset=false);
    void Render(void);
 
signals:
        
public slots:
    void onUndo(void);
    void onRedo(void);
    void updateTransformationList(void);
    
private:
    medToolBoxRegistrationUndoRedoPrivate *d;
};

#endif

