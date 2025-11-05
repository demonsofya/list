#ifndef LIST_PTRS_H_INCLUDED
#define LIST_PTRS_H_INCLUDED

#define Return_If_Error(list)                                                               \
    {                                                                                       \
        int error = ListVerify(list);                                                       \
        if (error != NO_LIST_ERROR) {                                                       \
            ListDump(list, __FILE__, __FUNCTION__, __LINE__, "FUNCTION ERROR");             \
            return NULL;                                                                         \
        }                                                                                   \
    }

struct ListNode_t {
    int data;
    ListNode_t *prev;
    ListNode_t *next;
};

struct list_t {
    ListNode_t *head;
    ListNode_t *tail;
};

enum ListErrors {
    NO_LIST_ERROR   = 0,
    PTR_LIST_ERROR  = 1 << 0,
    HEAD_LIST_ERROR = 1 << 1,
    TAIL_LIST_ERROR = 1 << 2,
    PREV_NEXT_ERROR = 1 << 3
};

struct node_args_t {
    char        *label;
    const char  *fill_color;
    const char  *color;
    int         rank_num;
};

list_t ListOnPointersCtor(int first_element);
void ListOnPointersDtor(list_t *list_struct);

ListNode_t *NewElementAfter(list_t *list_struct, ListNode_t *prev_node, int new_data);
ListNode_t *NewElementBefore(list_t *list_struct, ListNode_t *next_node, int new_data);
ListNode_t *NewElement(list_t *list_struct, ListNode_t *prev_node, ListNode_t *new_node,
                          ListNode_t *next_node, int new_data);

bool DeleteElement(list_t *list_struct, ListNode_t *curr_node);

void DrawDotNodes(list_t *list_struct, FILE *dot_file_ptr);
void DrawCurrNode(FILE *dot_file_ptr, node_args_t *curr_node_args, const char *node_name);
char *CreateDotFileName(const char *file_type);
char *GetNodeName(int node_num);

void CreateDumpGraphFile(list_t *list_struct);
void ListDumpPrintErrors(list_t *list_struct, const char *file_name, const char *function_name,
                         int line_number, const char *calling_reason_string);
void ListDump(list_t *list_struct, const char *file_name, const char *function_name,
              int line_number, const char *calling_reason_string);

int ListVerify(list_t *list_struct);

FILE* OpenLogFile();
void CloseLogFile();

#endif // LIST_PTRS_H_INCLUDED
