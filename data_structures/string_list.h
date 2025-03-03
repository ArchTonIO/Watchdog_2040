#include <stdint.h>
#ifndef STRING_LIST_H
#define STRING_LIST_H

#ifndef uinteger
#define uinteger uint16_t
#endif
#ifndef integer
#define integer int32_t
#endif

#define MAX_STR_LEN 65535
#define MAX_LIST_LEN 65535

typedef struct
{
	uinteger len;
	struct lnode *head;
	struct lnode *tail;
} str_list;
str_list *list();
void lstappend(str_list *list, char *value);
integer index_of(str_list *list, char *value);
char *lstget(str_list *list, integer index);
char *lstpop(str_list *list, integer index);
void lstprint(str_list *list);
void lstclear(str_list *list);
uinteger lstlen(str_list *list);
uinteger lstcmp(str_list *list1, str_list *list2);
char *lstconcat(str_list *list);
str_list *lstextend(str_list *list1, str_list *list2);
str_list *lstcopy(str_list *to_copy);
void lstdel(str_list *list);

#endif