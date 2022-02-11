#pragma once

#include <medToolBox.h>

#include "medPipelineExport.h"

namespace med::pipeline
{

class ValuesToolBoxPrivate;

class MEDPIPELINE_EXPORT ValuesToolBox : public medToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Results", "Values toolbox",)

public:

    static bool registered();

    ValuesToolBox(QWidget* parent = 0);
    virtual ~ValuesToolBox();

    void addValues(QHash<QString, QVariant> resultVariables);

private slots:
    void exportToFile();

private:
    ValuesToolBoxPrivate* const d;
};

} // namespace med::pipeline


