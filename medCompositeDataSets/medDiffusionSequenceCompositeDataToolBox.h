#ifndef MEDDIFFUSIONSEQUENCECOMPOSITEDATATOOLBOX_H
#define MEDDIFFUSIONSEQUENCECOMPOSITEDATATOOLBOX_H

#include <medGui/medToolBoxCompositeDataSetImporterCustom.h>

class medDiffusionSequenceCompositeDataToolBoxPrivate;

class medDiffusionSequenceCompositeDataToolBox : public medToolBoxCompositeDataSetImporterCustom
{
    Q_OBJECT

public:
     medDiffusionSequenceCompositeDataToolBox (QWidget *parent = 0);
    ~medDiffusionSequenceCompositeDataToolBox (void);

public:
    static bool registered(void);

    QString description (void) const;
    
public slots:
    void run(void);

    bool import (void);
    void reset (void);
    void cancel (void);

    void onItemClicked (QTreeWidgetItem *, int);
    void onContextTreeMenu (QPoint);

private:
    medDiffusionSequenceCompositeDataToolBoxPrivate *d;

    void addFileToTree (QString path);
    void addFilesToTree (QStringList paths);
    
};

medToolBoxCompositeDataSetImporterCustom *createMedDiffusionCompositeDataToolBox(QWidget *parent);

#endif
