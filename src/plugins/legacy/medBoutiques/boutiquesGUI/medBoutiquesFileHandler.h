#ifndef MEDBOUTIQUESFILEHANDLER_H
#define MEDBOUTIQUESFILEHANDLER_H

#include "medBoutiquesAbstractFileHandler.h"

#include <QWidget>
#include <QMimeData>
#include <QJsonObject>
#include <QJsonArray>
#include "medBoutiquesConfiguration.h"

QT_BEGIN_NAMESPACE
class medDataIndex;
class medAbstractData;
class medBoutiquesToolBox;
QT_END_NAMESPACE

class medBoutiquesFileHandler : public medBoutiquesAbstractFileHandler
{
private:
    medBoutiquesToolBox *toolbox;
    QJsonObject dataTypeToFormatAndExtension;           // Describes which format to associate with each data type.
                                                        // When converting an input data to a temporary file
                                                        // (BoutiquesGUI saves medInria input data / images to temporary files to feed them to boutiques)
                                                        // the file format will be given by this settings, depending on the data type. When the user sets a parameter as input
                                                        // (by drag-and-dropping data, or with the corresponding `Set input` button),
                                                        // BoutiquesGUI opens a dialog to choose a format from a list of formats compatible with the data type
                                                        // (only if no compatible format is found in `dataTypeToFormatAndExtension` or `preferredFormatsAndExtensions`).
                                                        // This setting is updated when the user chooses a format to associate with the data type.
    QJsonArray preferredFormatsAndExtensions;           // Also used to determine the format of temporary files, but instead of associating each data type to a format,
                                                        // it just enumerates the preferred formats and extensions.
                                                        // If the data type is not found in the `dataTypeToFormatAndExtension` map,
                                                        // BoutiquesGUI will use the first compatible format in the `preferredFormatsAndExtensions` list.
    QJsonArray outputExtensions;                        // Define the extensions of the boutiques tools outputs which can then be opened in medInria.
                                                        // If an output parameter has an extension listed in `outputExtensions`,
                                                        // a button will enable to set the output as *current output* ; it will automatically be opened in medInria once the process is finished.
    QList<QString> temporaryFiles;                        // Keep track of created temporary files to delete them once the process is finished

public:
    medBoutiquesFileHandler(medBoutiquesToolBox *toolbox = nullptr);
    virtual ~medBoutiquesFileHandler();

    // Drag event are accepted (object is droppable) if the dragged object is a file or if the medDataIndex is valid for series
    void checkAcceptDragEvent(QDragEnterEvent *event) override;

    // Create a temporary file from the data described by mimeData / from the current input in medInria
    // the file and extension will be defined from dataTypeToFormatAndExtension, preferredFormatsAndExtensions and user interaction
    QString createTemporaryInputFileForMimeData(const QMimeData *mimeData) override;
    QString createTemporaryInputFileForCurrentInput() override;

    // Delete temporary files once the boutiques tool process is finished
    virtual void deleteTemporaryFiles() override;

    // Check if the file name has a known extension that can be opened in medInria
    // Used to filter which parameter can be set as output (to open them automatically once the process is finished)
    bool hasKnownExtension(const QString &fileName) override;

private:
    // Returns all file formats compatible with the data
    QList<FormatObject> getFileFormatsForData(medAbstractData *data);
    // If the data type is in dataTypeToFormatAndExtension: return the associated format and extension
    // Otherwise: for all formats compatible with the data type: return the first one in preferredFormatsAndExtensions if any, otherwise ask the user to select one
    FormatAndExtension getFormatAndExtensionForData(medAbstractData *data);
    // Ask the user to select a compatible format for the input data
    FormatAndExtension askFormatAndExtensionForData(const QString &dataType, const QList<FormatObject> &fileFormats);
    // Export the given data to a temporary file (to feed the boutiques tool later on)
    QString createTemporaryInputFile(medAbstractData *data, const QString &chosenType, const QString &chosenExtension);
    // Overwrite dataTypeToFormatAndExtension, preferredFormatsAndExtensions and outputExtensions in settings ("boutiques-gui-settings.json")
    void savePreferredFormatSettings();
};

#endif // MEDBOUTIQUESFILEHANDLER_H
