#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ProcessCmd.h"

ErrorCode input_cmd(int argc, const char* argv[], CmdInputData* cmd_data)
{
    assert(argv);
    assert(cmd_data);

    ErrorCode err = ERROR_NO;

    for (int i = 0; i < argc; i++) {
        for (int j = 0; j < COUNT_OPTIONS; j++) {
        	if (strcmp(argv[i], OPTIONS[j].name) == 0) {
                err = (*OPTIONS[j].callback)(&argv[i], cmd_data);
                if (err) return err;
                i += OPTIONS[j].n_args;
                break;
            }
        }
    }
    return err;
}

ErrorCode data_file_callback(const char* argv[], CmdInputData* data)
{
    data->is_data_file = 1;
    data->name_data_file = (char*)calloc(MAX_CMD_COMMAND_SIZE, sizeof(char));
    if (!data->name_data_file) return ERROR_ALLOC_FAIL;
	data->name_data_file = strcpy(data->name_data_file, argv[1]);
    if (!data->name_data_file) return ERROR_ALLOC_FAIL;

    return ERROR_NO;
}

ErrorCode help_callback(const char* /*argv*/[], CmdInputData* data)
{
    data->is_help = 1;

    return ERROR_NO;
}

ErrorCode cmd_data_delete(CmdInputData* cmd_data)
{
    if (!cmd_data) return ERROR_INVALID_CMD_DATA;

    free(cmd_data->name_data_file);

    return ERROR_NO;
}