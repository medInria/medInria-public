#pragma once

#include <mscAlgorithmPaintPluginExport.h>

#include <medAbstractData.h>
#include <medDoubleParameter.h>
#include <medImageMaskAnnotationData.h>
#include <medIntParameter.h>
#include <medAbstractSelectableToolBox.h>
#include <medViewEventFilter.h>

#include <itkImage.h>
#include <itkImageRegionIterator.h>

class medAbstractData;
class medAbstractImageView;
class medAnnotationData;

class dtkAbstractProcessFactory;
class medSeedPointAnnotationData;

namespace msc
{

class ClickAndMoveEventFilter;

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

//! Segmentation toolbox to allow manual painting of pixels
class MSCALGORITHMPAINT_EXPORT AlgorithmPaintToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("MUSIC Paint segmentation", "Paint Tool",
                          <<"segmentation")
public:

    typedef QPair<Mask2dType::Pointer,unsigned int> SlicePair;
    typedef QPair<QList<SlicePair>,unsigned char> PairListSlicePlaneId;

    AlgorithmPaintToolBox( QWidget *parent );
    ~AlgorithmPaintToolBox();

    static bool registered();

    inline void setPaintState( PaintState::E value){m_paintState = value;}
    inline PaintState::E paintState(){return m_paintState;}

    bool getSeedPlanted();
    void setSeedPlanted(bool,MaskType::IndexType,unsigned int,double);
    void setSeed(QVector3D);

    inline void setCurrentIdSlice(unsigned int id){currentIdSlice = id;}
    inline unsigned int getCurrentIdSlice(){return currentIdSlice;}
    inline void setCurrentPlaneIndex(unsigned int index){currentPlaneIndex = index;}
    inline unsigned int getCurrentPlaneIndex(){return currentPlaneIndex;}
    void setParameter(int channel, int value);
    void setCurrentView(medAbstractImageView* view);

    bool isData(Mask2dType::Pointer input,unsigned char label);
    Mask2dType::Pointer extract2DImageSlice(MaskType::Pointer input,int plane,int slice,MaskType::SizeType size,MaskType::IndexType start);
    Mask2dFloatType::Pointer computeDistanceMap(Mask2dType::Pointer img);
    void computeIntermediateSlice(Mask2dFloatType::Pointer distanceMapImg0,Mask2dFloatType::Pointer distanceMapImg1,int slice0,
                                                              int slice1,int j,MaskFloatIterator ito,MaskIterator itMask,double *vec);
    void computeCentroid(Mask2dIterator itmask,unsigned int *coord);
    Mask2dType::Pointer translateImageByVec(Mask2dType::Pointer img,int *vec);

    dtkPlugin* plugin();

    medAbstractData* processOutput();

public slots:
    void updateView();

    void activateStroke();
    void activateCustomedCursor();
    void deactivateCustomedCursor();
    void activateMagicWand();
    void updateMagicWandComputationSpeed();

    void copyMetaDataToPaintedData();
    void import();
    void clearMask();

    void setLabel(int newVal);
    void setLabelColor();

    void updateMagicWandComputation();

    void updateStroke(ClickAndMoveEventFilter * filter, medAbstractImageView * view);
    void updateWandRegion(medAbstractImageView * view, QVector3D &vec);
    void updateMouseInteraction();

    void undo();
    void redo();
    void addSliceToStack(medAbstractView * view,const unsigned char planeIndex,QList<int> listIdSlice);
    void onViewClosed();

    void newSeed();
    void removeSeed();

    void copySliceMask();
    void pasteSliceMask();

    void increaseBrushSize();
    void reduceBrushSize();

    void interpolate();

    void behaveWithBodyVisibility();

protected:
    friend class ClickAndMoveEventFilter;

    void addStroke( medAbstractImageView *view, const QVector3D &vec );
    void setData( medAbstractData *data );

    // update with seed point data.
    void updateTableRow(int row);

    void initializeMaskData( medAbstractData * imageData, medAbstractData * maskData );

    void showButtons( bool value);

    void generateLabelColorMap(unsigned int numLabels);

    void updateButtons();

    void addBrushSize(int size);

    char computePlaneIndex(const QVector3D &,MaskType::IndexType & ,bool& isInside);

    void copySliceFromMask3D(itk::Image<unsigned char,2>::Pointer copy,const char planeIndex,const char * direction,const unsigned int slice);
    void pasteSliceToMask3D(itk::Image<unsigned char,2>::Pointer image2D,const char planeIndex,const char * direction,const unsigned int slice);

    void addViewEventFilter(medViewEventFilter * filter );

private:

    void showEvent(QShowEvent * event);

    typedef dtkSmartPointer<medSeedPointAnnotationData> SeedPoint;

    // Stroke's objects
    QPushButton *m_strokeButton;
    medIntParameter* m_brushSizeSlider;
    QShortcut *addBrushSize_shortcut, *reduceBrushSize_shortcut;
    double m_strokeRadius;
    bool maskHasBeenSaved;
    //

    // Magic Wand's objects
    // The slider works on percentages of a linear scale between min and max values, i.e.
    // wandradius = (max - min) * sliderPerc / 2.0
    QPushButton *m_magicWandButton;
    QFormLayout * magicWandLayout;
    medDoubleParameter *m_wandUpperThresholdSlider, *m_wandLowerThresholdSlider;
    QPushButton * m_removeSeedButton;
    QShortcut *removeSeed_shortcut;
    QCheckBox *m_wand3DCheckbox, *m_wand3DRealTime;
    QLabel * m_wandInfo;
    QTime wandTimer;

    bool seedPlanted;
    QVector3D m_seed;

    // Common objects
    QPushButton *m_interpolateButton;
    QPushButton *m_labelColorWidget;
    QSpinBox *m_strokeLabelSpinBox;
    QLabel *m_colorLabel;
    QShortcut *undo_shortcut, *redo_shortcut, *copy_shortcut, *paste_shortcut;

    QPushButton *m_applyButton;

    QPushButton *m_clearMaskButton;

    dtkSmartPointer< ClickAndMoveEventFilter > m_viewFilter;

    dtkSmartPointer<medImageMaskAnnotationData> m_maskAnnotationData;
    //TODO smartPointing have to be managed only in abstraction -rde

    dtkSmartPointer<medAbstractData> m_maskData;
    medAbstractData* m_imageData;

    medImageMaskAnnotationData::ColorMapType m_labelColorMap;

    MaskType::Pointer m_itkMask;

    QPair<Mask2dType::Pointer,char> m_copy;

    // undo_redo_feature's attributes
    QHash<medAbstractView*,QStack<PairListSlicePlaneId>*> * m_undoStacks,*m_redoStacks;
    medAbstractImageView* currentView;
    medAbstractImageView * viewCopied;

    unsigned int currentPlaneIndex; //plane Index of the current/last operation
    unsigned int currentIdSlice; // current slice;
    bool undoRedoCopyPasteModeOn;

    template <typename IMAGE> void RunConnectedFilter (MaskType::IndexType &index, unsigned int planeIndex);
    template <typename IMAGE> void GenerateMinMaxValuesFromImage ();

    QVector3D m_lastVup;
    QVector3D m_lastVpn;
    double m_sampleSpacing[2];

    PaintState::E m_paintState;
};

}
