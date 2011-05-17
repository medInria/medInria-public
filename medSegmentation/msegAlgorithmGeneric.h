
#ifndef _msegAlgorithmGeneric_h_
#define _msegAlgorithmGeneric_h_

namespace mseg {

/** Generic segmentation algorithm */
class AlgorithmGeneric {

public:
    AlgorithmGeneric::AlgorithmGeneric();
    virtual ~AlgorithmGeneric();

    virtual void run() = 0;
protected:
private:

};

} // namespace mseg

#endif // _msegAlgorithmGeneric_h_
