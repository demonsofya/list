#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

//=============================================================================

#define Return_If_Error(list)                                           \
    {                                                                   \
        int error = ListVerify(list);                                   \
        if (error != NO_LIST_ERROR) {                                   \
            ListDump(list, __FILE__, __FUNCTION__, __LINE__);           \
            return error;                                               \
        }                                                               \
    }

#define Return_Error(list)                                              \
    {                                                                   \
        int error = ListVerify(list);                                   \
        if (error != NO_LIST_ERROR)                                     \
            ListDump(list, __FILE__, __FUNCTION__, __LINE__);           \
        return error;                                                   \
    }


//-----------------------------------------------------------------------------

const int POISON = 1984;

struct list_t {
    int *data;
    int *next;
    int *prev;

    int arr_size;
    int free;
};

enum ListErrors {

    NO_LIST_ERROR   = 0,
    NEXT_LIST_ERROR = 1 << 0,
    PREV_LIST_ERROR = 1 << 1,
    FREE_LIST_ERROR = 1 << 2,
    SIZE_LIST_ERROR = 1 << 3,
    HEAD_LIST_ERROR = 1 << 4,
    TAIL_LIST_ERROR = 1 << 5,
    PTR_LIST_ERROR  = 1 << 6,
    PTR_DATA_ERROR  = 1 << 7,
    PTR_NEXT_ERROR  = 1 << 8,
    PTR_PREV_ERROR  = 1 << 9
};


FILE* OpenLogFile();
void CloseLogFile();

list_t ListCtor(int data_size);
void ListDtor(list_t *list_struct);
void InsertAfter(list_t *list_struct, int prev_pos, int insert_element);
void InsertBefore(list_t *list_struct, int next_pos, int insert_element);
void ChangePositionsAfterInsertion(list_t *list_struct, int prev_pos, int curr_pos, int next_pos);
void DeleteElement(list_t *list_struct, int curr_pos);
int ListVerify(list_t *list_struct);
void ListDump(list_t *list_struct, const char *file_name, const char *function_name, int line_number);


#endif // LIST_H_INCLUDED
