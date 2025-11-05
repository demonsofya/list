#include <stdio.h>

#include "list_ptrs.h"
//#include "TXlib.h"

int main()
{
    list_t list_1 = ListOnPointersCtor(10);

    NewElementAfter(&list_1, list_1.head, 10);
    NewElementAfter(&list_1, list_1.head, 20);
    NewElementAfter(&list_1, list_1.head, 30);

    NewElementAfter(&list_1, list_1.head, 30);

    for (ListNode_t *i = list_1.head; i != NULL; i = i->next)
        printf("[%d] ", i->data);
    printf("\n\n");
    ListDump(&list_1, __FILE__, __FUNCTION__, __LINE__, "DUMP BECAUSE OF DUMP");

    list_1.tail = NULL;
    ListDump(&list_1, __FILE__, __FUNCTION__, __LINE__, "DUMP BECAUSE OF TAIL");

    ListOnPointersDtor(&list_1);

    return 0;
}
