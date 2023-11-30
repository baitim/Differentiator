#ifndef PROCESS_CMD_H
#define PROCESS_CMD_H

#include "Errors.h"

const int MAX_CMD_COMMAND_SIZE = 30;

struct CmdInputData {
    int is_data_file;
    int is_help;
    char* name_data_file;
};

struct CmdLineOption {
    const char* name;
    const char* description;
    int n_args;
    ErrorCode (*callback)(const char* argv[], CmdInputData* data);
};

ErrorCode input_cmd         (int argc, const char* argv[], CmdInputData* cmd_data);
ErrorCode data_file_callback(const char* argv[], CmdInputData* data);
ErrorCode help_callback     (const char* /*argv*/[], CmdInputData* data);
ErrorCode cmd_data_delete   (CmdInputData* cmd_data);

const CmdLineOption OPTIONS[] = {
    {"--name_data_file", "include file with equation",  1, data_file_callback},
    {"--help",           "help",                        0, help_callback}
};
const int COUNT_OPTIONS = sizeof(OPTIONS) / sizeof(CmdLineOption);

#endif // PROCESS_CMD_H