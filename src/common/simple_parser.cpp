#include "simple_parser.h"

#include "stdlib.h"
#include "Assert.h"

enum read_flags_return_e
ReadFlags(int                argc,
          const char* const* argv,
          const char**       base_file_name)
{
    ASSERT(argv != NULL);
    ASSERT(base_file_name != NULL);

    if (argc == 1)
    {
        return READ_FLAGS_RETURN_SUCCESS;
    }
    else if (argc ==  2)
    {
        (*base_file_name) = argv[1];
        return READ_FLAGS_RETURN_SUCCESS;
    }

    return READ_FLAGS_RETURN_TOO_MANY_ARGS;
}
