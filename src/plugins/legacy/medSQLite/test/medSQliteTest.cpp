/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include "gtest/gtest.h"
#include <medSQLite.h>


TEST(medSQlite, is_writeable_test)
{
    medSQlite m = medSQlite();
    EXPECT_EQ(true, m.isWriteable());
}

TEST(medSQlite, is_local_test)
{
    medSQlite m = medSQlite();
    EXPECT_EQ(true, m.isLocal());
}

TEST(medSQlite, is_cached_test)
{
    medSQlite m = medSQlite();
    EXPECT_EQ(false, m.isCached());
}

TEST(medSQlite, is_online_test)
{
    medSQlite m = medSQlite();
    EXPECT_EQ(false, m.isOnline());
}
