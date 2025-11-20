#ifndef SIMPLE_PARSER_H
#define SIMPLE_PARSER_H

enum read_flags_return_e
{
    READ_FLAGS_RETURN_SUCCESS,
    READ_FLAGS_RETURN_TOO_MANY_ARGS
};

read_flags_return_e ReadFlags(int                argc,
                              const char* const* argv,
                              const char**       base_file_name);

#endif // SIMPLE_PARSER
