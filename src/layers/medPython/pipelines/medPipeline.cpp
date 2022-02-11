/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPipeline.h"

#include "medPipelineChooserToolBox.h"
#include "medPipelineControlToolBox.h"
#include "medPipelineExportToolBox.h"
#include "medPipelineFactory.h"
#include "medPipelineInstanceReader.h"
#include "medPipelineInstanceWriter.h"
#include "medPipelineRoiToolBox.h"
#include "medPipelineValuesToolBox.h"
#include "medPipelineWorkspace.h"
#include "medPythonPipelineInstance.h"

namespace med::pipeline
{

namespace
{

template<class TYPE>
void registerType()
{
    if(!TYPE::registered())
    {
        qWarning() << "Unable to register " << TYPE::staticMetaObject.className();
    }
}

} // namespace

void initialize()
{
    PipelineFactory::initialize();

    registerType<PipelineWorkspace>();
    registerType<PipelineControlToolBox>();
    registerType<PipelineChooserToolBox>();
    registerType<PipelineInstance>();
    registerType<PythonPipelineInstance>();
    registerType<PipelineInstanceReader>();
    registerType<PipelineInstanceWriter>();
    registerType<PipelineAttachedData>();
    registerType<ValuesToolBox>();
    registerType<PipelineExportToolBox>();
    registerType<RoiToolBox>();

    PipelineDelegate::registerDelegate();
    ReformatDelegate::registerDelegate();
    RegistrationDelegate::registerDelegate();
    SegmentationDelegate::registerDelegate();
    FilteringDelegate::registerDelegate();
    RoiToolboxDelegate::registerDelegate();
}

} // namespace med::pipeline
