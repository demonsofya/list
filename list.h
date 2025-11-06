#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

//=============================================================================

#define Return_If_Error(list)                                                               \
    {                                                                                       \
        int error = ListVerify(list);                                                       \
        if (error != NO_LIST_ERROR) {                                                       \
            ListDump(list, __FILE__, __FUNCTION__, __LINE__, "FUNCTION ERROR");             \
            return error;                                                                   \
        }                                                                                   \
    }

#define Return_Zero_If_Error(list)                                                          \
    {                                                                                       \
        int error = ListVerify(list);                                                       \
        if (error != NO_LIST_ERROR) {                                                       \
            ListDump(list, __FILE__, __FUNCTION__, __LINE__, "FUNCTION ERROR");             \
            return 0;                                                                         \
        }                                                                                   \
    }

#define Return_Error(list)                                                                  \
    {                                                                                       \
        int error = ListVerify(list);                                                       \
        if (error != NO_LIST_ERROR)                                                         \
            ListDump(list, __FILE__, __FUNCTION__, __LINE__, "FUNCTION ERROR");             \
        return error;                                                                       \
    }

#define Return_If_Param_Error(pos)


//-----------------------------------------------------------------------------

#define DEBUG

#ifdef DEBUG
    #define ON_DEBUG(...) __VA_ARGS__
#else
    #define ON_DEBUG(...)
#endif

//-----------------------------------------------------------------------------

const int POISON = 1984;

const int CANARY = 451;

const int DUMMY_ELEMENT_POS = 0;

const int FREE_PREV_ELEMENT_POS = -1;

struct list_t {
    int *data;
    int *next;
    int *prev;

    int list_size;
    int free;
};

struct node_args_t {
    char        *label;
    const char  *fill_color;
    const char  *color;
    int         rank_num;
};

enum ListErrors {

    NO_LIST_ERROR       = 0,
    NEXT_LIST_ERROR     = 1 << 0,
    PREV_LIST_ERROR     = 1 << 1,
    FREE_LIST_ERROR     = 1 << 2,
    SIZE_LIST_ERROR     = 1 << 3,
    HEAD_LIST_ERROR     = 1 << 4,
    TAIL_LIST_ERROR     = 1 << 5,
    PTR_LIST_ERROR      = 1 << 6,
    PTR_DATA_ERROR      = 1 << 7,
    PTR_NEXT_ERROR      = 1 << 8,
    PTR_PREV_ERROR      = 1 << 9,
    PREV_NEXT_ERROR     = 1 << 10,
    INSERT_ERROR        = 1 << 11,
    DELETE_ERROR        = 1 << 12,
    ARG_LIST_ERROR      = 1 << 13
};


FILE* OpenLogFile();
void CloseLogFile();

list_t ListCtor(int data_size);
void ListDtor(list_t *list_struct);
void FillListArrays(list_t *list_struct);
void ReallocList(list_t *list_struct, int new_size);

int CheckParamPosition(list_t *list_struct, int pos);

int InsertAfter(list_t *list_struct, int prev_pos, int insert_element, int *new_element_pos);
int InsertBefore(list_t *list_struct, int next_pos, int insert_element, int *new_element_pos);
int ChangePositionsAfterInsertion(list_t *list_struct, int prev_pos, int curr_pos, int next_pos);
int InsertElement (list_t *list_struct, int prev_pos, int next_pos, int insert_element, int *new_element_pos);

int DeleteElement(list_t *list_struct, int curr_pos, bool *if_element_deleted);

int ListVerify(list_t *list_struct);
void ListDump(list_t *list_struct, const char *file_name, const char *function_name,
              int line_number, const char *calling_reason_string);
void ListDumpPrintErrors(list_t *list_struct, const char *file_name, const char *function_name,
                         int line_number, const char *calling_reason_string);
void ListDumpPrintListArraysErrors(list_t *list_struct, int error);

void CreateDumpGraphFile(list_t *list_struct);

int GetFreeElementPosition(list_t *list_struct);
int GetHeadPosition(list_t *list_struct);
int GetTailPosition(list_t *list_struct);
int GetNextPosition(list_t *list_struct, int curr_pos);
int GetPrevPosition(list_t *list_struct, int curr_pos);

char *GetNodeName(int node_num);
void ChangeDotFileName(int file_num, char *file_name);

node_args_t *NodeArgsCtor(const char *fill_color, const char *color, int rank_num);
void NodeArgsDtor(node_args_t *curr_node_args);
void SetNodeProperties(node_args_t *curr_node_args, const char *fill_color,
                       const char *color, int rank_num);

void PrintDotFileHeader(FILE *dot_file_ptr, char *DotFilePngName);
void PrintDotFileEnd(FILE *dot_file_ptr);
void CreateImageFromDotFile(char *DotFileName, char *DotFilePngName);

void DrawDotNodes(list_t *list_struct, FILE *dot_file_ptr);
void DrawDotEdges(list_t *list_struct, FILE *dot_file_ptr);
void DrawBothDirEdges(list_t *list_struct, FILE *dot_file_ptr);
void DrawCurrEdge(FILE *dot_file_ptr, const char* first_node, const char *second_node,
                  const char* color, int pen_width, const char *dir_type);
void DrawCurrNode(FILE *dot_file_ptr, node_args_t *curr_node_args, const char *node_name);
void DrawHeadAndTail(list_t *list_struct, FILE *dot_file_ptr);

char *CreateDotFileName(const char *file_type);

int ChangeListElements(list_t *list_struct, int first_pos, int second_pos);
int SortList(list_t *list_struct);

#endif // LIST_H_INCLUDED
