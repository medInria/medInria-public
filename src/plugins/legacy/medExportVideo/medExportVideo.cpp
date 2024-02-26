/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include "medExportVideo.h"

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkJPEGWriter.h>
#include <vtkOggTheoraWriter.h>
#include <vtkQImageToImageSource.h>
#include <vtkSmartPointer.h>

#ifdef MED_USE_FFmpeg
#include <vtkFFMPEGWriter.h>
#endif

// Qt
#include <QApplication>
#include <QComboBox>
#include <QEventLoop>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QProcess>
#include <QSpinBox>

// /////////////////////////////////////////////////////////////////
// ExportVideoPrivate
// /////////////////////////////////////////////////////////////////

namespace med
{
class ExportVideoPrivate
{
public:
    QVector< QImage > imagesArray;
    int width;
    int height;
    int maximumFrame;

    // GUI
    QFileDialog *exportDialog;
    QComboBox   *formatComboBox;
    QSpinBox    *frameRateSpinBox;
    QLabel      *frameRateLabel;
    QComboBox   *subsamplingComboBox;
    QLabel      *subsamplingLabel;
    QComboBox   *qualityComboBox;
    QLabel      *qualityLabel;
    QLabel      *explanationLabel;

    // User parameters
    QString filename;
    int format;            // Cf. VideoFormat in header file
    int frameRate;         // Frame per second
    bool subsampling;      // Is the video to be encoded using 4:2:0 subsampling?
    int quality;           // 0 = low, 1 = medium, 2 = high
};

// /////////////////////////////////////////////////////////////////
// ExportVideo
// /////////////////////////////////////////////////////////////////

ExportVideo::ExportVideo() : medAbstractProcessLegacy(), d(new ExportVideoPrivate)
{
    d->imagesArray.clear();
    d->width  = 0;
    d->height = 0;
    d->maximumFrame = 0;

    // User parameters
    d->format = OGGVORBIS;
    d->filename = "";
    d->frameRate = 24;
    d->subsampling = false;
    d->quality = 2;
}

ExportVideo::~ExportVideo()
{
    delete d;
}

bool ExportVideo::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("med::ExportVideo", createExportVideo);
}

QString ExportVideo::description() const
{
    return "ExportVideo";
}

QString ExportVideo::identifier() const
{
    return description();
}

void ExportVideo::setParameter(int *data, int frame)
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
        if (displayFileDialog() == medAbstractProcessLegacy::SUCCESS)
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            QApplication::processEvents();
            qDebug() << metaObject()->className() <<" ENCODING... w h "<<d->width<<"/"<<d->height;

            int res = medAbstractProcessLegacy::FAILURE;

            if (d->format == JPGBATCH)
            {
                res = this->exportAsJPEG();
            }
            else
            {
                res = this->exportAsVideo();
            }

            qDebug() << metaObject()->className() <<" END OF ENCODING -- "<<res;
            QApplication::restoreOverrideCursor();

            return res;
        }
    }
    return medAbstractProcessLegacy::FAILURE;
}

int ExportVideo::exportAsJPEG()
{
    // Get the index of images base name
    int lastPoint = d->filename.size();
    if (d->filename.contains("."))
    {
        lastPoint = d->filename.lastIndexOf(".");
    }
    if (d->filename.at(lastPoint-1) == QString::number(0).at(0))
    {
        lastPoint = lastPoint - 1;
    }

    int cpt = 0;
    for (QImage qimage : d->imagesArray)
    {
        // Qt to VTK images (vtkImageData)
        vtkSmartPointer<vtkQImageToImageSource> qimageToImageSource = vtkSmartPointer<vtkQImageToImageSource>::New();
        qimageToImageSource->SetQImage(&qimage);
        qimageToImageSource->Update();

        // Current image filename
        QString name = d->filename.left(lastPoint) + QString::number(cpt)+".jpg";

        vtkSmartPointer<vtkJPEGWriter> writerJPEG = vtkSmartPointer<vtkJPEGWriter>::New();
        writerJPEG->SetFileName(name.toStdString().c_str());
        writerJPEG->SetInputConnection(qimageToImageSource->GetOutputPort());
        writerJPEG->Write();

        cpt++;
    }

    return medAbstractProcessLegacy::SUCCESS;
}

