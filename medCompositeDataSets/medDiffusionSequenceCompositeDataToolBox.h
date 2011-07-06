#ifndef MEDDIFFUSIONSEQUENCECOMPOSITEDATATOOLBOX_H
#define MEDDIFFUSIONSEQUENCECOMPOSITEDATATOOLBOX_H

#include <medGui/medToolBoxCompositeDataSetImporterCustom.h>

#include <medDiffusionSequenceCompositeData.h>
#include <itkGradientFileReader.h>


class medDiffusionSequenceCompositeDataToolBoxPrivate;

class medDiffusionSequenceCompositeDataToolBox: public medToolBoxCompositeDataSetImporterCustom {
    Q_OBJECT

public:

     medDiffusionSequenceCompositeDataToolBox (QWidget *parent = 0);
    ~medDiffusionSequenceCompositeDataToolBox (void);

public:

    static bool registered(void);

    QString description (void) const;

    typedef medDiffusionSequenceCompositeData::GradientReaderType GradientReaderType;
    typedef medDiffusionSequenceCompositeData::GradientType       GradientType;
    typedef medDiffusionSequenceCompositeData::GradientListType   GradientListType;
    typedef medDiffusionSequenceCompositeData::Volumes            Volumes;

public slots:

    bool import (void);
    void reset (void);
    void cancel (void);

    void onItemClicked (QTreeWidgetItem *, int);
    void onContextTreeMenu (QPoint);
    
private:

    medDiffusionSequenceCompositeDataToolBoxPrivate *d;

    void addVolumeToTree (unsigned int index, QString volumename);
    void addGradientToTree (unsigned int index, GradientType gradient);

    void readVolumes (QStringList paths);
    void readGradients (QString filepath);
    bool writeInDataBase (void);
};

medToolBoxCompositeDataSetImporterCustom *createMedDiffusionCompositeDataToolBox(QWidget *parent);

#endif
