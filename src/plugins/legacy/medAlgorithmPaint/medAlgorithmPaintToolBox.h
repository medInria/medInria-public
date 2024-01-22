#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAlgorithmPaintPluginExport.h>
#include <medPaintBrush.h>

#include <medAbstractData.h>
#include <medAbstractSelectableToolBox.h>
#include <medDoubleParameterL.h>
#include <medImageMaskAnnotationData.h>
#include <medIntParameterL.h>
#include <medViewEventFilter.h>

#include <itkImage.h>
#include <itkImageRegionIterator.h>

class medAbstractData;
class medAbstractImageView;
class medAnnotationData;

class dtkAbstractProcessFactory;
class medSeedPointAnnotationData;

namespace med
{

class ClickAndMoveEventFilter;

struct PaintBrushObjComparator
{
    bool operator()(medPaintBrush *lhs, medPaintBrush *rhs) const
    {
        return lhs->getIdSlice() < rhs->getIdSlice();
    }
};

struct PaintState
{
    enum E{ None, Wand, Stroke, DeleteStroke };
};

struct MaskPixelValues
{
    enum E{ Unset = 0, Foreground = 1, Background = 2 };
};

typedef itk::Image <unsigned char, 3>              MaskType;
typedef itk::Image <float, 3>                      MaskFloatType;
typedef itk::ImageRegionIterator <MaskType>        MaskIterator;
typedef itk::ImageRegionIterator <MaskFloatType>   MaskFloatIterator;

typedef itk::Image <unsigned char, 2>              Mask2dType;
typedef itk::ImageRegionIterator<Mask2dType>       Mask2dIterator;
typedef itk::Image <float, 2>                      Mask2dFloatType;
typedef itk::ImageRegionIterator <Mask2dFloatType> Mask2dFloatIterator;

/*! \brief Segmentation toolbox to apply manual painting of pixels.
*
* This toolbox has several named widgets which can be accessed in python pipelines:\n\n
* "paintButton" : QPushButton\n
* "Magic Wand" : QPushButton\n
* "Upper Threshold" : medDoubleParameter\n
* "Lower Threshold" : medDoubleParameter\n
* "Activate 3D mode" : QCheckBox\n
* "saveButton" : QPushButton\n
* "clearMaskButton" : QPushButton
*/
class MEDALGORITMPAINT_EXPORT AlgorithmPaintToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Paint Segmentation",
                          "Use paint and region growing tools.",
                          <<"Segmentation")
public:

    typedef std::set<dtkSmartPointer<medPaintBrush>, PaintBrushObjComparator> PaintBrushSet;
    typedef QPair<PaintBrushSet,unsigned char> PairListSlicePlaneId;
    PaintBrushSet setOfPaintBrushRois;

    AlgorithmPaintToolBox( QWidget *parent );
    ~AlgorithmPaintToolBox();

    static bool registered();

    inline void setPaintState( PaintState::E value){m_paintState = value;}
    inline PaintState::E paintState(){return m_paintState;}

    bool getSeedPlanted();
    void setSeedPlanted(bool val, MaskType::IndexType index,
                        unsigned int planeIndex, double value);
    void setSeed(QVector3D point);

    inline void setCurrentIdSlice(unsigned int id){currentIdSlice = id;}
    inline unsigned int getCurrentIdSlice(){return currentIdSlice;}
    inline void setCurrentPlaneIndex(unsigned int index){currentPlaneIndex = index;}
    inline unsigned int getCurrentPlaneIndex(){return currentPlaneIndex;}
    void setParameter(int channel, int value);
    void setCurrentView(medAbstractImageView* view);

    Mask2dType::Pointer extract2DImageSlice(MaskType::Pointer input, int plane, int slice,
                                            MaskType::SizeType size, MaskType::IndexType start);
    Mask2dFloatType::Pointer computeDistanceMap(Mask2dType::Pointer img);
    void computeIntermediateSlice(Mask2dFloatType::Pointer distanceMapImg0, Mask2dFloatType::Pointer distanceMapImg1,
                                  unsigned int slice0, unsigned int slice1, int j,
                                  MaskFloatIterator ito, MaskIterator itMask, double *vec);
    void computeCentroid(Mask2dIterator itmask, unsigned int *coord);
    Mask2dType::Pointer translateImageByVec(Mask2dType::Pointer img, int *vec);

    dtkPlugin* plugin();

    medAbstractData* processOutput();

