#include <assert.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "list.h"

#define SEREGA LOGFILE
FILE *LOGFILE = OpenLogFile();


FILE* OpenLogFile() {

    FILE *file_ptr = fopen("ListLogFile.html", "w");
    atexit(CloseLogFile);

    return file_ptr;
}

void CloseLogFile() {

    printf("Programm ended\n");
    fclose(SEREGA);
}

list_t ListCtor(int data_size) {

    list_t list_struct = {};

    list_struct.data = (int *) calloc(sizeof(int), data_size);
    list_struct.next = (int *) calloc(sizeof(int), data_size);
    list_struct.prev = (int *) calloc(sizeof(int), data_size);

    for (int i = 1; i < data_size; i++) {
        list_struct.next[i] = i + 1;
        list_struct.prev[i] = -1;
    }

    list_struct.next[data_size - 1] = 0;

    list_struct.arr_size = data_size;
    list_struct.free = 1;

    return list_struct;
}


void ListDtor(list_t *list_struct) {

    if (list_struct == NULL)
        return;

    list_struct->arr_size = POISON;
    list_struct->free = POISON;

    free(list_struct->data);
    free(list_struct->next);
    free(list_struct->prev);

    free(list_struct);
}


void InsertAfter(list_t *list_struct, int prev_pos, int insert_element) {

    assert(list_struct);

    int curr_pos = list_struct->free;
    int next_pos = list_struct->next[prev_pos];

    list_struct->data[curr_pos] = insert_element;

    ChangePositionsAfterInsertion(list_struct, prev_pos, curr_pos, next_pos);
}


void InsertBefore(list_t *list_struct, int next_pos, int insert_element) {

    assert(list_struct);

    int curr_pos = list_struct->free;
    int prev_pos = list_struct->prev[next_pos];

    list_struct->data[curr_pos] = insert_element;

    ChangePositionsAfterInsertion(list_struct, prev_pos, curr_pos, next_pos);
}


void ChangePositionsAfterInsertion(list_t *list_struct, int prev_pos, int curr_pos, int next_pos) {

    assert(list_struct);

    list_struct->free = list_struct->next[list_struct->free];

    list_struct->next[curr_pos] = next_pos;
    list_struct->next[prev_pos] = curr_pos;

    list_struct->prev[next_pos] = curr_pos;
    list_struct->prev[curr_pos] = prev_pos;
}


void DeleteElement(list_t *list_struct, int curr_pos) {

    assert(list_struct);

    int prev_pos = list_struct->prev[curr_pos];
    int next_pos = list_struct->next[curr_pos];

    list_struct->next[curr_pos] = list_struct->free;
    list_struct->next[prev_pos] = next_pos;

    list_struct->free = curr_pos;

    list_struct->data[curr_pos] = POISON;


    list_struct->prev[next_pos] = prev_pos;
    list_struct->prev[curr_pos] = -1;
}


int ListVerify(list_t *list_struct) {

    int error = NO_LIST_ERROR;

    if (list_struct == NULL)
        return PTR_LIST_ERROR;

    if (list_struct->data == NULL)
        error |= PTR_DATA_ERROR;

    if (list_struct->arr_size <= 0)
        error |= SIZE_LIST_ERROR;

    if (list_struct->free <= 0)
        error |= FREE_LIST_ERROR;

    if (list_struct->next == NULL)
        error |= PTR_NEXT_ERROR;
    else if (list_struct->next[0] <= 0 || list_struct->next[0] >= list_struct->arr_size)
        error |= HEAD_LIST_ERROR;

    if (list_struct->prev == NULL)
        error |= PTR_PREV_ERROR;
    else if (list_struct->prev[0] <= 0 || list_struct->prev[0] >= list_struct->arr_size)
        error |= TAIL_LIST_ERROR;

    if (list_struct->next != NULL && list_struct->prev != NULL) {
        for (int i = 0; i < list_struct->arr_size; i++)
            if (list_struct->prev[list_struct->next[i]] != i) {
                error |= PREV_LIST_ERROR;
                error |= NEXT_LIST_ERROR;
                break;
            }

    }

    return error;
}


