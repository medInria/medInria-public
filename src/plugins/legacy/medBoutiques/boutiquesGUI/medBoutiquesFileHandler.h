#ifndef MEDBOUTIQUESFILEHANDLER_H
#define MEDBOUTIQUESFILEHANDLER_H

#include "medBoutiquesAbstractFileHandler.h"
#include "../medBoutiquesToolBox.h"

QT_BEGIN_NAMESPACE
class medDataIndex;
class medAbstractData;
QT_END_NAMESPACE

#define PREFERRED_FORMATS_SETTINGS_PATH "../preferredFormats.json"

class medBoutiquesFileHandler : public medBoutiquesAbstractFileHandler
{
private:
    medBoutiquesToolBox *toolbox;
    QJsonObject dataTypeToFormatAndExtension;
    QJsonArray preferredFormatsAndExtensions;

public:
    medBoutiquesFileHandler(medBoutiquesToolBox *toolbox);

    void checkAcceptDragEvent(QDragEnterEvent *event) override;
    QString createTemporaryInputFileForMimeData(const QMimeData *mimeData) override;
    QString createTemporaryInputFileForCurrentInput() override;

private:
    QList<FormatObject> getFileFormatsForData(medAbstractData *data);
    FormatAndExtension getFormatAndExtensionForData(medAbstractData *data);
    FormatAndExtension getFormatForInputFile(const QString &dataType, const QList<FormatObject> &fileFormats);
    QString createTemporaryInputFile(medAbstractData *data, const QString &chosenType, const QString &chosenExtension);
    void savePreferredFormatSettings();
};

#endif // MEDBOUTIQUESFILEHANDLER_H
