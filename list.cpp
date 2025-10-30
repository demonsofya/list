#include <assert.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "list.h"

//=============================================================================

#define SEREGA LOGFILE
FILE *LOGFILE = OpenLogFile();

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

//-----------------------------------------------------------------------------

list_t ListCtor(int data_size) {

    list_t list_struct = {};

    list_struct.data = (int *) calloc(sizeof(int), data_size);
    list_struct.next = (int *) calloc(sizeof(int), data_size);
    list_struct.prev = (int *) calloc(sizeof(int), data_size);

    for (int i = 1; i < data_size; i++) {
        list_struct.next[i] = i + 1;
        list_struct.prev[i] = -1;
        list_struct.data[i] = POISON;
    }

    list_struct.data[0] = CANARY; // TODO: DUMMY_ELEMENT_POS

    list_struct.next[data_size - 1] = 0;

    list_struct.list_size = data_size;
    list_struct.free = 1;

    return list_struct;
}


void ListDtor(list_t *list_struct) {

    if (list_struct == NULL)
        return;

    list_struct->list_size = POISON;
    list_struct->free = POISON;

    free(list_struct->data);
    free(list_struct->next);
    free(list_struct->prev);

    free(list_struct);
}

//=============================================================================

void ReallocList(list_t *list_struct, int new_size) {

    assert(list_struct);

    if (new_size <= 0)
        return;

    size_t new_size_in_bytes = new_size * sizeof(int);

    list_struct->data = (int *) realloc(list_struct->data, new_size_in_bytes);
    list_struct->next = (int *) realloc(list_struct->next, new_size_in_bytes);
    list_struct->prev = (int *) realloc(list_struct->prev, new_size_in_bytes);

    list_struct->free = list_struct->list_size;
    list_struct->list_size = new_size;

    for (int i = list_struct->free; i < list_struct->list_size; i++) {
        list_struct->next[i] = i + 1;
        list_struct->prev[i] = -1;
        list_struct->data[i] = POISON;
    }

    list_struct->next[list_struct->list_size - 1] = 0;
}

//=============================================================================

int GetFreeElementPosition(list_t *list_struct) {

    assert(list_struct);

    return list_struct->free;
}

int GetHeadPosition(list_t *list_struct) {

    assert(list_struct);

    return list_struct->next[0];
}

int GetTailPosition(list_t *list_struct) {

    assert(list_struct);

    return list_struct->prev[0];
}

int GetNextPosition(list_t *list_struct, int curr_pos) {

    assert(list_struct);

    return list_struct->next[curr_pos];
}

int GetPrevPosition(list_t *list_struct, int curr_pos) {

    assert(list_struct);

    return list_struct->prev[curr_pos];
}

//=============================================================================

int CheckParamPosition(list_t *list_struct, int pos) {

    Return_If_Error(list_struct);

    if (pos < 0 || pos >= list_struct->list_size)
        return ARG_LIST_ERROR;

    if (list_struct->prev[pos] == -1)
        return ARG_LIST_ERROR;

    return NO_LIST_ERROR;
}

int InsertAfter(list_t *list_struct, int prev_pos, int insert_element, int *new_element_pos) {

    Return_If_Error(list_struct);

    if (CheckParamPosition(list_struct, prev_pos) != NO_LIST_ERROR) {
        ListDump(list_struct, __FILE__, __FUNCTION__, __LINE__, "INSERT ERROR");
        return INSERT_ERROR;
    }

    if (list_struct->free == 0)
        ReallocList(list_struct, list_struct->list_size * 2);

    int next_pos = GetNextPosition(list_struct, prev_pos); // один из них

    return InsertElement(list_struct, prev_pos, next_pos, insert_element, new_element_pos);
}


int InsertBefore(list_t *list_struct, int next_pos, int insert_element, int *new_element_pos) {

    Return_If_Error(list_struct);

    if (CheckParamPosition(list_struct, next_pos) != NO_LIST_ERROR) {
        ListDump(list_struct, __FILE__, __FUNCTION__, __LINE__, "INSERT ERROR");
        return INSERT_ERROR;
    }

    if (list_struct->free == 0)
        ReallocList(list_struct, list_struct->list_size * 2);

    int prev_pos = GetPrevPosition(list_struct, next_pos);

    return InsertElement(list_struct, prev_pos, next_pos, insert_element, new_element_pos);
}

