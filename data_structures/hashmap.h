#include <stdint.h>
#ifndef HASHMAP_H
#define HASHMAP_H

#ifndef uinteger
#define uinteger uint8_t
#endif
#ifndef integer
#define integer int8_t
#endif

#define NUM_BUCKETS 16
#define NO_MATCHES "__no_matches__"

typedef struct
{
  struct Bucket *buckets[NUM_BUCKETS];
  uinteger len;
} HashMap;

HashMap *hashmap();
void hmput(HashMap *map, char *key, char *value);
char *hmget(HashMap *map, char *key);
void hmremove(HashMap *map, char *key);
void hmprint(HashMap *map);
void hmclear(HashMap *map);
uinteger hmlen(HashMap *map);
void hmdel(HashMap *map);
#endif
