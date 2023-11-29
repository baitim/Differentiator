#ifndef ERRORS_H
#define ERRORS_H

struct Error {
    int error;
    const char* description;
};

enum ErrorCode {
    ERROR_NO =                  0,
    ERROR_ALLOC_FAIL =          1 << 0,
    ERROR_INVALID_TREE =        1 << 1,
    ERROR_SYSTEM_COMMAND =      1 << 2,
    ERROR_OPEN_FILE =           1 << 3,
    ERROR_EMPTY_NODE =          1 << 4,
    ERROR_READ_INPUT =          1 << 5,
    ERROR_INVALID_INPUT =       1 << 6,
    ERROR_INVALID_FILE =        1 << 7,
    ERROR_INVALID_BUF =         1 << 8,
    ERROR_READ_FILE =           1 << 9,
    ERROR_COUNT_VALID =         1 << 10,
    ERROR_TREE_LOOP =           1 << 11,
    ERROR_TREE_DEP =            1 << 12,
    ERROR_LEAF =                1 << 13,
    ERROR_STRDUP =              1 << 14,
    ERROR_INPUT_VARIABLE =      1 << 15,
    ERROR_DIVISION_ZERO =       1 << 16,
    ERROR_DUPLICATE_VAR =       1 << 17,
    ERROR_INVALID_CMD_DATA =    1 << 18,
    ERROR_INVALID_VAR_EVAL =    1 << 19,
};

const Error ERRORS[] = {
    {ERROR_NO,                  "no errors"},
    {ERROR_ALLOC_FAIL,          "alloc fail"},
    {ERROR_INVALID_TREE,        "invalid pointer to tree"},
    {ERROR_SYSTEM_COMMAND,      "cannot process system command"},
    {ERROR_OPEN_FILE,           "invalid name of file"},
    {ERROR_EMPTY_NODE,          "invalid node in tree"},
    {ERROR_READ_INPUT,          "invalid input argument"},
    {ERROR_INVALID_INPUT,       "invalid input"},
    {ERROR_INVALID_FILE,        "invalid pointer to file"},
    {ERROR_INVALID_BUF,         "invalid pointer to buffer"},
    {ERROR_READ_FILE,           "cannot read file"},
    {ERROR_COUNT_VALID,         "wrong count of valid nodes"},
    {ERROR_TREE_LOOP,           "tree has loop"},
    {ERROR_TREE_DEP,            "wrong depth in tree"},
    {ERROR_LEAF,                "leaf isn't number or variable"},
    {ERROR_STRDUP,              "cannot strdup"},
    {ERROR_INPUT_VARIABLE,      "variable has banned symbol"},
    {ERROR_DIVISION_ZERO,       "division by zero is evalute"},
    {ERROR_DUPLICATE_VAR,       "reinitialization variable in input file"},
    {ERROR_INVALID_CMD_DATA,    "cmd data already free"},
    {ERROR_INVALID_VAR_EVAL,    "use variable without value"},
};
const int COUNT_ERRORS = sizeof(ERRORS) / sizeof(Error);

#define err_dump(err) err_dump_(err, __FILE__, __PRETTY_FUNCTION__, __LINE__) 
void err_dump_(int err, const char* file, const char* func, int line);

#endif // ERRORS_H