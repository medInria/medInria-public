#include "medPipelineTypes.h"

#include <stdexcept>

#include <QVariant>
#include <QVector>

#include "medPipelineChooserToolBox.h"
#include "medPipelineControlToolBox.h"
#include "medPipelineRoiToolBox.h"
#include "medPipelineValuesToolBox.h"

namespace med::pipeline
{

QString PAINT_TOOLBOX = "med::AlgorithmPaintToolBox";
QString CROP_TOOLBOX = "medCropToolBox";
QString EXTRACTION_4D_TO_3D_TOOLBOX = "msc::Extraction4Dto3DToolBox";
QString HISTOGRAM_ANALYSIS_TOOLBOX = "msc::HistogramAnalysisToolBox";
QString ITK_FILTERS_TOOLBOX = "itkFiltersToolBox";
QString ITK_MORPHOLOGICAL_FILTERS_TOOLBOX = "itkMorphologicalFiltersToolBox";
QString LAYERS_TOOLBOX = "layersToolBox";
QString LEVEL_SET_SEGMENTATION_TOOLBOX = "msc::LevelSetSegmentationToolBox";
QString MANUAL_REGISTRATION_TOOLBOX = "manualRegistrationToolBox";
QString MERGE_VOLUMES_TOOLBOX = "msc::MergeVolumesToolBox";
QString MOUSE_INTERACTION_TOOLBOX = "mouseInteractionToolBox";
QString PIPELINE_CHOOSER_TOOLBOX = med::pipeline::PipelineChooserToolBox::staticIdentifier();
QString PIPELINE_CONTROL_TOOLBOX = med::pipeline::PipelineControlToolBox::staticIdentifier();
QString POLYGON_ROI_TOOLBOX = "polygonRoiToolBox";
QString PROGRESSION_STACK_TOOLBOX = "progressionStackToolBox";
QString RESLICE_TOOLBOX = "resliceToolBox";
QString ROI_TOOLBOX = med::pipeline::RoiToolBox::staticIdentifier();
QString VALUES_TOOLBOX = med::pipeline::ValuesToolBox::staticIdentifier();
QString VESSEL_SEGMENTATION_TOOLBOX = "mscVesselSegmentationToolBox";
QString VIEW_SETTINGS_TOOLBOX = "viewSettingsToolBox";
QString VOI_CUTTER_TOOLBOX = "voiCutterToolBox";

const char* RenderingParameters::SURFACE = "Surface";
const char* RenderingParameters::WIREFRAME = "Wireframe";
const char* RenderingParameters::POINTS = "Points";

RenderingParameters::RenderingParameters(QString renderingMode, QString color, double opacity, QString attribute,
                                         double minRange, double maxRange)
    : renderingMode(renderingMode), color(color), attribute(attribute), opacity(opacity), minRange(minRange), maxRange(maxRange)
{
}

void writeQVariantContent(QXmlStreamWriter& out, const QVariant& variant)
{
    if (variant.canConvert<medDataIndex>())
    {
        writeDataIndexContent(out, variant.value<medDataIndex>());
    }
    else if (variant.canConvert<QString>())
    {
        out.writeCharacters(variant.toString());
    }
    else if (variant.canConvert<QList<QString> >())
    {
        writeQStringListContent(out, variant.value<QList<QString> >());
    }
    else if (variant.canConvert<QStringList>())
    {
        writeQStringListContent(out, variant.value<QStringList>());
    }
    else if (variant.canConvert<QList<bool> >())
    {
        writeNumericQListContent(out, variant.value<QList<bool> >());
    }
    else if (variant.canConvert<QList<char> >())
    {
        writeNumericQListContent(out, variant.value<QList<char> >());
    }
    else if (variant.canConvert<QList<unsigned char> >())
    {
        writeNumericQListContent(out, variant.value<QList<unsigned char> >());
    }
    else if (variant.canConvert<QList<short> >())
    {
        writeNumericQListContent(out, variant.value<QList<short> >());
    }
    else if (variant.canConvert<QList<unsigned short> >())
    {
        writeNumericQListContent(out, variant.value<QList<unsigned short> >());
    }
    else if (variant.canConvert<QList<int> >())
    {
        writeNumericQListContent(out, variant.value<QList<int> >());
    }
    else if (variant.canConvert<QList<unsigned int> >())
    {
        writeNumericQListContent(out, variant.value<QList<unsigned int> >());
    }
    else if (variant.canConvert<QList<long> >())
    {
        writeNumericQListContent(out, variant.value<QList<long> >());
    }
    else if (variant.canConvert<QList<unsigned long> >())
    {
        writeNumericQListContent(out, variant.value<QList<unsigned long> >());
    }
    else if (variant.canConvert<QList<long long> >())
    {
        writeNumericQListContent(out, variant.value<QList<long long> >());
    }
    else if (variant.canConvert<QList<unsigned long long> >())
    {
        writeNumericQListContent(out, variant.value<QList<unsigned long long> >());
    }
    else if (variant.canConvert<QList<float> >())
    {
        writeNumericQListContent(out, variant.value<QList<float> >());
    }
    else if (variant.canConvert<QList<double> >())
    {
        writeNumericQListContent(out, variant.value<QList<double> >());
    }
    else if (variant.canConvert<med::pipeline::QStringToQStringMap>())
    {
        writeQHashContent(out, variant.value<med::pipeline::QStringToQStringMap>());
    }
    else if (variant.canConvert<QVariantHash>())
    {
        writeQHashContent(out, variant.toHash());
    }
    else if (variant.canConvert<med::pipeline::RenderingParameters>())
    {
        writeRenderingParametersContent(out, variant.value<med::pipeline::RenderingParameters>());
    }
    else
    {
        QString message;
        message += "QVariant type not supported: ";
        message += variant.typeName();
        throw std::runtime_error(message.toStdString().c_str());
    }
}

QVariant readQVariantContent(QXmlStreamReader& in, QString typeName)
{
    if (typeName == "medDataIndex")
    {
        return QVariant::fromValue(readDataIndexContent(in));
    }
    else if (typeName == "QList<QString>" || typeName == "QStringList")
    {
        return QVariant::fromValue(readQStringListContent(in));
    }
    else if (typeName == "QList<bool>")
    {
        return QVariant::fromValue(readNumericQListContent<bool>(in));
    }
    else if (typeName == "QList<char>")
    {
        return QVariant::fromValue(readNumericQListContent<char>(in));
    }
    else if (typeName == "QList<unsigned char>")
    {
        return QVariant::fromValue(readNumericQListContent<unsigned char>(in));
    }
    else if (typeName == "QList<short>")
    {
        return QVariant::fromValue(readNumericQListContent<short>(in));
    }
    else if (typeName == "QList<unsigned short>")
    {
        return QVariant::fromValue(readNumericQListContent<unsigned short>(in));
    }
    else if (typeName == "QList<int>")
    {
        return QVariant::fromValue(readNumericQListContent<int>(in));
    }
    else if (typeName == "QList<unsigned int>")
    {
        return QVariant::fromValue(readNumericQListContent<unsigned int>(in));
    }
    else if (typeName == "QList<long>")
    {
        return QVariant::fromValue(readNumericQListContent<long>(in));
    }
    else if (typeName == "QList<unsigned long>")
    {
        return QVariant::fromValue(readNumericQListContent<unsigned long>(in));
    }
    else if (typeName == "QList<long long>")
    {
        return QVariant::fromValue(readNumericQListContent<long long>(in));
    }
    else if (typeName == "QList<unsigned long long>")
    {
        return QVariant::fromValue(readNumericQListContent<unsigned long long>(in));
    }
    else if (typeName == "QList<float>")
    {
        return QVariant::fromValue(readNumericQListContent<float>(in));
    }
    else if (typeName == "QList<double>")
    {
        return QVariant::fromValue(readNumericQListContent<double>(in));
    }
    else if ((typeName == "med::pipeline::QStringToQStringMap")
             || (typeName == "medPipeline::QStringToQStringMap"))
    {
        return QVariant::fromValue(readQHashContent<QString>(in));
    }
    else if (typeName == "QVariantHash")
    {
        return QVariant::fromValue(readQHashContent<QVariant>(in));
    }
    else if ((typeName == "med::pipeline::RenderingParameters")
             || (typeName == "medPipeline::RenderingParameters"))
    {
        return QVariant::fromValue(readRenderingParametersContent(in));
    }
    else if (typeName == "bool"
             || typeName == "char"
             || typeName == "unsigned char"
             || typeName == "short"
             || typeName == "unsigned short"
             || typeName == "int"
             || typeName == "unsigned int"
             || typeName == "long"
             || typeName == "unsigned long"
             || typeName == "long long"
             || typeName == "unsigned long long"
             || typeName == "float"
             || typeName == "double")
    {
        QVariant result = QVariant(in.readElementText());
        result.convert(QVariant::nameToType(typeName.toStdString().c_str()));
        return result;
    }
    else
    {
        QString message;
        message += "Unrecognized XML element";
        throw std::runtime_error(message.toStdString().c_str());
    }
}

void writeDataIndexContent(QXmlStreamWriter& out, const medDataIndex& index)
{
    out.writeTextElement("dataSourceId", QString::number(index.dataSourceId()));
    out.writeTextElement("patientId", QString::number(index.patientId()));
    out.writeTextElement("studyId", QString::number(index.studyId()));
    out.writeTextElement("seriesId", QString::number(index.seriesId()));
}

medDataIndex readDataIndexContent(QXmlStreamReader& in)
{
    int dataSourceId, patientId, studyId, seriesId;

    while (in.readNextStartElement())
    {
        if (in.name() == "dataSourceId")
        {
            dataSourceId = in.readElementText().toInt();
        }
        else if (in.name() == "patientId")
        {
            patientId = in.readElementText().toInt();
        }
        else if (in.name() == "studyId")
        {
            studyId = in.readElementText().toInt();
        }
        else if (in.name() == "seriesId")
        {
            seriesId = in.readElementText().toInt();
        }
        else if (in.name() == "imageId")
        {
            // for retro-compatibility
            in.readElementText();
        }
        else
        {
            throwXMLException(in);
        }
    }

    return medDataIndex(dataSourceId, patientId, studyId, seriesId);
}

template<typename TYPE>
void writeNumericQListContent(QXmlStreamWriter& out, QList<TYPE> list)
{
    QStringList stringList;

    foreach (TYPE item, list)
    {
        stringList.append(QString::number(item));
    }

    writeQStringListContent(out, stringList);
}

template<typename TYPE>
QList<TYPE> readNumericQListContent(QXmlStreamReader& in)
{
    QList<TYPE> result;

    foreach (QString item, readQStringListContent(in))
    {
        result.append((QVariant(item).value<TYPE>()));
    }

    return result;
}

void writeQStringListContent(QXmlStreamWriter& out, QStringList list)
{
    foreach (QString item, list)
    {
        out.writeTextElement("item", item);
    }
}

QStringList readQStringListContent(QXmlStreamReader& in)
{
    QStringList result;

    while (in.readNextStartElement())
    {
        if (in.name() == "item")
        {
            result.append(in.readElementText());
        }
        else
        {
            throwXMLException(in);
        }
    }

    return result;
}

template<typename VALUE_TYPE>
void writeQHashContent(QXmlStreamWriter& out, QHash<QString, VALUE_TYPE> hashTable)
{
    foreach (QString key, hashTable.keys())
    {
        out.writeStartElement("item");
        out.writeAttribute("key", key);
        QVariant valueAsVariant = QVariant::fromValue<VALUE_TYPE>(hashTable[key]);
        writeQVariantContent(out, valueAsVariant);
        out.writeEndElement();
    }
}

template<typename VALUE_TYPE>
QHash<QString, VALUE_TYPE> readQHashContent(QXmlStreamReader& in)
{
    QHash<QString, VALUE_TYPE> result;

    while (in.readNextStartElement())
    {
        if (in.name() == "item")
        {
            QString key = in.attributes().value("key").toString();
            QVariant valueAsVariant = in.readElementText();
            result[key] = valueAsVariant.value<VALUE_TYPE>();
        }
        else
        {
            throwXMLException(in);
        }
    }

    return result;
}

void writeRenderingParametersContent(QXmlStreamWriter& out, const RenderingParameters& parameters)
{
    out.writeTextElement("mode", parameters.renderingMode);
    out.writeTextElement("color", parameters.color);
    out.writeTextElement("attribute", parameters.attribute);
    out.writeTextElement("opacity", QString::number(parameters.opacity));
    out.writeTextElement("minRange", QString::number(parameters.minRange));
    out.writeTextElement("maxRange", QString::number(parameters.maxRange));
}

RenderingParameters readRenderingParametersContent(QXmlStreamReader& in)
{
    RenderingParameters result;

    while (in.readNextStartElement())
    {
        if (in.name() == "mode")
        {
            result.renderingMode = in.readElementText();
        }
        else if (in.name() == "color")
        {
            result.color = in.readElementText();
        }
        else if (in.name() == "attribute")
        {
            result.attribute = in.readElementText();
        }
        else if (in.name() == "opacity")
        {
            result.opacity = in.readElementText().toDouble();
        }
        else if (in.name() == "minRange")
        {
            result.minRange = in.readElementText().toDouble();
        }
        else if (in.name() == "maxRange")
        {
            result.maxRange = in.readElementText().toDouble();
        }
        else
        {
            throwXMLException(in);
        }
    }

    return result;
}

bool hasXMLError(QXmlStreamReader& stream)
{
    return (stream.error() == QXmlStreamReader::NoError);
}

void throwXMLException(QXmlStreamReader& stream)
{
    QString errorString;

    if (hasXMLError(stream))
    {
        errorString = stream.errorString();
    }
    else
    {
        errorString = "Unexpected error";
    }

    QString message;
    message = QString("Line %1: %2")
              .arg(stream.lineNumber())
              .arg(errorString);
    throw std::runtime_error(message.toStdString().c_str());
}

} // namespace med::pipeline
