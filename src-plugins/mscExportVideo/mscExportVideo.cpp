#include "mscExportVideo.h"

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkJPEGWriter.h>
#include <vtkOggTheoraWriter.h>
#include <vtkQImageToImageSource.h>
#include <vtkSmartPointer.h>

// /////////////////////////////////////////////////////////////////
// ExportVideoPrivate
// /////////////////////////////////////////////////////////////////
namespace msc
{
class ExportVideoPrivate
{
public:
    QVector< QImage > imagesArray;
    int width;
    int height;
    int maximumFrame;

    // User parameters
    QString filename;
    int format;            // Cf. VideoFormat in header file
    int frameRate;         // Frame per second
    bool subsampling;      // Is the video to be encoded using 4:2:0 subsampling?
    int quality;           // 0 = low, 1 = medium, 2 = high
    bool exportOnlyAsJPEG; // Can be used for home encoding
};

// /////////////////////////////////////////////////////////////////
// ExportVideo
// /////////////////////////////////////////////////////////////////

ExportVideo::ExportVideo() : medAbstractProcess(), d(new ExportVideoPrivate)
{
    d->imagesArray.clear();
    d->width  = 0;
    d->height = 0;
    d->maximumFrame = 0;

    // User parameters
    d->format = OGGVORBIS;
    d->filename = "";
    d->frameRate = 1;
    d->subsampling = false;
    d->quality = 2;
    d->exportOnlyAsJPEG = false;
}

ExportVideo::~ExportVideo()
{
    delete d;
}

bool ExportVideo::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("msc::ExportVideo", createExportVideo);
}

QString ExportVideo::description() const
{
    return "ExportVideo";
}

QString ExportVideo::identifier() const
{
    return description();
}

void ExportVideo::setParameter(int* data, int frame)
{
    d->width  = data[0];
    d->height = data[1];

    QImage currentImage = QPixmap(d->width, d->height).toImage();

    for (int i=0; i<d->width; ++i)
    {
        for (int j=0; j<d->height; ++j)
        {
            // R, G, B to QColor (A default at 255)
            int index1D = 2 + (i * d->height + j)*3;
            QColor color = QColor(data[index1D    ],
                                  data[index1D + 1],
                                  data[index1D + 2]);

            currentImage.setPixel(i, j, color.rgb());
        }
    }

    int arraySize = d->imagesArray.size();

    while (arraySize <= frame)
    {
       d->imagesArray.resize(1 + 2 * arraySize); // it's common to double here to avoid N resizes when giving N frames in order
       arraySize = d->imagesArray.size();
    }

    d->imagesArray[frame] = currentImage;
    d->maximumFrame = std::max(d->maximumFrame, frame);
}

medAbstractData* ExportVideo::output()
{
    return nullptr;
}

int ExportVideo::update()
{
    if (d->imagesArray.count() >= 2)
    {
        // Handle error of missing frame: ignore the frame by removing it
        d->imagesArray.resize(d->maximumFrame);
        int cpt = 0;
        foreach (QImage image, d->imagesArray)
        {
            if (image.isNull())
            {
                d->imagesArray.remove(cpt);
            }
            else
            {
                cpt++;
            }
        }

        // Run export video/jpeg
        if (displayFileDialog() == medAbstractProcess::SUCCESS)
        {
            qDebug()<<"### ExportVideo::update ENCODING... w h "<<d->width<<"/"<<d->height;

            int res = medAbstractProcess::FAILURE;

            if (d->exportOnlyAsJPEG)
            {
                res = this->exportAsJPEG();
            }
            else
            {
                res = this->exportAsVideo();
            }

            qDebug()<<"### ExportVideo::update END OF ENCODING";

            return res;
        }
    }
    return medAbstractProcess::FAILURE;
}

int ExportVideo::exportAsJPEG()
{
    int cpt = 0;
    foreach (QImage qimage, d->imagesArray)
    {
        // Qt to VTK images (vtkImageData)
        vtkSmartPointer<vtkQImageToImageSource> qimageToImageSource = vtkSmartPointer<vtkQImageToImageSource>::New();
        qimageToImageSource->SetQImage(&qimage);
        qimageToImageSource->Update();
        vtkImageData* vtkImage = qimageToImageSource->GetOutput();

        int lastPoint = d->filename.lastIndexOf(".");
        QString name = d->filename.left(lastPoint) + QString::number(cpt)+".jpg";

        vtkSmartPointer<vtkJPEGWriter> writerJPEG = vtkSmartPointer<vtkJPEGWriter>::New();
        writerJPEG->SetFileName(name.toStdString().c_str());
        writerJPEG->SetInputConnection(vtkImage->GetProducerPort());
        writerJPEG->Write();

        cpt++;
    }

    return medAbstractProcess::SUCCESS;
}

