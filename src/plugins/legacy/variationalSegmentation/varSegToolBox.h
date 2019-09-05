/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifndef VarSegToolBox_H
#define VarSegToolBox_H

#include <medAbstractSelectableToolBox.h>
#include <vtkLandmarkSegmentationController.h>

class medAbstractData;
class medAbstractView;
class medAnnotationData;

class dtkAbstractProcessFactory;

namespace mseg
{
class VarSegToolBoxPrivate;

/*!
 * \brief Segmentation toolbox to apply manual painting of pixels.
 */
class VarSegToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Variational Segmentation",
                          "Apply variational segmentation on a dataset",
                          <<"Segmentation")

    VarSegToolBox( QWidget *parent );
    virtual ~VarSegToolBox();

    static medAbstractSelectableToolBox *createInstance( QWidget *parent );

    static QString s_description();
    static QString s_identifier();
    static QString s_name();
    dtkPlugin* plugin();
    medAbstractData *processOutput();
    void endSegmentation();

    template <typename PixelType> typename vtkLandmarkSegmentationController::ImageType::Pointer castImageToType(medAbstractData*);

public slots:
    virtual void updateView();
    void updateLandmarksRenderer(QString key, QString value);
    void addBinaryImage();
    void applyMaskToImage();
    void startSegmentation();
    void manageClosingView();
    void enableButtons(bool isEnabled);
    void segmentation(bool checked);
    virtual void clear();

protected slots:
    void displayOutput();

private:
    VarSegToolBoxPrivate* const d;
};

}

#endif // VarSegToolBox_H
