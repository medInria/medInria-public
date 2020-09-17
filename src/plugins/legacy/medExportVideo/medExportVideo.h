#pragma once

#include "medExportVideoPluginExport.h"

#include <medAbstractData.h>
#include <medAbstractProcessLegacy.h>

namespace med
{
class ExportVideoPrivate;

class EXPORTVIDEOPLUGIN_EXPORT ExportVideo : public medAbstractProcessLegacy
{
    Q_OBJECT

public:
    ExportVideo();
    virtual ~ExportVideo();

    virtual QString description() const;
    virtual QString identifier() const;
    static bool registered();

    enum VideoFormat
    {
        OGGVORBIS,
        JPGBATCH,
        FFMPEG
    };

public slots:

    //! Parameters are set through here, channel allows to handle multiple parameters
    void setParameter(int *data, int frame);

    //! Method to actually start the filter
    int update();

    //! The output function is needed in medAbstractProcess even if not used
    medAbstractData *output();

    //! Adapt widgets in FileDialog according to the format type chosen
    void handleWidgetDisplayAccordingToType(int index);

protected:

    //! Export only each slice of the view as JPEG. Useful for home encoding video
    int exportAsJPEG();

    //! Export the current view as a video file
    int exportAsVideo();

    //! Display a File dialog with several video parameters
    int displayFileDialog();

private:
    ExportVideoPrivate *d;
};

dtkAbstractProcess *createExportVideo();
}
