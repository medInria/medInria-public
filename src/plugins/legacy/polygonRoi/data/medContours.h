#pragma once


#include "medTagContours.h"

#include <medAbstractMeshData.h>

class medContoursPrivate;

/**
 * @brief A Contour is a set of points with label.
 */
class medContours : public medAbstractMeshData
{
    Q_OBJECT
    MED_DATA_INTERFACE("Contour Data",
                       "Set of nodes which defines a contour")

public:
    medContours();
    medContours(const medContours& other);
    virtual ~medContours();

    // Covariance
    virtual medContours* clone() override;

    static bool registered();

    QVector<medTagContours> &getTagContoursSet();

public slots:
    // derived from dtkAbstractData
    void* data() override;
    void* data(int channel) override;

    void setData(void* data) override;
    void setData(void* data, int channel) override;

    void* output() override;

private:
    std::unique_ptr<medContoursPrivate> d;
};
