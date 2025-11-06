#include <assert.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "list.h"

//#include "TXlib.h"

//=============================================================================

#define SEREGA LOGFILE
FILE *LOGFILE = OpenLogFile();

#define DEFAULT_NODE_NAME "node_0"
#define DEFAULT_DOT_TXT_FILE_NAME "DotFileNum_0.txt"
#define DEFAULT_DOT_PNG_FILE_NAME "DotFileNum_0.png"

const char DEFAULT_LOG_FILE_NAME[] = "ListLogFile.html";

const int MAX_DOT_NAME_SIZE      = 100;
const int MAX_DOT_FILE_NAME_SIZE = 100;
const int MAX_LABEL_STRING_SIZE  = 100;
const int MAX_DOT_COMMAND_SIZE   = 100;

const char *DARK_RED_COLOR             = "990000";
const char *PURPLE_BLUE_COLOR          = "6666FF";
const char *LIGHT_PURPLE_PINK_COLOR    = "baacc7";
const char *SOFT_PURPLE_COLOR          = "876f9e";
const char *LIGHT_BLUE_COLOR           = "99CCFF";
const char *DEEP_BLUE_COLOR            = "0066cc";
const char *ERROR_RED_COLOR            = "CC0000";
const char *LIGHT_PASTEL_RED_COLOR     = "FF6666";
const char *HOLY_WIGHT_COLOR           = "f5f5dc";
const char *GRASS_GREEN_COLOR          = "4C9900";
const char *LAVANDER_BLUE_COLOR        = "ccccff";
const char *COBALITE_COLOR             = "9999ff";
const char *LIGHT_YELLOW_GREEN_COLOR   = "CCFF99";
const char *PASTEL_PURPLE_PINK_COLOR   = "cfa6e0";
const char *LIGHT_GRAY_PURPLE_COLOR    = "e3cced";
const char *BRIGHT_GREEN_COLOR         = "66CC00";

//=============================================================================


FILE* OpenLogFile() {

    FILE *file_ptr = fopen(DEFAULT_LOG_FILE_NAME, "w");
    atexit(CloseLogFile);

    return file_ptr;
}

//-----------------------------------------------------------------------------

void CloseLogFile() {

    printf("Programm ended\n");
    fclose(SEREGA);
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
            if (list_struct->next[i] < list_struct->list_size && list_struct->next[i] >= 0)
                if (list_struct->prev[list_struct->next[i]] != i && list_struct->prev[i] != -1) {
                    error |= PREV_NEXT_ERROR;
                    break;
                }

        int counter = 0;
        for (int i = GetHeadPosition(list_struct); i > 0 && i < list_struct->list_size
                     && counter <= list_struct->list_size; i = list_struct->next[i])
            counter++;
        if (counter > list_struct->list_size)
            error |= NEXT_LIST_ERROR;

        counter = 0;
        for (int i = GetTailPosition(list_struct); i > 0 && i < list_struct->list_size
                     && counter <= list_struct->list_size; i = list_struct->prev[i])
            counter++;
        if (counter > list_struct->list_size)
            error |= PREV_LIST_ERROR;

    }

    return error;
}

//-----------------------------------------------------------------------------


void ListDump(list_t *list_struct, const char *file_name, const char *function_name,
              int line_number, const char *calling_reason_string) {

    ListDumpPrintErrors(list_struct, file_name, function_name, line_number, calling_reason_string);

    CreateDumpGraphFile(list_struct);
}

//=============================================================================

void ListDumpPrintErrors(list_t *list_struct, const char *file_name, const char *function_name,
                         int line_number, const char *calling_reason_string) {

    int error = ListVerify(list_struct);

    fprintf(SEREGA, "<h2><font color=\"#%s\"><p align=\"center\">===========DUMP==========="
                    "</p></font></h2>\n<h3><p align=\"center\">%s</p></h3>\n\n",
                    ERROR_RED_COLOR, calling_reason_string);

    fprintf(SEREGA, "<h4>ListDump() from %s at %s:%d:</h4>\n<pre>", file_name, function_name,
                                                                    line_number);

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

    ListDumpPrintListArraysErrors(list_struct, error);

    fprintf(SEREGA, "</pre>\n");
}

//-----------------------------------------------------------------------------

