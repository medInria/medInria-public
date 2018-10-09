#pragma once

#include <dtkCore/dtkSmartPointer>
#include <medUtilities.h>
#include <medUtilitiesExport.h>

class medAbstractData;
class vtkDataArray;

class MEDUTILITIES_EXPORT medUtilitiesVTK
{
public:

    enum DataArrayType
    {
        POINT_ARRAY,
        CELL_ARRAY,
        FIELD_ARRAY
    };

    static medAbstractData *changeMaxNumberOfMeshTriangles(medAbstractData *mesh, int value);

    /**
     * @brief Retrieves an array from input data. This functions first looks in
     * point data, then cell data and finally in field data.
     *
     * @param[in] data input data, must be a mesh or map
     * @param[in] arrayName array to retrieve
     * @return specified array if it exits, nullptr otherwise
     */
    static vtkDataArray* getArray(medAbstractData* data,
                                  QString arrayName);

    /**
     * @brief Return array index and data type as optional argument
     * @param[in] data input data, must be a mesh or map
     * @param[in] arrayName array name
     * @param[out] arrayType type of array: point, cell or field array
     * @return data index of array
     */
    static int getArrayIndex(medAbstractData* data,
                             QString arrayName,
                             DataArrayType* arrayType = nullptr);

    /**
     * @brief Retrieve single tuple from a real-valued array.
     *
     * @param[in] data input data, must be a mesh or map
     * @param[in] arrayName input array name
     * @param[in] index tuple index
     * @return tuple as a list. Returns a empty list on failure
     */
    static QList<double> peekArray(medAbstractData* data,
                                   QString arrayName,
                                   int index);
    /**
     * @brief Compute real-valued array range (min and max values).
     *
     * @param[in] data input data, must be a mesh or map
     * @param[in] arrayName input array name
     * @param[out] minRange minimum range
     * @param[out] maxRange maximum range
     * @param[in] component range will be computed on this component.
     *            Pass -1 to compute the L2 norm over all components.
     * @return true if all went well, false otherwise
     */
    static bool arrayRange(medAbstractData* data,
                           QString arrayName,
                           double* minRange,
                           double* maxRange,
                           int component = 0);

    /**
     * @brief Computes mean and standard deviation of an real-valued array.
     * N.B. stats will be computed only on the first component.
     *
     * @param[in] data input data, must be a mesh or map
     * @param[in] arrayName input array name
     * @param[in] component stats will be computed on this component
     * @return list with 2 values: mean and standard deviation (in that order).
     *         Returns an empty list on failure.
     */
    static bool arrayStats(medAbstractData* data,
                           QString arrayName,
                           double* mean,
                           double* stdDev,
                           int component = 0);
};
