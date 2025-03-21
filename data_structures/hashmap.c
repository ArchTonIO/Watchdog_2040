#include "hashmap.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Hash
{
  char *key;
  char *value;
  struct Hash *next;
};

struct Bucket
{
  struct Hash *head;
  struct Hash *tail;
  uinteger len;
};

HashMap *hashmap();
uint16_t hash_key(char *key, uinteger num_buckets);
void bucket_put(struct Bucket *bucket, char *key, char *value);
void hmput(HashMap *map, char *key, char *value);
char *hmget(HashMap *map, char *key);
uinteger replace_value_at_key(HashMap *map, char *key, char *value);
void hmremove(HashMap *map, char *key);
void hmprint(HashMap *map);
void hmclear(HashMap *map);
uinteger hmlen(HashMap *map);
void bucket_remove_item(struct Bucket *bucket, char *key);
void hmdel(HashMap *map);

HashMap *hashmap()
{
  HashMap *map = (HashMap *)malloc(sizeof(HashMap));
  for (uinteger i = 0; i < 16; i++)
  {
    struct Bucket *bucket = (struct Bucket *)malloc(sizeof(struct Bucket));
    map->buckets[i] = bucket;
    map->buckets[i]->head = NULL;
    map->buckets[i]->tail = NULL;
    map->buckets[i]->len = 0;
    map->len = 0;
  }
  return map;
}

uint16_t hash_key(char *key, uinteger num_buckets)
{
  uint16_t key_id = 0;
  for (uinteger i = 0; i < strlen(key); i++)
  {
    key_id = key_id + (uinteger)key[i] + i;
  }
  return key_id % num_buckets;
}

void bucket_put(struct Bucket *bucket, char *key, char *value)
{
  struct Hash *new = (struct Hash *)malloc(sizeof(struct Hash));
  bucket->len++;
  if (bucket->tail != NULL)
  {
    bucket->tail->next = new;
  }
  new->key = key;
  new->value = value;
  new->next = NULL;
  bucket->tail = new;
  if (bucket->head == NULL)
  {
    bucket->head = new;
  }
}

void bucket_remove_item(struct Bucket *bucket, char *key)
{
  bucket->len--;
  struct Hash *cursor = bucket->head;
  struct Hash *last_traversed;
  while (cursor != NULL)
  {
    if (strcmp(cursor->key, key) == 0)
    {
      if (cursor == bucket->head)
      {
        bucket->head = cursor->next;
      }
      if (cursor == bucket->tail)
      {
        bucket->tail = last_traversed;
        bucket->tail->next = NULL;
      }
      if (cursor != bucket->head && cursor != bucket->tail)
      {
        last_traversed->next = cursor->next;
      }
      free(cursor);
      return;
    }
    last_traversed = cursor;
    cursor = cursor->next;
  }
}

void hmput(HashMap *map, char *key, char *value)
{
  if (strcmp(value, NO_MATCHES) == 0)
  {
    printf("It is illegal to use %s as a value, ignoring ...", value);
    return;
  }
  if (strcmp(hmget(map, key), NO_MATCHES) == 1)
  {
    replace_value_at_key(map, key, value);
    return;
  }
  map->len++;
  uint16_t hashed = hash_key(key, NUM_BUCKETS);
  bucket_put(map->buckets[hashed], key, value);
}

char *hmget(HashMap *map, char *key)
{
  uint16_t target_bucket = hash_key(key, NUM_BUCKETS);
  struct Hash *cursor;
  cursor = map->buckets[target_bucket]->head;
  while (cursor != NULL)
  {
    if (!strcmp(cursor->key, key))
    {
      return cursor->value;
    }
    cursor = cursor->next;
  }
  return NO_MATCHES;
}

uinteger replace_value_at_key(HashMap *map, char *key, char *value)
{
  uint16_t target_bucket = hash_key(key, NUM_BUCKETS);
  struct Hash *cursor;
  cursor = map->buckets[target_bucket]->head;
  while (cursor != NULL)
  {
    if (!strcmp(cursor->key, key))
    {
      cursor->value = value;
      return 0;
    }
    cursor = cursor->next;
  }
  return 1;
}

void hmremove(HashMap *map, char *key)
{
  if (strcmp(hmget(map, key), NO_MATCHES) == 0)
  {
    return;
  }
  map->len--;
  uint16_t hashed = hash_key(key, NUM_BUCKETS);
  bucket_remove_item(map->buckets[hashed], key);
}

void hmprint(HashMap *map)
{
  printf("lenght: %d\n", map->len);
  for (uinteger i = 0; i < NUM_BUCKETS; i++)
  {
    struct Hash *cursor;
    cursor = map->buckets[i]->head;
    while (cursor != NULL)
    {
      printf("%s: %s", cursor->key, cursor->value);
      cursor = cursor->next;
    }
  }
}

void hmclear(HashMap *map)
{
  for (uinteger i = 0; i < NUM_BUCKETS; i++)
  {
    struct Hash *cursor;
    while (cursor != NULL)
    {
      cursor->key = NULL;
      cursor->value = NULL;
      cursor = cursor->next;
    }
  }
}

uinteger hmlen(HashMap *map) { return map->len; }

void hmdel(HashMap *map)
{
  for (uinteger i = 0; i < NUM_BUCKETS; i++)
  {
    struct Bucket *bucket = map->buckets[i];
    struct Hash *cursor = bucket->head;
    while (cursor != NULL)
    {
      struct Hash *temp = cursor->next;
      free(cursor);
      cursor = temp;
    }
    free(bucket);
  }
  free(map);
}