int ExportVideo::exportAsVideo()
{
    vtkSmartPointer<vtkImageCanvasSource2D> source = vtkSmartPointer<vtkImageCanvasSource2D>::New();
    source->SetScalarTypeToUnsignedChar();
    source->SetNumberOfScalarComponents(3);
    source->SetExtent(0, d->width-1, 0, d->height-1, 0, 0);

    if (d->format == OGGVORBIS)
    {
        vtkSmartPointer<vtkOggTheoraWriter> writerVideo = vtkSmartPointer<vtkOggTheoraWriter>::New();
        writerVideo->SetInputConnection(source->GetOutputPort());
        writerVideo->SetFileName(d->filename.toStdString().c_str());
        writerVideo->SetRate(d->frameRate);
        writerVideo->SetSubsampling(d->subsampling); // OGG/OGV parameter
        writerVideo->SetQuality(d->quality);
        writerVideo->Start();

        foreach (QImage qimage, d->imagesArray)
        {
            // Qt to VTK images (vtkImageData)
            vtkSmartPointer<vtkQImageToImageSource> qimageToImageSource = vtkSmartPointer<vtkQImageToImageSource>::New();
            qimageToImageSource->SetQImage(&qimage);
            qimageToImageSource->Update();
            vtkImageData* vtkImage = qimageToImageSource->GetOutput();

            source->DrawImage(0, 0, vtkImage);
            source->Update();

            writerVideo->Write();
        }

        writerVideo->End();

        return medAbstractProcess::SUCCESS;
    }
    return medAbstractProcess::FAILURE;
}

int ExportVideo::displayFileDialog()
{
    QFileDialog * exportDialog = new QFileDialog(0, tr("Save movie as"));
    exportDialog->setOption(QFileDialog::DontUseNativeDialog);
    exportDialog->setAcceptMode(QFileDialog::AcceptSave);
    exportDialog->selectFile("video.ogv");

    QLayout* layout = exportDialog->layout();
    QGridLayout* gridbox = qobject_cast<QGridLayout*>(layout);

    // Hide the filter list
    QWidget * filtersLabel = gridbox->itemAtPosition(gridbox->rowCount()-1, 0)->widget();
    QWidget * filtersList = gridbox->itemAtPosition(gridbox->rowCount()-1, 1)->widget();
    filtersLabel->hide(); filtersList->hide();

    // Add user widgets: Video Format
    QComboBox* format = new QComboBox(exportDialog);
    // Free, opensource, no patent.
    // "Before 2007, the .ogg filename extension was used for all files whose content used the Ogg container
    // format. Since 2007, the Xiph.Org Foundation recommends that .ogg only be used for Ogg Vorbis audio files.
    // The Xiph.Org Foundation decided to create a new set of file extensions and media types to describe different
    // types of content such as .oga for audio only files, .ogv for video with or without sound (including Theora),
    // and .ogx for multiplexed Ogg."
    format->addItem("Ogg Vorbis (.ogv)", 0);
    format->setCurrentIndex(d->format);
    gridbox->addWidget(new QLabel("Format", exportDialog), gridbox->rowCount()-1, 0);
    gridbox->addWidget(format, gridbox->rowCount()-1, 1);

    // Add user widgets: Frame Rate
    QSpinBox* frameRate = new QSpinBox(exportDialog);
    frameRate->setValue(d->frameRate);
    gridbox->addWidget(new QLabel("Frame Rate (frame/sec)", exportDialog), gridbox->rowCount(), 0);
    gridbox->addWidget(frameRate, gridbox->rowCount()-1, 1);

    // Add user widgets: Subsampling
    QComboBox *subsampling = new QComboBox(exportDialog);
    subsampling->addItem("Yes", 0);
    subsampling->addItem("No",  1);
    subsampling->setCurrentIndex(d->subsampling? 0:1);
    gridbox->addWidget(new QLabel("Encode using 4:2:0 subsampling", exportDialog), gridbox->rowCount()+1, 0);
    gridbox->addWidget(subsampling, gridbox->rowCount()-1, 1);

    // Add user widgets: Quality
    QComboBox* quality = new QComboBox(exportDialog);
    quality->addItem("Low",    0);
    quality->addItem("Medium", 1);
    quality->addItem("High",   2);
    quality->setCurrentIndex(d->quality);
    gridbox->addWidget(new QLabel("Video quality", exportDialog), gridbox->rowCount()+2, 0);
    gridbox->addWidget(quality, gridbox->rowCount()-1, 1);

    // Add user widgets: JPEG export
    QComboBox *jpeg = new QComboBox(exportDialog);
    jpeg->addItem("Yes", 0);
    jpeg->addItem("No",  1);
    jpeg->setCurrentIndex(d->exportOnlyAsJPEG? 0:1);
    gridbox->addWidget(new QLabel("Only export frames as JPEG", exportDialog), gridbox->rowCount()+3, 0);
    gridbox->addWidget(jpeg, gridbox->rowCount()-1, 1);

    exportDialog->setLayout(gridbox);

    if ( exportDialog->exec() )
    {
        QString filename = exportDialog->selectedFiles().first().toUtf8();
        if (!filename.isEmpty())
        {
            // Get back chosen parameters
            d->filename = filename;
            d->format = format->currentIndex();
            d->frameRate = frameRate->value();
            d->subsampling = subsampling->currentIndex()? false:true;
            d->quality = quality->currentIndex();
            d->exportOnlyAsJPEG = jpeg->currentIndex()? false:true;

            delete exportDialog;
            return medAbstractProcess::SUCCESS;
        }
    }

    delete exportDialog;
    return medAbstractProcess::FAILURE;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createExportVideo()
{
    return new ExportVideo;
}
}