void ListDump(list_t *list_struct, const char *file_name, const char *function_name, int line_number, char *calling_reason_string) {

    int error = ListVerify(list_struct);
    static int dot_files_counter = 0;

    fprintf(SEREGA, "<h3>===========DUMP===========</h3>\n");

    fprintf(SEREGA, "<h4>ListDump() from %s at %s:%d:</h4>\n<pre>", file_name, function_name, line_number);

    if (list_struct == NULL || error == PTR_LIST_ERROR) {
        fprintf(SEREGA, "ERROR: list pointer error. List pointer is [%p]\n\n", list_struct);
        return;
    }

    fprintf(SEREGA, "List structure [%p]\n", list_struct);

    if (error & SIZE_LIST_ERROR)
        fprintf(SEREGA, "ERROR: list array size error. Size is %d\n", list_struct->arr_size);
    else
        fprintf(SEREGA, "List size is %d\n", list_struct->arr_size);

    if (error & FREE_LIST_ERROR)
        fprintf(SEREGA, "ERROR: free element position error. Position is [%d]\n\n", list_struct->free);
    else
        fprintf(SEREGA, "Free position is %d\n\n", list_struct->free);

    if (list_struct->data == NULL)
        fprintf(SEREGA, "ERROR: data array pointer error. Data pointer is [%p]\n\n", list_struct->data);
    else {
        fprintf(SEREGA, "----------DATA---------\n");
        for (int i = 0; i < list_struct->arr_size; i++) {
            fprintf(SEREGA, "[%d] = %d\n", i, list_struct->data[i]);
        }
        fprintf(SEREGA, "-----------------------\n\n");
    }

    if (list_struct->next == NULL)
        fprintf(SEREGA, "ERROR: next array pointer error. Next pointer is [%p]\n\n", list_struct->next);
    else {
        fprintf(SEREGA, "----------NEXT---------\n");
        for (int i = 0; i < list_struct->arr_size; i++) {
            fprintf(SEREGA, "[%d] ", list_struct->next[i]);
        }
        fprintf(SEREGA, "\n-----------------------\n\n");
    }

    if (list_struct->prev == NULL)
        fprintf(SEREGA, "ERROR: prev array pointer error. Prev pointer is [%p]\n\n", list_struct->next);
    else {
        fprintf(SEREGA, "----------PREV---------\n");
        for (int i = 0; i < list_struct->arr_size; i++) {
            fprintf(SEREGA, "[%d] ", list_struct->prev[i]);
        }
        fprintf(SEREGA, "\n-----------------------\n\n");
    }

    fprintf(SEREGA, "</pre>\n");

    char DotFileName[16] = "DotFileNum0.txt";
    char DotFilePngName[16] = "DotFileNum0.png";
    DotFilePngName[10] = '0' + dot_files_counter;
    DotFileName[10] = '0' + dot_files_counter++;
    FILE *dot_file_ptr = fopen(DotFileName, "w");

    fprintf(SEREGA, "<img src=\"%s\" />\n", DotFilePngName);

    fprintf(dot_file_ptr, "digraph {\nrankdir=\"LR\";\n");
    char curr_node[7] = "node_0";
    for (int i = 0; i < list_struct->arr_size; i++) {
         curr_node[5] = '0' + i;
         fprintf(dot_file_ptr, "%s [shape = \"Mrecord\", label = \"data = %d | { next = %d | prev = %d }\",style = \"filled\", fillcolor = \"#f0ceda\", color = \"#f7abc0\"]\n",
                                curr_node, list_struct->data[i], list_struct->next[i], list_struct->prev[i]);
    }

    fprintf(dot_file_ptr, "{\n edge [color = \"#f5f5dc\"];\n");
    for (int i = 0; i < list_struct->arr_size - 1; i++) {
        curr_node[5] = '0' + i;
        fprintf(dot_file_ptr, "\"%s\" -> ", curr_node);
    }
    curr_node[5] = '0' + list_struct->arr_size - 1;
    fprintf(dot_file_ptr, "\"%s\" [weight = 500];\n}\n", curr_node);


    fprintf(dot_file_ptr, "{\n edge [color = \"#6600CC\"];\n");
    for (int i = list_struct->next[0]; i != 0; i = list_struct->next[i]) {
        curr_node[5] = '0' + i;
        if (list_struct->next[i] == 0)
            fprintf(dot_file_ptr, "\"%s\" [weight = 10];\n}\n", curr_node);
        else
            fprintf(dot_file_ptr, "\"%s\" -> ", curr_node);
    }

    fprintf(dot_file_ptr, "}");
}
