#include <stdio.h>

#include "list.h"

int main()
{
// Это все отладка
// Надо на половине пути проверить работает ли прога
    list_t list_1 = ListCtor(10);

    InsertAfter(&list_1, 0, 10, 0);
    InsertAfter(&list_1, 1, 20, 0);
    InsertAfter(&list_1, 2, 30, 0);

    for (int i = 1; i != 0; i = list_1.next[i])
        printf("[%d] ", list_1.data[i]);
    printf("\n\n");

    ListDump(&list_1, __FILE__, __FUNCTION__, __LINE__, "DUMP BEFORE INSERT");

    InsertAfter(&list_1, 2, 40, 0);
    InsertAfter(&list_1, 2, 50, 0);
    InsertAfter(&list_1, 2, 60, 0);

    ListDump(&list_1, __FILE__, __FUNCTION__, __LINE__, "DUMP AFTER INSERT");

    DeleteElement(&list_1, 2, 0);

    ListDump(&list_1, __FILE__, __FUNCTION__, __LINE__, "DUMP AFTER DELETE");

    InsertAfter(&list_1, 3, 70, 0);
    InsertBefore(&list_1, 1, 80, 0);
    InsertAfter(&list_1, 3, 90, 0);
    InsertBefore(&list_1, 1, 100, 0);
    InsertAfter(&list_1, 3, 110, 0);
    InsertBefore(&list_1, 1, 120, 0);
    InsertAfter(&list_1, 3, 130, 0);
    InsertBefore(&list_1, 1, 140, 0);

    ListDump(&list_1, __FILE__, __FUNCTION__, __LINE__, "DUMP AFTER INSERT AFTER TAIL AND BEFORE HEAD");

    for (int i = GetHeadPosition(&list_1); i != 0; i = list_1.next[i])
        printf("[%d] ", list_1.data[i]);
    printf("\n\n");

    return 0;
}
