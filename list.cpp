#include <assert.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "list.h"

//-----------------------------------------------------------------------------

list_t ListCtor(int data_size) {

    list_t list_struct = {};

    list_struct.data = (int *) calloc(sizeof(int), data_size);
    list_struct.next = (int *) calloc(sizeof(int), data_size);
    list_struct.prev = (int *) calloc(sizeof(int), data_size);

    for (int i = 1; i < data_size; i++) {
        list_struct.next[i] = i + 1;
        list_struct.prev[i] = FREE_PREV_ELEMENT_POS;
        list_struct.data[i] = POISON;
    }

    list_struct.data[DUMMY_ELEMENT_POS] = CANARY; // TODO: DUMMY_ELEMENT_POS

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
        list_struct->prev[i] = FREE_PREV_ELEMENT_POS;
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

    return list_struct->next[DUMMY_ELEMENT_POS];
}

int GetTailPosition(list_t *list_struct) {

    assert(list_struct);

    return list_struct->prev[DUMMY_ELEMENT_POS];
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

    if (list_struct->prev[pos] == FREE_PREV_ELEMENT_POS)
        return ARG_LIST_ERROR;

    return NO_LIST_ERROR;
}


int InsertAfter(list_t *list_struct, int prev_pos, int insert_element, int *new_element_pos) {

    Return_If_Error(list_struct);

    if (CheckParamPosition(list_struct, prev_pos) != NO_LIST_ERROR) {
        ListDump(list_struct, __FILE__, __FUNCTION__, __LINE__, "INSERT ERROR");
        return INSERT_ERROR;
    }

    if (list_struct->free == DUMMY_ELEMENT_POS)
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

    if (list_struct->free == DUMMY_ELEMENT_POS)
        ReallocList(list_struct, list_struct->list_size * 2);

    int prev_pos = GetPrevPosition(list_struct, next_pos);

    return InsertElement(list_struct, prev_pos, next_pos, insert_element, new_element_pos);
}


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

    if (curr_pos <= 0 || curr_pos >= list_struct->list_size || list_struct->prev[curr_pos] == FREE_PREV_ELEMENT_POS)
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
    list_struct->prev[curr_pos] = FREE_PREV_ELEMENT_POS; //...

    if (if_element_deleted != NULL)
        *if_element_deleted = false;

    Return_Error(list_struct);
}

//=============================================================================

