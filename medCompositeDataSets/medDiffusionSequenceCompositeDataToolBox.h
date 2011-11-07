#ifndef MEDDIFFUSIONSEQUENCECOMPOSITEDATATOOLBOX_H
#define MEDDIFFUSIONSEQUENCECOMPOSITEDATATOOLBOX_H

#include <medToolBoxCompositeDataSetImporterCustom.h>

#include <medDiffusionSequenceCompositeData.h>
#include <itkGradientFileReader.h>


class medDiffusionSequenceCompositeDataToolBoxPrivate;

class medDiffusionSequenceCompositeDataToolBox: public medToolBoxCompositeDataSetImporterCustom {
    Q_OBJECT

public:

     medDiffusionSequenceCompositeDataToolBox(QWidget *parent = 0);
    ~medDiffusionSequenceCompositeDataToolBox();

public:

    static bool registered(void);

    QString description() const;

    typedef medDiffusionSequenceCompositeData::GradientReaderType GradientReaderType;
    typedef medDiffusionSequenceCompositeData::GradientType       GradientType;
    typedef medDiffusionSequenceCompositeData::GradientListType   GradientListType;
    typedef medDiffusionSequenceCompositeData::Volumes            Volumes;

public slots:

    bool import();
    void reset();
    void cancel();

    void onItemClicked (QTreeWidgetItem *, int);
    void onContextTreeMenu (QPoint);
    
private:

    medDiffusionSequenceCompositeDataToolBoxPrivate *d;

    void addVolumeToTree (unsigned int index, QString volumename);
    void addGradientToTree (unsigned int index, GradientType gradient);

    void readVolumes (QStringList paths);
    void readGradients (QString filepath);
    bool writeInDataBase();
};

medToolBoxCompositeDataSetImporterCustom* createMedDiffusionCompositeDataToolBox(QWidget *parent);

#endif
