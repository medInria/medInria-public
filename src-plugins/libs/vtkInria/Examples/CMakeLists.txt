#######################################################################
#
# medInria
#
# Copyright (c) INRIA 2013. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
#######################################################################

SUBDIRS(SynchronizedViews
        FibersManager
        IsosurfacesManager
        TensorManager
        CompareImageManager
        DataManager
        PolydataToFiberXML
        RegisterDataSets
  )

if (VTKINRIA_USE_SPHERICALHARMONICS)
    add_subdirectory(SphericalHarmonicView)
endif()