public slots:
    void updateView();

    void activateStroke(bool checked);
    void activateCustomedCursor();
    void deactivateCustomedCursor();
    void activateMagicWand(bool checked);
    void updateMagicWandComputationSpeed();

    void copyMetaData(medAbstractData *output,
                      medAbstractData *input);
    void import();

    void setLabel(int newVal);
    void setLabelColor();

    void updateMagicWandComputation();

    void updateStroke(ClickAndMoveEventFilter *filter, medAbstractImageView *view);
    void updateWandRegion(medAbstractImageView *view, QVector3D &vec);
    void updateMouseInteraction();

    void undo();
    void redo();
    void addSliceToStack(medAbstractView *view, const unsigned char planeIndex,
                         QList<unsigned int> listIdSlice, bool isMaster = true);

    virtual void clear();
    void clearMask();
    void resetToolbox();

    void newSeed();
    void removeSeed();

    void copySliceMask();
    void pasteSliceMask();

    void increaseBrushSize();
    void reduceBrushSize();

    void interpolate();

protected:
    friend class ClickAndMoveEventFilter;

    void addStroke( medAbstractImageView *view, const QVector3D &vec );
    void setData( medAbstractData *data );

    // update with seed point data.
    void updateTableRow(int row);

    void initializeMaskData( medAbstractData *imageData, medAbstractData *maskData );

    void showButtons( bool value);

    void generateLabelColorMap(unsigned int numLabels);

    void updateButtons();

    void addBrushSize(int size);

    char computePlaneIndex(const QVector3D &, MaskType::IndexType &, bool& isInside);

    void copySliceFromMask3D(itk::Image<unsigned char,2>::Pointer copy, const char planeIndex,
                             const char * direction, const unsigned int slice, bool becomesAMasterOne = true);
    void pasteSliceToMask3D(itk::Image<unsigned char,2>::Pointer image2D, const char planeIndex,
                            const char * direction, const unsigned int slice, bool becomesAMasterOne = true);

    void addViewEventFilter(medViewEventFilter *filter);

    void setButtonsDisabled(bool disable);

private:

    typedef dtkSmartPointer<medSeedPointAnnotationData> SeedPoint;

    // Stroke's objects
    QPushButton *m_strokeButton;
    medIntParameterL *m_brushSizeSlider;
    QShortcut *addBrushSize_shortcut, *reduceBrushSize_shortcut;
    double m_strokeRadius;
    bool maskHasBeenSaved;
    int interpolatedMaskPixelValue;

    // Magic Wand's objects
    // The slider works on percentages of a linear scale between min and max values, i.e.
    // wandradius = (max - min) * sliderPerc / 2.0
    QPushButton *m_magicWandButton;
    QFormLayout *magicWandLayout;
    medDoubleParameterL *m_wandUpperThresholdSlider, *m_wandLowerThresholdSlider;
    QPushButton * m_removeSeedButton;
    QShortcut *removeSeed_shortcut;
    QCheckBox *m_wand3DCheckbox, *m_wand3DRealTime;
    QLabel *m_wandInfo;
    QTime wandTimer;

    bool seedPlanted;
    QVector3D m_seed;

    // Common objects
    QPushButton *m_interpolateButton;
    QPushButton *m_labelColorWidget;
    QSpinBox *m_strokeLabelSpinBox;
    QLabel *m_colorLabel;
    QShortcut *undo_shortcut, *redo_shortcut, *copy_shortcut, *paste_shortcut;

    QPushButton *m_addButton, *m_eraseButton;
    QPushButton *m_applyButton;

    QPushButton *m_clearMaskButton;

    dtkSmartPointer< ClickAndMoveEventFilter > m_viewFilter;

    dtkSmartPointer<medImageMaskAnnotationData> m_maskAnnotationData;

    dtkSmartPointer<medAbstractData> m_maskData;
    medAbstractData* m_imageData;

    medImageMaskAnnotationData::ColorMapType m_labelColorMap;

    MaskType::Pointer m_itkMask;

    QPair<Mask2dType::Pointer,char> m_copy;

    // undo_redo_feature's attributes
    QHash<medAbstractView*,QStack<PairListSlicePlaneId>*> *m_undoStacks,*m_redoStacks;
    medAbstractImageView *currentView;
    medAbstractImageView *viewCopied;

    unsigned int currentPlaneIndex; //plane Index of the current/last operation
    unsigned int currentIdSlice; // current slice;
    bool undoRedoCopyPasteModeOn;
    unsigned int currentSliceLabel;

    template <typename IMAGE> void RunConnectedFilter (MaskType::IndexType &index, unsigned int planeIndex);
    template <typename IMAGE> void GenerateMinMaxValuesFromImage ();

    void interpolateBetween2PaintBrush(unsigned int firstSlice, unsigned int secondSlice);
    void deleteSliceFromMask3D(unsigned int sliceIndex);

    QVector3D m_lastVup;
    QVector3D m_lastVpn;
    double m_sampleSpacing[2];

    PaintState::E m_paintState;

    medIntParameterL *slicingParameter;
    void updateMaskWithMasterLabel();
    bool isMask2dOnSlice();
};

}