/*
InsertAfter {


    if (CheckParamPosition(list_struct, next_pos) != NO_LIST_ERROR) {
        ListDump(list_struct, __FILE__, __FUNCTION__, __LINE__, "INSERT ERROR");
        return INSERT_ERROR;
    }

    if (list_struct->free == 0)
        ReallocList(list_struct, list_struct->list_size * 2);

    int next_pos = GetNextPosition(list_struct, prev_pos); // один из них
    //prev_pos = ...;

    return Insert()
}
*/
int InsertElement (list_t *list_struct, int prev_pos, int next_pos, int insert_element, int *new_element_pos) {

    Return_If_Error(list_struct);

    int curr_pos = GetFreeElementPosition(list_struct);

    list_struct->data[curr_pos] = insert_element;

    ChangePositionsAfterInsertion(list_struct, prev_pos, curr_pos, next_pos);

    if (new_element_pos != NULL)
        *new_element_pos = curr_pos;

    Return_Error(list_struct);
}


int ChangePositionsAfterInsertion(list_t *list_struct, int prev_pos, int curr_pos, int next_pos) {

    Return_If_Error(list_struct);

    list_struct->free = GetNextPosition(list_struct, list_struct->free);

    list_struct->next[curr_pos] = next_pos;
    list_struct->next[prev_pos] = curr_pos;

    list_struct->prev[next_pos] = curr_pos;
    list_struct->prev[curr_pos] = prev_pos;

    Return_Error(list_struct);
}

//-----------------------------------------------------------------------------

int DeleteElement(list_t *list_struct, int curr_pos, bool *if_element_deleted) {

    if (CheckParamPosition(list_struct, curr_pos) != NO_LIST_ERROR) {
        ListDump(list_struct, __FILE__, __FUNCTION__, __LINE__, "DELETE ERROR");
        return DELETE_ERROR;
    }

    Return_If_Error(list_struct);

    if (curr_pos <= 0 || curr_pos >= list_struct->list_size || list_struct->prev[curr_pos] == -1)
        if (if_element_deleted != NULL) {
            *if_element_deleted = false;

            Return_Error(list_struct);
        }

    int prev_pos = GetPrevPosition(list_struct, curr_pos);
    int next_pos = GetNextPosition(list_struct, curr_pos);

    list_struct->next[curr_pos] = GetFreeElementPosition(list_struct);
    list_struct->next[prev_pos] = next_pos;

    list_struct->free = curr_pos;

    list_struct->data[curr_pos] = POISON;

    list_struct->prev[next_pos] = prev_pos;
    list_struct->prev[curr_pos] = -1; //...

    if (if_element_deleted != NULL)
        *if_element_deleted = false;

    Return_Error(list_struct);
}

//=============================================================================

int ListVerify(list_t *list_struct) {

    int error = NO_LIST_ERROR;

    if (list_struct == NULL)
        return PTR_LIST_ERROR;

    if (list_struct->data == NULL)
        error |= PTR_DATA_ERROR;

    if (list_struct->list_size <= 0)
        error |= SIZE_LIST_ERROR;

    if (list_struct->free < 0)
        error |= FREE_LIST_ERROR;

    if (list_struct->next == NULL)
        error |= PTR_NEXT_ERROR;
    else if (list_struct->next[0] < 0 || list_struct->next[0] >= list_struct->list_size)
        error |= HEAD_LIST_ERROR;

    if (list_struct->prev == NULL)
        error |= PTR_PREV_ERROR;
    else if (list_struct->prev[0] < 0 || list_struct->prev[0] >= list_struct->list_size)
        error |= TAIL_LIST_ERROR;

    if (list_struct->next != NULL && list_struct->prev != NULL) {
        for (int i = 1; i < list_struct->list_size; i++)
            if (list_struct->prev[list_struct->next[i]] != i && list_struct->prev[i] != -1) {
                error |= PREV_NEXT_ERROR;
                break;
            }

        int counter = 0;
        for (int i = GetHeadPosition(list_struct); i != 0 && counter <= list_struct->list_size; i = list_struct->next[i])
            counter++;
        if (counter > list_struct->list_size)
            error |= NEXT_LIST_ERROR;

        counter = 0;
        for (int i = GetTailPosition(list_struct); i != 0 && counter <= list_struct->list_size; i = list_struct->prev[i])
            counter++;
        if (counter > list_struct->list_size)
            error |= PREV_LIST_ERROR;

    }

    return error;
}


void ListDump(list_t *list_struct, const char *file_name, const char *function_name,
              int line_number, const char *calling_reason_string) {

    static int dot_files_counter = 0;

    ListDumpPrintErrors(list_struct, file_name, function_name, line_number, calling_reason_string);

    CreateDumpGraphFile(list_struct, &dot_files_counter);
}


