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

    fprintf(dot_file_ptr, "digraph {\nrankdir=\"LR\";\n");
    fprintf(dot_file_ptr, "node [shape = \"doubleoctagon\", style = \"filled\", fillcolor = \"#FF6666\", color = \"#CC0000\", fontcolor = white, fontsize=20, margin=0.1];\n");
    char curr_node[] = DEFAULT_NODE_NAME;

    DrawDotNodes(list_struct, dot_file_ptr);
    DrawDotEdges(list_struct, dot_file_ptr);

    fprintf(dot_file_ptr, "}");

    fclose(dot_file_ptr);

    char command[100] = "";
    sprintf(command, "dot %s -T png > %s", DotFileName, DotFilePngName);

ON_DEBUG(printf("Result string is %s\n", command));

    system(command);

}

//-----------------------------------------------------------------------------

void DrawDotEdges(list_t *list_struct, FILE *dot_file_ptr) {

    if (list_struct == NULL || dot_file_ptr == NULL)
        return;

    char curr_node[] = DEFAULT_NODE_NAME;

    fprintf(dot_file_ptr, "{\n edge [color = \"#f5f5dc\", dir=none];\n");
    for (int i = 0; i < list_struct->list_size - 1; i++) {
        ChangeNodeName(i, curr_node);
        fprintf(dot_file_ptr, "\"%s\" -> ", curr_node);
    }
    ChangeNodeName(list_struct->list_size - 1, curr_node);
    fprintf(dot_file_ptr, "\"%s\" [weight = 1000000];\n}\n", curr_node);

    /*
    fprintf(dot_file_ptr, "{\n edge [color = \"#6600CC\", penwidth = 0.2, arrowhead = dot];\n");
    for (int i = list_struct->next[0]; i > 0 && i < list_struct->list_size; i = list_struct->next[i]) {
        ChangeNodeName(i, curr_node);

        if (list_struct->next[i] <= 0 || list_struct->next[i] >= list_struct->list_size)
            fprintf(dot_file_ptr, "\"%s\";\n}\n", curr_node);
        else
            fprintf(dot_file_ptr, "\"%s\" -> ", curr_node);
    }


    fprintf(dot_file_ptr, "{\n edge [color = \"#523f49\", penwidth = 0.2, arrowhead = dot];\n");
    for (int i = list_struct->prev[0]; i > 0 && i < list_struct->list_size; i = list_struct->prev[i]) {
        ChangeNodeName(i, curr_node);

        if (list_struct->prev[i] <= 0 || list_struct->prev[i] >= list_struct->list_size)
            fprintf(dot_file_ptr, "\"%s\";\n}\n", curr_node);
        else
            fprintf(dot_file_ptr, "\"%s\" -> ", curr_node);
    }
    */

    fprintf(dot_file_ptr, "{\n edge [color = \"#4C9900\"];\n");
    for (int i = list_struct->free; i > 0 && i < list_struct->list_size; i = list_struct->next[i]) {
        ChangeNodeName(i, curr_node);

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

    char curr_node[] = DEFAULT_NODE_NAME;
    char prev_node[] = DEFAULT_NODE_NAME;
    char next_node[] = DEFAULT_NODE_NAME;

    for (int i = 1; i < list_struct->list_size; i++) {

        int next_pos = list_struct->next[i];

        ChangeNodeName(i, curr_node);
        ChangeNodeName(next_pos, next_node);

        if (list_struct->next[i] < 0 || list_struct->next[i] >= list_struct->list_size) {
            char curr_pos[10] = "";
            snprintf(curr_pos, 10, "%d", next_pos);
            DrawCurrEdge(dot_file_ptr, curr_node, curr_pos, "CC0000", 4, "forward");
            //fprintf(dot_file_ptr, "%s -> %d [color = \"#CC0000\", penwidth = 4];\n", curr_node, next_pos);
            continue;
        }

        int prev_pos = list_struct->prev[next_pos];
        ChangeNodeName(prev_pos, prev_node);

        if (prev_pos == i && i != 0 && next_pos != 0)
            DrawCurrEdge(dot_file_ptr, curr_node, next_node, "6666FF", 1, "both");
            //fprintf(dot_file_ptr, "%s -> %s [color = \"#6666FF\", penwidth = 1, dir = both];\n", curr_node, next_node);

        else if (prev_pos >= 0 && prev_pos < list_struct->list_size && next_pos > 0 && next_pos < list_struct->list_size) {
            DrawCurrEdge(dot_file_ptr, next_node, prev_node, "990000", 2, "forward");
            DrawCurrEdge(dot_file_ptr, curr_node, next_node, "990000", 2, "forward");
            //fprintf(dot_file_ptr, "%s -> %s [color = \"#990000\", penwidth = 2, dir = forward];\n", next_node, prev_node);
            //fprintf(dot_file_ptr, "%s -> %s [color = \"#990000\", penwidth = 2, dir = forward];\n", curr_node, next_node);

        }
    }

    /*
    for (int i = list_struct->next[0]; i > 0 && i < list_struct->list_size
                                          && list_struct->next[i] >= 0
                                          && list_struct->next[i] < list_struct->list_size
                                          && list_struct->prev[i] >= 0
                                          && list_struct->prev[i] < list_struct->list_size;
                                          i = list_struct->next[i]) {
        ChangeNodeName(i, curr_node);
        ChangeNodeName(list_struct->prev[i], prev_node);
        ChangeNodeName(list_struct->next[list_struct->prev[i]], next_node);

        if (list_struct->next[list_struct->prev[i]] != i) {
            fprintf(dot_file_ptr, "%s -> %s [color = \"#009b76\", penwidth = 2];\n", prev_node, curr_node);
            fprintf(dot_file_ptr, "%s -> %s [color = \"#1faee9\", penwidth = 2];\n", prev_node, next_node);

        } else {
            fprintf(dot_file_ptr, "%s -> %s [color = \"#af4035\", penwidth = 1, dir = both];\n", prev_node, curr_node);

        }
    }
    */
}

void DrawCurrEdge(FILE *dot_file_ptr, const char* first_node, const char *second_node,
                  const char* color, int pen_width, const char *dir_type) {

    if (first_node == NULL || second_node == NULL || color == NULL || dir_type == NULL)
        return;

    fprintf(dot_file_ptr, "%s -> %s [color = \"#%s\", penwidth = %d, dir = %s];\n",
                           first_node, second_node, color, pen_width, dir_type);

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
    snprintf(curr_node_args.label, 100, "data = %d | index = %d | { HEAD = %d | TAIL = %d }",
                         list_struct->data[0], 0, list_struct->next[0], list_struct->prev[0]);

    curr_node_args.fill_color = "baacc7";
    curr_node_args.color = "876f9e";
    curr_node_args.rank_num = 0;
    //curr_node_args.node_name = DEFAULT_NODE_NAME;

    DrawCurrNode(dot_file_ptr, &curr_node_args, curr_node);
    //fprintf(dot_file_ptr, "%s [shape = \"Mrecord\", label = \"data = %d | index = %d | { HEAD = %d | TAIL = %d }"
    //                       "\", style = \"filled\", fillcolor = \"#baacc7\", color = \"#876f9e\", rank = %d, fontcolor = black]\n",
    //                               curr_node, list_struct->data[0], 0, list_struct->next[0], list_struct->prev[0], 0);

    for (int i = 1; i < list_struct->list_size; i++) {
         ChangeNodeName(i, curr_node);
         curr_node_args.rank_num = i;

         curr_node_args.fill_color = "99CCFF";
         curr_node_args.color = "0066CC";

         snprintf(curr_node_args.label, 100, "data = %d | index =  %d | { next = %d | prev = %d }",
                                             list_struct->data[i], i, list_struct->next[i], list_struct->prev[i]);

         if (i == list_struct->free) {
            snprintf(curr_node_args.label, 100, "FREE | data = %d | index = %d | { next = %d | prev = %d }",
                                                list_struct->data[i], i, list_struct->next[i], list_struct->prev[i]);
            curr_node_args.fill_color = "ccccff";
            curr_node_args.color = "9999ff";

            DrawCurrNode(dot_file_ptr, &curr_node_args, curr_node);
            //fprintf(dot_file_ptr, "%s [shape = \"Mrecord\", label = \"FREE | data = %d | index = %d | { next = %d | prev = %d }"
            //                      "\", style = \"filled\", fillcolor = \"#ccccff\", color = \"#9999ff\", rank = %d, fontcolor = black]\n",
            //                       curr_node, list_struct->data[i], i, list_struct->next[i], list_struct->prev[i], i);
        }

         else if (i == list_struct->next[0]) {
            DrawCurrNode(dot_file_ptr, &curr_node_args, curr_node);
            //snprintf(curr_node_args.label, 100, "data = %d | index =  %d | { next = %d | prev = %d }",
            //                                    list_struct->data[i], i, list_struct->next[i], list_struct->prev[i]);
            //curr_node_args.fill_color = "99CCFF";
            //curr_node_args.color = "0066CC";

            //fprintf(dot_file_ptr, "%s [shape = \"Mrecord\", label = \"data = %d | index =  %d | { next = %d | prev = %d }"
            //                      "\", style = \"filled\", fillcolor = \"#99CCFF\", color = \"#0066CC\", rank = %d, fontcolor = black]\n",
            //                       curr_node, list_struct->data[i], i, list_struct->next[i], list_struct->prev[i], i);
            fprintf(dot_file_ptr, "head_node [shape = \"Mrecord\", label = \"HEAD"
                                  "\", style = \"filled\", fillcolor = \"#e3cced\", color = \"#cfa6e0\", rank = \"same\", fontcolor = black]\n",
                                  i + 1);

            fprintf(dot_file_ptr, "head_node -> %s [color = \"#cfa6e0\"];\n", curr_node);


         }

         else if (i == list_struct->prev[0]) {
            DrawCurrNode(dot_file_ptr, &curr_node_args, curr_node);
            //snprintf(curr_node_args.label, 100, "data = %d | index =  %d | { next = %d | prev = %d }",
            //                                    list_struct->data[i], i, list_struct->next[i], list_struct->prev[i]);
            //curr_node_args.fill_color = "99CCFF";
            //curr_node_args.color = "0066CC";

            //fprintf(dot_file_ptr, "%s [shape = \"Mrecord\", label = \"data = %d | index =  %d | { next = %d | prev = %d }"
            //                      "\", style = \"filled\", fillcolor = \"#99CCFF\", color = \"#0066CC\", rank = %d, fontcolor = black]\n",
            //                       curr_node, list_struct->data[i], i, list_struct->next[i], list_struct->prev[i], i);
            fprintf(dot_file_ptr, "tail_node [shape = \"Mrecord\", label = \"TAIL"
                                  "\", style = \"filled\", fillcolor = \"#e3cced\", color = \"#cfa6e0\", rank = \"same\", fontcolor = black]\n");

            fprintf(dot_file_ptr, "tail_node -> %s [color = \"#cfa6e0\"];\n", curr_node);


         }

         else if (list_struct->prev[i] == -1) {
            snprintf(curr_node_args.label, 100, "data = %d [POISON] | index =  %d | { next = %d | prev = %d }",
                                                list_struct->data[i], i, list_struct->next[i], list_struct->prev[i]);
            curr_node_args.fill_color = "CCFF99";
            curr_node_args.color = "66CC00";
            //fprintf(dot_file_ptr, "%s [shape = \"Mrecord\", label = \"data = %d [POISON] | index =  %d | { next = %d | prev = %d }"
            //                       "\", style = \"filled\", fillcolor = \"#CCFF99\", color = \"#66CC00\", rank = %d, fontcolor = black]\n",
            //                       curr_node, list_struct->data[i], i, list_struct->next[i], list_struct->prev[i], i);
            DrawCurrNode(dot_file_ptr, &curr_node_args, curr_node);

        }

         else
            DrawCurrNode(dot_file_ptr, &curr_node_args, curr_node);

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

