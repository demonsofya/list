#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "list_ptrs.h"

//=============================================================================

#define SEREGA LOGFILE
FILE *LOGFILE = OpenLogFile();

#define DEFAULT_NODE_NAME "node_a"
#define DEFAULT_DOT_TXT_FILE_NAME "DotFileNuma.txt"
#define DEFAULT_DOT_PNG_FILE_NAME "DotFileNuma.png"

//=============================================================================

FILE* OpenLogFile() {

    FILE *file_ptr = fopen("ListLogFile.html", "w");
    atexit(CloseLogFile);

    return file_ptr;
}

void CloseLogFile() {

    printf("Programm ended\n");
    fclose(SEREGA);
}


//=============================================================================


list_t ListOnPointersCtor(int first_element) {

    list_t new_list = {};

    ListNode_t *first_node = (ListNode_t *) calloc(1, sizeof(ListNode_t));
    first_node->data = first_element;
    first_node->prev = NULL;
    first_node->next = NULL;


    new_list.head = first_node;
    new_list.tail = first_node;

    return new_list;
    //return first_node;
}


void ListOnPointersDtor(list_t *list_struct) {

    if (list_struct == NULL)
        return;

    ListNode_t *curr_node = list_struct->head;

    while (curr_node != NULL) {
        ListNode_t *next_node = curr_node->next;
        free(curr_node);
        curr_node = next_node;
    }

    //free(list_struct);
}


ListNode_t *NewElementAfter(list_t *list_struct, ListNode_t *prev_node, int new_data) {

    Return_If_Error(list_struct);

    if (prev_node == NULL)
        return NULL;

    ListNode_t *new_node = (ListNode_t *) calloc(1, sizeof(ListNode_t));
    if (new_node == NULL)
        return new_node;

    ListNode_t *next_node = prev_node->next;

    if (next_node == NULL)
        list_struct->tail = new_node;

    return NewElement(list_struct, prev_node, new_node, next_node, new_data);
}


ListNode_t *NewElementBefore(list_t *list_struct, ListNode_t *next_node, int new_data) {

    Return_If_Error(list_struct);

    if (next_node == NULL)
        return NULL;

    ListNode_t *new_node = (ListNode_t *) calloc(1, sizeof(ListNode_t));
    if (new_node == NULL)
        return new_node;

    ListNode_t *prev_node = next_node->prev;

    if (prev_node == NULL)
        list_struct->head = new_node;

    return NewElement(list_struct, prev_node, new_node, next_node, new_data);
}


ListNode_t *NewElement(list_t *list_struct, ListNode_t *prev_node, ListNode_t *new_node,
                          ListNode_t *next_node, int new_data) {

    Return_If_Error(list_struct);

    new_node->next = next_node;
    new_node->prev = prev_node;
    if (next_node != NULL)
        next_node->prev = new_node;

    if (prev_node != NULL)
        prev_node->next = new_node;

    new_node->data = new_data;

    return new_node;
}


bool DeleteElement(list_t *list_struct, ListNode_t *curr_node) {

    Return_If_Error(list_struct);

    if (curr_node == NULL)
        return false;

    ListNode_t *next_node = curr_node->next;
    ListNode_t *prev_node = curr_node->prev;

    free(curr_node);

    if (next_node == NULL || prev_node == NULL)
        return false;

    next_node->prev = prev_node;
    prev_node->next = next_node;

    return true;
}


//=============================================================================

int ListVerify(list_t *list_struct) {

    int error = NO_LIST_ERROR;

    if (list_struct == NULL)
        return PTR_LIST_ERROR;

    if (list_struct->head == NULL)
        error |= HEAD_LIST_ERROR;

    if (list_struct->tail == NULL)
        error |= TAIL_LIST_ERROR;

    if (list_struct->head != NULL && list_struct->tail != NULL)
        for (ListNode_t *i = list_struct->head; i != NULL; i = i->next)
            if (i->next != NULL)
                if (i->next->prev != i) {
                    error |= PREV_NEXT_ERROR;
                    break;
                }

    return error;
}


void ListDump(list_t *list_struct, const char *file_name, const char *function_name,
              int line_number, const char *calling_reason_string) {

    static int dot_files_counter = 0;

    ListDumpPrintErrors(list_struct, file_name, function_name, line_number, calling_reason_string);

    CreateDumpGraphFile(list_struct, &dot_files_counter);
}

//=============================================================================

