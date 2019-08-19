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
    QJsonObject dataTypeToFormatAndExtension;
    QJsonArray preferredFormatsAndExtensions;
    QJsonArray outputExtensions;

public:
    medBoutiquesFileHandler(medBoutiquesToolBox *toolbox = nullptr);

    void checkAcceptDragEvent(QDragEnterEvent *event) override;
    QString createTemporaryInputFileForMimeData(const QMimeData *mimeData) override;
    QString createTemporaryInputFileForCurrentInput() override;
    bool hasKnownExtension(const QString &fileName) override;

private:
    QList<FormatObject> getFileFormatsForData(medAbstractData *data);
    FormatAndExtension getFormatAndExtensionForData(medAbstractData *data);
    FormatAndExtension getFormatForInputFile(const QString &dataType, const QList<FormatObject> &fileFormats);
    QString createTemporaryInputFile(medAbstractData *data, const QString &chosenType, const QString &chosenExtension);
    void savePreferredFormatSettings();
};

#endif // MEDBOUTIQUESFILEHANDLER_H