void ListDumpPrintListArraysErrors(list_t *list_struct, int error) {

    if (list_struct == NULL)
        return;

    if (list_struct->data == NULL)
        fprintf(SEREGA, "ERROR: data array pointer error. Data pointer is [%p]\n\n", list_struct->data);
    else {
        fprintf(SEREGA, "----------DATA---------\n");
        for (int i = 0; i < list_struct->list_size; i++) {
            fprintf(SEREGA, "[%d] = %d\n", i, list_struct->data[i]);
        }
        fprintf(SEREGA, "-----------------------\n\n");
    }


    if (error & PREV_NEXT_ERROR)
        fprintf(SEREGA, "ERROR: next and prev arrays are different \n");


    if (error & NEXT_LIST_ERROR)
        fprintf(SEREGA, "ERROR: next array error \n");

    if (list_struct->next == NULL)
        fprintf(SEREGA, "ERROR: next array pointer error. Next pointer is [%p]\n\n", list_struct->next);
    else {
        fprintf(SEREGA, "\n----------NEXT---------\n");
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
}

//=============================================================================

void CreateDumpGraphFile(list_t *list_struct) {

    if (list_struct == NULL)
        return;

    char *DotFileName = CreateDotFileName("txt");
    char *DotFilePngName = CreateDotFileName("png");

    if (DotFileName == NULL || DotFilePngName == NULL) {
        fprintf(SEREGA, "ERROR: pointer dot file name error\n\n");
        return;    // TODO: ’”À» Õ≈ –¿¡Œ“¿≈“ Õ»’”ﬂ???
    }


    FILE *dot_file_ptr = fopen(DotFileName, "w");

    if (dot_file_ptr == NULL) {
        fprintf(SEREGA, "ERROR: dot file pointer error\n\n");
        return;
    }

    PrintDotFileHeader(dot_file_ptr, DotFilePngName);
    DrawDotNodes(list_struct, dot_file_ptr);
    DrawDotEdges(list_struct, dot_file_ptr);
    PrintDotFileEnd(dot_file_ptr);

    fclose(dot_file_ptr);

    CreateImageFromDotFile(DotFileName, DotFilePngName);
}

//-----------------------------------------------------------------------------

void PrintDotFileHeader(FILE *dot_file_ptr, char *DotFilePngName) {

    assert(dot_file_ptr);
    assert(DotFilePngName);

    fprintf(SEREGA, "<img src=\"%s\" />\n", DotFilePngName);

    fprintf(dot_file_ptr, "digraph {\nrankdir=\"LR\";\n");
    fprintf(dot_file_ptr, "node [shape = \"doubleoctagon\", style = \"filled\", fillcolor = \"#%s\""
                          ", color = \"#%s\", fontcolor = white, fontsize=20, margin=0.1];\n", LIGHT_PASTEL_RED_COLOR, ERROR_RED_COLOR);
}

//-----------------------------------------------------------------------------

void PrintDotFileEnd(FILE *dot_file_ptr) {

    if (dot_file_ptr == NULL)
        return;

    fprintf(dot_file_ptr, "}");
}

//-----------------------------------------------------------------------------

void CreateImageFromDotFile(char *DotFileName, char *DotFilePngName) {

    if (DotFileName == NULL || DotFilePngName == NULL)
        return;

    char command[MAX_DOT_COMMAND_SIZE] = "";
    snprintf(command, MAX_DOT_COMMAND_SIZE, "dot %s -T png > %s", DotFileName, DotFilePngName);

    system(command);

    ON_DEBUG(fprintf(stderr, "%s\n", command));
}

//-----------------------------------------------------------------------------

void DrawDotEdges(list_t *list_struct, FILE *dot_file_ptr) {

    if (list_struct == NULL || dot_file_ptr == NULL)
        return;

    char *curr_node = NULL;

    fprintf(dot_file_ptr, "{\n edge [color = \"#%s\", dir=none];\n", HOLY_WIGHT_COLOR);
    for (int i = 0; i < list_struct->list_size - 1; i++) {
        curr_node = GetNodeName(i);
        fprintf(dot_file_ptr, "\"%s\" -> ", curr_node);
    }

    curr_node = GetNodeName(list_struct->list_size - 1);
    fprintf(dot_file_ptr, "\"%s\" [weight = 1000000];\n}\n", curr_node);

    fprintf(dot_file_ptr, "{\n edge [color = \"#%s\"];\n", GRASS_GREEN_COLOR);
    for (int i = list_struct->free; i > 0 && i < list_struct->list_size; i = list_struct->next[i]) {
        curr_node = GetNodeName(i);

        if (list_struct->next[i] <= 0 || list_struct->next[i] >= list_struct->list_size)
            fprintf(dot_file_ptr, "\"%s\";\n}\n", curr_node);
        else
            fprintf(dot_file_ptr, "\"%s\" -> ", curr_node);
    }

    DrawBothDirEdges(list_struct, dot_file_ptr);

}

//-----------------------------------------------------------------------------

void DrawBothDirEdges(list_t *list_struct, FILE *dot_file_ptr) {

    if (list_struct == NULL || dot_file_ptr == NULL)
        return;

    for (int i = 1; i < list_struct->list_size; i++) {

        int next_pos = list_struct->next[i];

        char *curr_node = GetNodeName(i);
        char *next_node = GetNodeName(next_pos);
        if (curr_node == NULL || next_node == NULL)
            return;

        if (list_struct->next[i] < 0 || list_struct->next[i] >= list_struct->list_size) {
            char curr_pos[MAX_DOT_NAME_SIZE] = "";
            snprintf(curr_pos, MAX_DOT_NAME_SIZE, "%d", next_pos);
            DrawCurrEdge(dot_file_ptr, curr_node, curr_pos, ERROR_RED_COLOR, 4, "forward");

            continue;
        }

        int prev_pos = list_struct->prev[next_pos];
        char *prev_node = GetNodeName(prev_pos);
        if (prev_node == NULL)
            return;

        if (prev_pos == i && i != 0 && next_pos != 0)
            DrawCurrEdge(dot_file_ptr, curr_node, next_node, PURPLE_BLUE_COLOR, 1, "both");

        else if (prev_pos >= 0 && prev_pos < list_struct->list_size && next_pos > 0
                               && next_pos < list_struct->list_size) {
            DrawCurrEdge(dot_file_ptr, next_node, prev_node, DARK_RED_COLOR, 2, "forward");
            DrawCurrEdge(dot_file_ptr, curr_node, next_node, DARK_RED_COLOR, 2, "forward");      // TODO: ÷¬≈“ ’”… «Õ¿≈“  ¿ Œ…

        }
    }
}

//-----------------------------------------------------------------------------

void DrawCurrEdge(FILE *dot_file_ptr, const char* first_node, const char *second_node,
                  const char* color, int pen_width, const char *dir_type) {

    if (first_node == NULL || second_node == NULL || color == NULL || dir_type == NULL)
        return;

    fprintf(dot_file_ptr, "%s -> %s [color = \"#%s\", penwidth = %d, dir = %s];\n",
                           first_node, second_node, color, pen_width, dir_type);

}


//=============================================================================

node_args_t *NodeArgsCtor(const char *fill_color, const char *color, int rank_num) {

    if (fill_color == NULL || color == NULL)
        return NULL;

    node_args_t *curr_node_args = (node_args_t *) calloc(1, sizeof(node_args_t));
    if (curr_node_args == NULL)
        return NULL;

    SetNodeProperties(curr_node_args, fill_color, color, rank_num);

    curr_node_args->label = (char *) calloc(MAX_LABEL_STRING_SIZE, sizeof(char));

    return curr_node_args;
}

void SetNodeProperties(node_args_t *curr_node_args, const char *fill_color, const char *color, int rank_num) {

    assert(curr_node_args);
    assert(fill_color);
    assert(color);

    curr_node_args->fill_color = fill_color;
    curr_node_args->color = color;
    curr_node_args->rank_num = rank_num;
}

void NodeArgsDtor(node_args_t *curr_node_args) {

    if (curr_node_args == NULL)
        return;

    free(curr_node_args->label);
    free(curr_node_args);
}

//-----------------------------------------------------------------------------

void DrawDotNodes(list_t *list_struct, FILE *dot_file_ptr) {

    if (list_struct == NULL || dot_file_ptr == NULL)
        return;

    char *curr_node = GetNodeName(0);

    node_args_t *curr_node_args = NodeArgsCtor(LIGHT_PURPLE_PINK_COLOR, SOFT_PURPLE_COLOR, 0);
    if (curr_node_args == NULL)
        return;
    // ‚˚ÌÂÒÚË ÒÓÁ‰‡ÌËÂ ÌÛÎÂ‚Ó„Ó ÌÓ‰‡ ‚ ÙÛÌÍˆË˛˚
    snprintf(curr_node_args->label, MAX_LABEL_STRING_SIZE, "data = %d | index = %d"
                                                          "| { HEAD = %d | TAIL = %d }",
                                   list_struct->data[0], 0,
                                   list_struct->next[DUMMY_ELEMENT_POS], list_struct->prev[DUMMY_ELEMENT_POS]); // TODO: const

    DrawCurrNode(dot_file_ptr, curr_node_args, curr_node);

    for (int i = 1; i < list_struct->list_size; i++) {
         curr_node = GetNodeName(i);

         SetNodeProperties(curr_node_args, LIGHT_BLUE_COLOR, DEEP_BLUE_COLOR, i); // TODO: SetNodeProperties

         snprintf(curr_node_args->label, MAX_LABEL_STRING_SIZE,
                                        "data = %d | index =  %d | { next = %d | prev = %d }",
                                        list_struct->data[i], i, list_struct->next[i],
                                        list_struct->prev[i]);

         if (i == list_struct->free) {
            snprintf(curr_node_args->label, MAX_LABEL_STRING_SIZE,
                                           "FREE | data = %d | index = %d | { next = %d | prev = %d }",
                                           list_struct->data[i], i, list_struct->next[i],
                                           list_struct->prev[i]);

            curr_node_args->fill_color = LAVANDER_BLUE_COLOR;
            curr_node_args->color = COBALITE_COLOR;

         }


        else if (list_struct->prev[i] == FREE_PREV_ELEMENT_POS) {
            snprintf(curr_node_args->label, MAX_LABEL_STRING_SIZE,
                                           "data = %d [POISON] | index =  %d | { next = %d | prev = %d }",
                                           list_struct->data[i], i, list_struct->next[i],
                                           list_struct->prev[i]);

            curr_node_args->fill_color = LIGHT_YELLOW_GREEN_COLOR;
            curr_node_args->color = BRIGHT_GREEN_COLOR;

        }

        DrawCurrNode(dot_file_ptr, curr_node_args, curr_node);
    }

    DrawHeadAndTail(list_struct, dot_file_ptr);

    NodeArgsDtor(curr_node_args);
}

void DrawHeadAndTail(list_t *list_struct, FILE *dot_file_ptr) {

    if (list_struct == NULL || dot_file_ptr == NULL)
        return;

    char *head_node = GetNodeName(list_struct->next[0]);
    fprintf(dot_file_ptr, "head_node [shape = \"Mrecord\", label = \"HEAD"
                          "\", style = \"filled\", fillcolor = \"#%s\", color = \"#%s\", "
                          "rank = \"same\", fontcolor = black]\n", LIGHT_GRAY_PURPLE_COLOR, PASTEL_PURPLE_PINK_COLOR);

    fprintf(dot_file_ptr, "head_node -> %s [color = \"#%s\"];\n", head_node, PASTEL_PURPLE_PINK_COLOR);


    char *tail_node = GetNodeName(list_struct->prev[0]);
    fprintf(dot_file_ptr, "tail_node [shape = \"Mrecord\", label = \"TAIL"
                          "\", style = \"filled\", fillcolor = \"#%s\", "
                          "color = \"#%s\", rank = \"same\", fontcolor = black]\n",
                          LIGHT_GRAY_PURPLE_COLOR, PASTEL_PURPLE_PINK_COLOR);

    fprintf(dot_file_ptr, "tail_node -> %s [color = \"#%s\"];\n", tail_node, PASTEL_PURPLE_PINK_COLOR);
}

//-----------------------------------------------------------------------------

void DrawCurrNode(FILE *dot_file_ptr, node_args_t *curr_node_args, const char *node_name) {

    if (dot_file_ptr == NULL || curr_node_args == NULL || node_name == NULL)
        return;

    fprintf(dot_file_ptr, "%s [shape = \"Mrecord\", label = \"%s\", style = \"filled\", "
                          "fillcolor = \"#%s\", color = \"#%s\", rank = %d, fontcolor = black]\n",
                          node_name, curr_node_args->label, curr_node_args->fill_color,
                          curr_node_args->color, curr_node_args->rank_num);

}


//=============================================================================

char *GetNodeName(int node_num) {

    char *node = (char *) calloc(MAX_DOT_NAME_SIZE, sizeof(char));
    if (node == NULL)
        return node;

    snprintf(node, MAX_DOT_NAME_SIZE, "node_%d", node_num);

    return node;
}

//-----------------------------------------------------------------------------

char *CreateDotFileName(const char *file_type) {

    static int dot_file_number = 0;

    char *file_name = (char *) calloc(MAX_DOT_FILE_NAME_SIZE, sizeof(char));

    snprintf(file_name, MAX_DOT_FILE_NAME_SIZE, "DotFileNum_%d.%s", dot_file_number, file_type);

    dot_file_number++;

    return file_name;
}