int ExportVideo::exportAsVideo()
{
    vtkSmartPointer<vtkImageCanvasSource2D> source = vtkSmartPointer<vtkImageCanvasSource2D>::New();
    source->SetScalarTypeToUnsignedChar();
    source->SetNumberOfScalarComponents(3);
    source->SetExtent(0, d->width-1, 0, d->height-1, 0, 0);

    vtkSmartPointer<vtkGenericMovieWriter> writerVideo;

    if (d->format == OGGVORBIS)
    {
        vtkSmartPointer<vtkOggTheoraWriter> writerVideoTmp = vtkSmartPointer<vtkOggTheoraWriter>::New();
        writerVideoTmp->SetInputConnection(source->GetOutputPort());
        writerVideoTmp->SetFileName(d->filename.toStdString().c_str());
        writerVideoTmp->SetRate(d->frameRate);
        writerVideoTmp->SetSubsampling(d->subsampling);
        writerVideoTmp->SetQuality(d->quality);

        writerVideo = writerVideoTmp;
    }
#ifdef MED_USE_FFmpeg
    else if (d->format == FFMPEG)
    {
        vtkSmartPointer<vtkFFMPEGWriter> writerVideoTmp = vtkSmartPointer<vtkFFMPEGWriter>::New();
        writerVideoTmp->SetInputConnection(source->GetOutputPort());
        writerVideoTmp->SetFileName(d->filename.toStdString().c_str());
        writerVideoTmp->SetRate(d->frameRate);
        writerVideoTmp->SetQuality(d->quality);
        writerVideo = writerVideoTmp;
    }
#endif
    else
    {
        return medAbstractProcessLegacy::FAILURE;
    }
	
    writerVideo->Start();

    for (QImage qimage : d->imagesArray)
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

    return medAbstractProcessLegacy::SUCCESS;
}

