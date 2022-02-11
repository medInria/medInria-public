#pragma once

namespace med::pipeline
{

enum OutputType
{
    MASK,
    IMAGE,
    COMPLEMENT_MASK,
    COMPLEMENT_IMAGE
};

enum RetryMode
{
    SINGLE_TRY,
    MULTIPLE_TRIES
};

enum OperationType
{
    ADD_MASK,
    ERASE_MASK
};

enum RadiusType
{
    RADIUS_IN_MM,
    RADIUS_IN_PIXELS,
    RADIUS_IN_MM_OR_PIXELS
};

/* MIN_BASIC_FILTER_TYPE and MAX_BASIC_FILTER_TYPE are provided to access the lowest and
 * highest values of BasicFilterType
 */
enum BasicFilterType
{
    MIN_BASIC_FILTER_TYPE,
    ADD = MIN_BASIC_FILTER_TYPE,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    GAUSSIAN,
    NORMALIZE,
    MEDIAN,
    INVERT,
    SHRINK,
    WINDOWING,
    THRESHOLDING,
    SIZE_THRESHOLDING,
    MAX_BASIC_FILTER_TYPE = SIZE_THRESHOLDING
};

/* The values of BasicFilterType and MorphologicalFilterType do not overlap so that
 * they may be used together in functions that need them both, as if they were just one enum.
 */
enum MorphologicalFilterType
{
    DILATE = MAX_BASIC_FILTER_TYPE + 1,
    ERODE,
    BINARY_CLOSE,
    BINARY_OPEN,
    GRAYSCALE_CLOSE,
    GRAYSCALE_OPEN
};

enum ThresholdType
{
    LOWER_THRESHOLD,
    UPPER_THRESHOLD,
    OUTSIDE_THRESHOLD
};

enum ResultDropMode
{
    NO_RESULT_DROP,
    SINGLE_RESULT_DROP,
    MULTIPLE_RESULT_DROPS
};

} // namespace med::pipeline