void ListDumpPrintErrors(list_t *list_struct, const char *file_name, const char *function_name,
                         int line_number, const char *calling_reason_string) {

    int error = ListVerify(list_struct);

    fprintf(SEREGA, "<h2><font color=\"#CC0000\"><p align=\"center\">===========DUMP===========</p></font></h2>"
                    "\n<h3><p align=\"center\">%s</p></h3>\n\n", calling_reason_string);

    fprintf(SEREGA, "<h4>ListDump() from %s at %s:%d:</h4>\n<pre>", file_name, function_name, line_number);

    if (list_struct == NULL || error == PTR_LIST_ERROR) {
        fprintf(SEREGA, "ERROR: list pointer error. List pointer is [%p]\n\n", list_struct);
        return;
    }

    fprintf(SEREGA, "List structure [%p]\n", list_struct);

    if (error & HEAD_LIST_ERROR)
        fprintf(SEREGA, "ERROR: head position error. Position is [%p]\n\n", list_struct->head);
    else
        fprintf(SEREGA, "Head position is %p\n", list_struct->head);

    if (error & TAIL_LIST_ERROR)
        fprintf(SEREGA, "ERROR: tail position error. Position is [%p]\n\n", list_struct->tail);
    else
        fprintf(SEREGA, "Tail position is %p\n\n", list_struct->tail);

     if (error & PREV_NEXT_ERROR)
        fprintf(SEREGA, "ERROR: next and prev arrays are different \n");


    fprintf(SEREGA, "</pre>\n");
}


//=============================================================================

void CreateDumpGraphFile(list_t *list_struct, int *dot_files_counter) {

    if (list_struct == NULL || dot_files_counter == NULL)
        return;

    char DotFileName[] = DEFAULT_DOT_TXT_FILE_NAME;
    char DotFilePngName[] = DEFAULT_DOT_PNG_FILE_NAME;

    ChangeDotFileName(*dot_files_counter, DotFilePngName);
    ChangeDotFileName(*dot_files_counter, DotFileName);
    (*dot_files_counter)++;

    FILE *dot_file_ptr = fopen(DotFileName, "w");

    fprintf(SEREGA, "<img src=\"%s\" />\n", DotFilePngName);

    fprintf(dot_file_ptr, "digraph {\n");
    fprintf(dot_file_ptr, "node [shape = \"doubleoctagon\", style = \"filled\", fillcolor = \"#FF6666\", color = \"#CC0000\", fontcolor = white, fontsize=20, margin=0.1];\n");
    char curr_node[] = DEFAULT_NODE_NAME;

    DrawDotNodes(list_struct, dot_file_ptr);

    fprintf(dot_file_ptr, "}");

    fclose(dot_file_ptr);

    char command[100] = "";
    sprintf(command, "dot %s -T png > %s", DotFileName, DotFilePngName);

    system(command);

}

//-----------------------------------------------------------------------------

void DrawDotNodes(list_t *list_struct, FILE *dot_file_ptr) {

    if (list_struct == NULL || dot_file_ptr == NULL)
        return;

    char curr_node[] = DEFAULT_NODE_NAME;

    node_args_t curr_node_args = {};
    curr_node_args.label = (char *) calloc(100, sizeof(100));
    if (curr_node_args.label == NULL)
        return;

    int counter = 0;

    for (ListNode_t *curr_ptr = list_struct->head; curr_ptr != NULL; curr_ptr = curr_ptr->next) {
         ChangeNodeName(counter, curr_node);
         curr_node_args.rank_num = counter;

         curr_node_args.fill_color = "99CCFF";
         curr_node_args.color = "0066CC";

         snprintf(curr_node_args.label, 100, "data = %d | { next = %p | prev = %p }",
                                             curr_ptr->data, curr_ptr->next, curr_ptr->prev);

         DrawCurrNode(dot_file_ptr, &curr_node_args, curr_node);

         counter++;
    }
}

void DrawCurrNode(FILE *dot_file_ptr, node_args_t *curr_node_args, const char *node_name) {

    if (dot_file_ptr == NULL || curr_node_args == NULL || node_name == NULL)
        return;

    fprintf(dot_file_ptr, "%s [shape = \"Mrecord\", label = \"%s\", style = \"filled\", "
                          "fillcolor = \"#%s\", color = \"#%s\", rank = %d, fontcolor = black]\n",
                          node_name, curr_node_args->label, curr_node_args->fill_color,
                          curr_node_args->color, curr_node_args->rank_num);

}


//=============================================================================

void ChangeNodeName(int node_num, char *node) {

    if (node == NULL || strlen(node) <= 5)
        return;

    node[5] = 'a' + node_num;

}

void ChangeDotFileName(int file_num, char *file_name) {

    if (file_name == NULL)
        return;

    file_name[10] = 'a' + file_num;
}