void ListDumpPrintErrors(list_t *list_struct, const char *file_name, const char *function_name,
                         int line_number, const char *calling_reason_string) {

    int error = ListVerify(list_struct);

    fprintf(SEREGA, "<h2><font color=\"#CC0000\"><p align=\"center\">===========DUMP===========</p></font></h2>\n<h3><p align=\"center\">%s</p></h3>\n\n",
                     calling_reason_string);

    fprintf(SEREGA, "<h4>ListDump() from %s at %s:%d:</h4>\n<pre>", file_name, function_name, line_number);

    if (list_struct == NULL || error == PTR_LIST_ERROR) {
        fprintf(SEREGA, "ERROR: list pointer error. List pointer is [%p]\n\n", list_struct);
        return;
    }

    fprintf(SEREGA, "List structure [%p]\n", list_struct);

    if (error & SIZE_LIST_ERROR)
        fprintf(SEREGA, "ERROR: list array size error. Size is %d\n", list_struct->list_size);
    else
        fprintf(SEREGA, "List size is %d\n\n", list_struct->list_size);

    if (error & FREE_LIST_ERROR)
        fprintf(SEREGA, "ERROR: free element position error. Position is [%d]\n\n", list_struct->free);
    else
        fprintf(SEREGA, "Free position is %d\n", list_struct->free);

    if (error & HEAD_LIST_ERROR)
        fprintf(SEREGA, "ERROR: head position error. Position is [%d]\n\n", list_struct->next[0]);
    else
        fprintf(SEREGA, "Head position is %d\n", list_struct->next[0]);

    if (error & TAIL_LIST_ERROR)
        fprintf(SEREGA, "ERROR: tail position error. Position is [%d]\n\n", list_struct->prev[0]);
    else
        fprintf(SEREGA, "Tail position is %d\n\n", list_struct->prev[0]);


    if (list_struct->data == NULL)
        fprintf(SEREGA, "ERROR: data array pointer error. Data pointer is [%p]\n\n", list_struct->data);
    else {
        fprintf(SEREGA, "----------DATA---------\n");
        for (int i = 0; i < list_struct->list_size; i++) {
            fprintf(SEREGA, "[%d] = %d\n", i, list_struct->data[i]);
        }
        fprintf(SEREGA, "-----------------------\n\n");
    }

    if (error & NEXT_LIST_ERROR)
        fprintf(SEREGA, "ERROR: next array error \n");
    if (list_struct->next == NULL)
        fprintf(SEREGA, "ERROR: next array pointer error. Next pointer is [%p]\n\n", list_struct->next);
    else {
        fprintf(SEREGA, "----------NEXT---------\n");
        for (int i = 0; i < list_struct->list_size; i++) {
            fprintf(SEREGA, "[%d] ", list_struct->next[i]);
        }
        fprintf(SEREGA, "\n-----------------------\n\n");
    }

    if (error & PREV_LIST_ERROR)
        fprintf(SEREGA, "ERROR: prev array error \n");
    if (list_struct->prev == NULL)
        fprintf(SEREGA, "ERROR: prev array pointer error. Prev pointer is [%p]\n\n", list_struct->next);
    else {
        fprintf(SEREGA, "----------PREV---------\n");
        for (int i = 0; i < list_struct->list_size; i++) {
            fprintf(SEREGA, "[%d] ", list_struct->prev[i]);
        }
        fprintf(SEREGA, "\n-----------------------\n\n");
    }

    fprintf(SEREGA, "</pre>\n");
}


