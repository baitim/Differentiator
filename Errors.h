#ifndef ERRORS_H
#define ERRORS_H

struct Error {
    int error;
    const char* description;
};

enum ErrorCode {
    ERROR_NO =                          0,
    ERROR_ALLOC_FAIL =                  1 << 0,
    ERROR_INVALID_TREE =                1 << 1,
    ERROR_SYSTEM_COMMAND =              1 << 2,
    ERROR_OPEN_FILE =                   1 << 3,
    ERROR_EMPTY_NODE =                  1 << 4,
    ERROR_READ_INPUT =                  1 << 5,
    ERROR_INVALID_INPUT =               1 << 6,
    ERROR_INVALID_FILE =                1 << 7,
    ERROR_INVALID_BUF =                 1 << 8,
    ERROR_READ_FILE =                   1 << 9,
};

const Error ERRORS[] = {
    {ERROR_NO,                          "no errors"},
    {ERROR_ALLOC_FAIL,                  "realloc fail"},
    {ERROR_INVALID_TREE,                "invalid pointer to tree"},
    {ERROR_SYSTEM_COMMAND,              "cannot process system command"},
    {ERROR_OPEN_FILE,                   "invalid name of file"},
    {ERROR_EMPTY_NODE,                  "invalid node in tree"},
    {ERROR_READ_INPUT,                  "invalid input argument"},
    {ERROR_INVALID_INPUT,               "invalid input"},
    {ERROR_INVALID_FILE,                "invalid pointer to file"},
    {ERROR_INVALID_BUF,                 "invalid pointer to buffer"},
    {ERROR_READ_FILE,                   "cannot read file"},
};
const int COUNT_ERRORS = sizeof(ERRORS) / sizeof(Error);

#define err_dump(err) err_dump_(err, __FILE__, __PRETTY_FUNCTION__, __LINE__) 
void err_dump_(int err, const char* file, const char* func, int line);

#endif // ERRORS_H