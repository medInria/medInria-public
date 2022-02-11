#pragma once

#include <medDataIndex.h>

#include "medPipelineConstants.h"
#include "medPipelineExport.h"

#define MED_PIPELINE_DATA_ID(name) const QString (name) = #name
#define MED_PIPELINE_DATA_ID2(name, value) const QString (name) = value

namespace med::pipeline
{

extern QString PAINT_TOOLBOX;
extern QString CROP_TOOLBOX;
extern QString EXTRACTION_4D_TO_3D_TOOLBOX;
extern QString HISTOGRAM_ANALYSIS_TOOLBOX;
extern QString ITK_FILTERS_TOOLBOX;
extern QString ITK_MORPHOLOGICAL_FILTERS_TOOLBOX;
extern QString LAYERS_TOOLBOX;
extern QString LEVEL_SET_SEGMENTATION_TOOLBOX;
extern QString MANUAL_REGISTRATION_TOOLBOX;
extern QString MERGE_VOLUMES_TOOLBOX;
extern QString MOUSE_INTERACTION_TOOLBOX;
extern QString PIPELINE_CONTROL_TOOLBOX;
extern QString PIPELINE_CHOOSER_TOOLBOX;
extern QString POLYGON_ROI_TOOLBOX;
extern QString PROGRESSION_STACK_TOOLBOX;
extern QString RESLICE_TOOLBOX;
extern QString ROI_TOOLBOX;
extern QString VALUES_TOOLBOX;
extern QString VESSEL_SEGMENTATION_TOOLBOX;
extern QString VIEW_SETTINGS_TOOLBOX;
extern QString VOI_CUTTER_TOOLBOX;

MED_PIPELINE_DATA_ID(sharedMetaDataId);
MED_PIPELINE_DATA_ID(resultValuesId);
MED_PIPELINE_DATA_ID(ctDataId);
MED_PIPELINE_DATA_ID(mrDataId);

typedef QHash<QString, QString> QStringToQStringMap;

struct MEDPIPELINE_EXPORT RenderingParameters
{
    // these should be in vtkDataMeshInteractor
    static const char* SURFACE;
    static const char* WIREFRAME;
    static const char* POINTS;

    QString renderingMode, color, attribute;
    double opacity, minRange, maxRange;

    RenderingParameters() {}
    RenderingParameters(QString renderingMode, QString color, double opacity,
                        QString attribute = "", double minRange = 0, double maxRange = 0);
};

void writeQVariantContent(QXmlStreamWriter& out, const QVariant& variant);
QVariant readQVariantContent(QXmlStreamReader& in, QString type);

void writeDataIndexContent(QXmlStreamWriter& out, const medDataIndex& index);
medDataIndex readDataIndexContent(QXmlStreamReader& in);

template<typename TYPE>
void writeNumericQListContent(QXmlStreamWriter& out, QList<TYPE> list);
template<typename TYPE>
QList<TYPE> readNumericQListContent(QXmlStreamReader& in);

void writeQStringListContent(QXmlStreamWriter& out, QStringList list);
QStringList readQStringListContent(QXmlStreamReader& in);

template<typename VALUE_TYPE>
void writeQHashContent(QXmlStreamWriter& out, QHash<QString, VALUE_TYPE> hashTable);
template<typename VALUE_TYPE>
QHash<QString, VALUE_TYPE> readQHashContent(QXmlStreamReader& in);

void writeRenderingParametersContent(QXmlStreamWriter& out, const RenderingParameters& parameters);
RenderingParameters readRenderingParametersContent(QXmlStreamReader& in);

bool hasXMLError(QXmlStreamReader& stream);
void throwXMLException(QXmlStreamReader& stream);

#define THROW_METHOD_EXCEPTION(message) throw std::runtime_error(QString(QString(typeid(*this).name()) + "::" + __func__ + ": " + message).toStdString().c_str())

} // namespace med::pipeline

Q_DECLARE_METATYPE(QList<bool>)
Q_DECLARE_METATYPE(QList<char>)
Q_DECLARE_METATYPE(QList<unsigned char>)
Q_DECLARE_METATYPE(QList<short>)
Q_DECLARE_METATYPE(QList<unsigned short>)
Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(QList<unsigned int>)
Q_DECLARE_METATYPE(QList<long>)
Q_DECLARE_METATYPE(QList<unsigned long>)
Q_DECLARE_METATYPE(QList<long long>)
Q_DECLARE_METATYPE(QList<unsigned long long>)
Q_DECLARE_METATYPE(QList<float>)
Q_DECLARE_METATYPE(QList<double>)
Q_DECLARE_METATYPE(QList<QString>)
Q_DECLARE_METATYPE(med::pipeline::QStringToQStringMap)
Q_DECLARE_METATYPE(med::pipeline::RenderingParameters)
