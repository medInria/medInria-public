#pragma once

#include <dtkCore/dtkSmartPointer.h>
#include <itkImage.h>
#include <medAbstractView.h>
#include <QFrame>
#include <QHBoxLayout>
#include <QVTKWidget.h>
#include <resliceToolBox.h>
#include <vtkImageData.h>
#include <vtkImagePlaneWidget.h>
#include <vtkResliceImageViewer.h>
#include <vtkSmartPointer.h>

class QVTKFrame : public QFrame
{
public:
    QVTKFrame(QWidget * parent):QFrame(parent)
    {
        QHBoxLayout * layout = new QHBoxLayout(this);
        view = new QVTKWidget(this);
        layout->addWidget(view);
        this->setLayout(layout);
    }

    ~QVTKFrame()
    {
        delete view;
    }

    QVTKWidget * getView()
    {
        return view;
    }

private:
    QVTKWidget * view;

};

class medResliceCursorCallback;

class medResliceViewer : public medAbstractView
{

    friend class medResliceCursorCallback;

    Q_OBJECT

public:

    medResliceViewer(medAbstractView * view,QWidget *parent =0);
    ~medResliceViewer();
    void setToolBox(resliceToolBox *);

    virtual QString description() const;

    virtual medViewBackend * backend() const;

    virtual QWidget* navigatorWidget();
    virtual QWidget *viewWidget();
    virtual QWidget *mouseInteractionWidget();

public slots:

    virtual void thickMode(int);
    virtual void blendMode(int);
    virtual void SetBlendModeToMaxIP();
    virtual void SetBlendModeToMinIP();
    virtual void SetBlendModeToMeanIP();
    virtual void SetBlendMode(int);
    virtual void reset();
    virtual void resetViews();
    virtual void render();

    void saveImage();
    void thickSlabChanged(double);
    void extentChanged(int);
    bool eventFilter(QObject * object,QEvent * event);

    vtkResliceImageViewer* getResliceImageViewer(int i);
    vtkImagePlaneWidget* getImagePlaneWidget(int i);
    dtkSmartPointer<medAbstractData> getOutput();

    virtual void update(){}

signals:
    void imageReformatedGenerated();

protected:
    vtkSmartPointer<vtkResliceImageViewer> riw[3];
    vtkSmartPointer<vtkImagePlaneWidget> planeWidget[3];
    double planeNormal[3][3];
    QWidget * viewBody;
    QVTKWidget * views[4];
    QVTKFrame * frames[4];
    dtkSmartPointer<medAbstractData> inputData;
    double outputSpacing[3];
    unsigned char selectedView;
    vtkSmartPointer<vtkImageData> vtkViewData;
    dtkSmartPointer<medAbstractData> outputData;
    int fromSlice,toSlice;
    resliceToolBox * reformaTlbx;

    void applyRadiologicalConvention();
    void calculateResliceMatrix(vtkMatrix4x4* result);
    void adjustResliceMatrixToViewUp(vtkMatrix4x4* resliceMatrix);
    void updatePlaneNormals();
    void ensureOrthogonalPlanes();
    int findMovingPlaneIndex();
    void makePlaneOrthogonalToOtherPlanes(vtkPlane* targetPlane, vtkPlane* plane1, vtkPlane* plane2);

    template <typename DATA_TYPE>
    void generateOutput(vtkImageReslice* reslicer, QString destType);

    void applyResamplingPix();

    template <typename DATA_TYPE>
    void compensateForRadiologicalView(itk::Image<DATA_TYPE, 3>* image);

    template <typename DATA_TYPE>
    void correctOutputTransform(itk::Image<DATA_TYPE, 3>* outputImage, vtkMatrix4x4* resliceMatrix);

    template <typename DATA_TYPE>
    void getImageCenterInLocalSpace(itk::Image<DATA_TYPE, 3>* image, double center[3]);

    vtkSmartPointer<vtkMatrix4x4> getResliceRotationMatrix(vtkMatrix4x4* resliceMatrix);

private:

    virtual QImage buildThumbnail(const QSize &size);

};