void CreateDumpGraphFile(list_t *list_struct, int *dot_files_counter) {

    if (list_struct == NULL || dot_files_counter == NULL)
        return;

    char DotFileName[] = "DotFileNuma.txt";
    char DotFilePngName[] = "DotFileNuma.png";

    DotFilePngName[10] = 'a' + *dot_files_counter;
    DotFileName[10] = 'a' + (*dot_files_counter)++;

    FILE *dot_file_ptr = fopen(DotFileName, "w");

    fprintf(SEREGA, "<img src=\"%s\" />\n", DotFilePngName);

    fprintf(dot_file_ptr, "digraph {\nrankdir=\"LR\";\n");
    char curr_node[] = "node_a";

    fprintf(dot_file_ptr, "%s [shape = \"Mrecord\", label = \"data = %d | index = %d | { HEAD = %d | TAIL = %d }\", style = \"filled\", fillcolor = \"#baacc7\", color = \"#876f9e\", rank = %d]\n",
                                   curr_node, list_struct->data[0], 0, list_struct->next[0], list_struct->prev[0], 0);

    for (int i = 1; i < list_struct->list_size; i++) {
         curr_node[5] = 'a' + i;

         if (i == list_struct->free)
            fprintf(dot_file_ptr, "%s [shape = \"Mrecord\", label = \"FREE | data = %d | index = %d | { next = %d | prev = %d }\", style = \"filled\", fillcolor = \"#ccccff\", color = \"#9999ff\", rank = %d]\n",
                                   curr_node, list_struct->data[i], i, list_struct->next[i], list_struct->prev[i], i);
         else
            fprintf(dot_file_ptr, "%s [shape = \"Mrecord\", label = \"data = %d | index =  %d | { next = %d | prev = %d }\", style = \"filled\", fillcolor = \"#f0ceda\", color = \"#f7abc0\", rank = %d]\n",
                                   curr_node, list_struct->data[i], i, list_struct->next[i], list_struct->prev[i], i);
    }


    fprintf(dot_file_ptr, "{\n edge [color = \"#f5f5dc\", dir=none];\n");
    for (int i = 0; i < list_struct->list_size - 1; i++) {
        curr_node[5] = 'a' + i;
        fprintf(dot_file_ptr, "\"%s\" -> ", curr_node);
    }
    curr_node[5] = 'a' + list_struct->list_size - 1;
    fprintf(dot_file_ptr, "\"%s\" [weight = 1000000];\n}\n", curr_node);


    fprintf(dot_file_ptr, "{\n edge [color = \"#6600CC\", penwidth = 0.2, arrowhead = dot];\n");
    for (int i = list_struct->next[0]; i != 0; i = list_struct->next[i]) {
        curr_node[5] = 'a' + i;

        if (list_struct->next[i] == 0)
            fprintf(dot_file_ptr, "\"%s\";\n}\n", curr_node);
        else
            fprintf(dot_file_ptr, "\"%s\" -> ", curr_node);
    }


    fprintf(dot_file_ptr, "{\n edge [color = \"#523f49\", penwidth = 0.2, arrowhead = dot];\n");
    for (int i = list_struct->prev[0]; i != 0 && i != -1; i = list_struct->prev[i]) {
        curr_node[5] = 'a' + i;

        if (list_struct->prev[i] == 0 || list_struct->prev[i] == -1)
            fprintf(dot_file_ptr, "\"%s\";\n}\n", curr_node);
        else
            fprintf(dot_file_ptr, "\"%s\" -> ", curr_node);
    }


    fprintf(dot_file_ptr, "{\n edge [color = \"#b5b8b1\"];\n");
    for (int i = list_struct->free; i != 0 && i != -1; i = list_struct->next[i]) {
        curr_node[5] = 'a' + i;

        if (list_struct->next[i] == 0)
            fprintf(dot_file_ptr, "\"%s\";\n}\n", curr_node);
        else
            fprintf(dot_file_ptr, "\"%s\" -> ", curr_node);
    }


    //-----------------------------------------------------------------------------

    char next_node[] = "node_a";
    char prev_node[] = "node_a";
    for (int i = list_struct->next[0]; i > 0 && i < list_struct->list_size
                                          && list_struct->next[i] >= 0
                                          && list_struct->next[i] < list_struct->list_size
                                          && list_struct->prev[i] >= 0
                                          && list_struct->prev[i] < list_struct->list_size;
                                          i = list_struct->next[i]) {
        curr_node[5] = 'a' + i;
        prev_node[5] = 'a' + list_struct->prev[i];
        next_node[5] = 'a' + list_struct->next[list_struct->prev[i]];

        if (list_struct->next[list_struct->prev[i]] != i) {
            fprintf(dot_file_ptr, "%s -> %s [color = \"#a36591\", penwidth = 2];\n", prev_node, curr_node);
            fprintf(dot_file_ptr, "%s -> %s [color = \"#af4035\", penwidth = 2];\n", prev_node, next_node);

        } else {
            fprintf(dot_file_ptr, "%s -> %s [color = \"#af4035\", penwidth = 1, dir = both];\n", prev_node, curr_node);

        }
    }

    //-----------------------------------------------------------------------------

    fprintf(dot_file_ptr, "}");

    fclose(dot_file_ptr);

    char command[100] = "";
    sprintf(command, "dot %s -T png > %s", DotFileName, DotFilePngName);

ON_DEBUG(printf("Result string is %s\n", command));

    system(command);

}