int ExportVideo::displayFileDialog()
{
    d->exportDialog = new QFileDialog(0, tr("Save movie as"));
    d->exportDialog->setOption(QFileDialog::DontUseNativeDialog);
    d->exportDialog->setAcceptMode(QFileDialog::AcceptSave);
    d->exportDialog->selectFile("video.ogv");

    QLayout *layout = d->exportDialog->layout();
    QGridLayout *gridbox = qobject_cast<QGridLayout*>(layout);

    // Hide the filter list
    QWidget *filtersLabel = gridbox->itemAtPosition(gridbox->rowCount()-1, 0)->widget();
    QWidget *filtersList  = gridbox->itemAtPosition(gridbox->rowCount()-1, 1)->widget();
    filtersLabel->hide();
    filtersList->hide();

    // Add user widgets: Video Format
    d->formatComboBox = new QComboBox(d->exportDialog);
    // Free, opensource, no patent.
    // "Before 2007, the .ogg filename extension was used for all files whose content used the Ogg container
    // format. Since 2007, the Xiph.Org Foundation recommends that .ogg only be used for Ogg Vorbis audio files.
    // The Xiph.Org Foundation decided to create a new set of file extensions and media types to describe different
    // types of content such as .oga for audio only files, .ogv for video with or without sound (including Theora),
    // and .ogx for multiplexed Ogg."
    d->formatComboBox->addItem("Ogg Vorbis (.ogv)", OGGVORBIS);
    d->formatComboBox->addItem("JPEG (.jpg .jpeg)", JPGBATCH);
#ifdef MED_USE_FFmpeg
    d->formatComboBox->addItem("FFMPEG (.mp4)", FFMPEG);
#endif
    d->formatComboBox->setCurrentIndex(d->format);
    gridbox->addWidget(new QLabel("Format", d->exportDialog), gridbox->rowCount()-1, 0);
    gridbox->addWidget(d->formatComboBox, gridbox->rowCount()-1, 1);

    connect(d->formatComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleWidgetDisplayAccordingToType(int)),
            Qt::UniqueConnection);

    // --- Only for VIDEO export --- //

    // Add user widgets: Frame Rate
    d->frameRateSpinBox = new QSpinBox(d->exportDialog);
    d->frameRateSpinBox->setValue(d->frameRate);
    d->frameRateLabel = new QLabel("Frame Rate (frame/sec)", d->exportDialog);
    gridbox->addWidget(d->frameRateLabel,   gridbox->rowCount(), 0);
    gridbox->addWidget(d->frameRateSpinBox, gridbox->rowCount()-1, 1);

    // Add user widgets: Subsampling
    d->subsamplingComboBox = new QComboBox(d->exportDialog);
    d->subsamplingComboBox->addItem("Yes", 0);
    d->subsamplingComboBox->addItem("No",  1);
    d->subsamplingComboBox->setCurrentIndex(d->subsampling? 0:1);
    d->subsamplingLabel = new QLabel("Encode using 4:2:0 subsampling", d->exportDialog);
    gridbox->addWidget(d->subsamplingLabel,    gridbox->rowCount()+1, 0);
    gridbox->addWidget(d->subsamplingComboBox, gridbox->rowCount()-1, 1);

    // Add user widgets: Quality
    d->qualityComboBox = new QComboBox(d->exportDialog);
    d->qualityComboBox->addItem("Low",    0);
    d->qualityComboBox->addItem("Medium", 1);
    d->qualityComboBox->addItem("High",   2);
    d->qualityComboBox->setCurrentIndex(d->quality);
    d->qualityLabel = new QLabel("Video quality", d->exportDialog);
    gridbox->addWidget(d->qualityLabel,    gridbox->rowCount()+2, 0);
    gridbox->addWidget(d->qualityComboBox, gridbox->rowCount()-1, 1);

    // Video explanation of conversion (default: ogv txt)
    d->explanationLabel = new QLabel("\nYou can convert OGV to MP4 for instance at https://anyconv.com/fr/convertisseur-de-ogv-en-mp4/ or with your own tools.");
    d->explanationLabel->setWordWrap(true);
    d->explanationLabel->setStyleSheet("font: italic");
    gridbox->addWidget(d->explanationLabel, gridbox->rowCount()+3, 0, 3, 0);
    
    d->exportDialog->setLayout(gridbox);

    if ( d->exportDialog->exec() )
    {
        QString filename = d->exportDialog->selectedFiles().first().toUtf8();
        if (!filename.isEmpty())
        {
            // Get back chosen parameters
            d->filename = filename;
            d->format = d->formatComboBox->itemData(d->formatComboBox->currentIndex()).toInt();
            d->frameRate = d->frameRateSpinBox->value();
            d->subsampling = d->subsamplingComboBox->currentIndex()? false:true;
            d->quality = d->qualityComboBox->currentIndex();

            delete d->exportDialog;
            return medAbstractProcessLegacy::SUCCESS;
        }
    }

    delete d->exportDialog;
    return medAbstractProcessLegacy::FAILURE;
}

void ExportVideo::handleWidgetDisplayAccordingToType(int index)
{
    switch (index)
    {
        case OGGVORBIS: default:
        {
            // Video
            d->exportDialog->selectFile("video.ogv");
            d->frameRateSpinBox->show();
            d->frameRateLabel->show();
            d->subsamplingComboBox->show();
            d->subsamplingLabel->show();
            d->qualityComboBox->show();
            d->qualityLabel->show();
            d->explanationLabel->setText("\nYou can convert OGV to MP4 for instance at https://anyconv.com/fr/convertisseur-de-ogv-en-mp4/ or with your own tools.");
            break;
        }
        case JPGBATCH:
        {
            // JPEG
            d->exportDialog->selectFile("image0.jpg");
            d->frameRateSpinBox->hide();
            d->frameRateLabel->hide();
            d->subsamplingComboBox->hide();
            d->subsamplingLabel->hide();
            d->qualityComboBox->hide();
            d->qualityLabel->hide();
            d->explanationLabel->setText("\n You can convert a batch of jpeg into movies through your own tools.");
            break;
        }
        case FFMPEG:
        {
            // Video
            d->exportDialog->selectFile("video.mp4");
            d->frameRateSpinBox->show();
            d->frameRateLabel->show();
            d->subsamplingComboBox->hide();
            d->subsamplingLabel->hide();
            d->qualityComboBox->show();
            d->qualityLabel->show();
            d->explanationLabel->setText("\nIf needed, you can switch to the OGV export and convert it to MP4 or other formats through your own tools.");
            break;
        }
    }
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createExportVideo()
{
    return new ExportVideo;
}
}
