#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NULL_MAP_SIZE -1

typedef struct node_t
{
    MapDataElement data;
    MapKeyElement key;
    struct node_t *previous;
    struct node_t *next;
} * Node;

struct Map_t
{
    copyMapDataElements copyData;
    copyMapKeyElements copyKey;
    freeMapDataElements freeData;
    freeMapKeyElements freeKey;
    compareMapKeyElements compareKey;
    Node first;
    Node current;
    int counter;
};

Node createNode(Map map, MapDataElement data, MapKeyElement key,
                Node previous, Node next);
Node findKey(Map map, MapKeyElement keyElement);

Node createNode(Map map, MapDataElement data, MapKeyElement key,
                Node previous, Node next)
{
    Node new_node = malloc(sizeof(*new_node));
    if (new_node == NULL)
    {
        return NULL;
    }
    new_node->data = map->copyData(data);
    if (new_node->data == NULL)
    {
        free(new_node);
        return NULL;
    }
    new_node->key = map->copyKey(key);
    if (new_node->key == NULL)
    {
        map->freeData(new_node->data);
        free(new_node);
        return NULL;
    }
    new_node->next = next;
    new_node->previous = previous;
    return new_node;
}

Node findKey(Map map, MapKeyElement keyElement)
{
    if (map == NULL || keyElement == NULL)
    {
        return NULL;
    }
    map->current = map->first->next;
    while (map->current != NULL)
    {
        if (map->compareKey(map->current->key, keyElement) == 0)
        {
            return map->current;
        }
        map->current = map->current->next;
    }
    return NULL;
}

Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements)
{
    if (copyDataElement == NULL || copyKeyElement == NULL ||
        freeDataElement == NULL || freeKeyElement == NULL ||
        compareKeyElements == NULL)
    {
        return NULL;
    }
    Map map = malloc(sizeof(*map));
    if (map == NULL)
    {
        return NULL;
    }
    map->copyData = copyDataElement;
    map->copyKey = copyKeyElement;
    map->freeData = freeDataElement;
    map->freeKey = freeKeyElement;
    map->compareKey = compareKeyElements;
    map->first = malloc(sizeof(*map->first));
    if (map->first == NULL)
    {
        free(map);
        return NULL;
    }
    map->first->data = NULL;
    map->first->key = NULL;
    map->first->next = NULL;
    map->first->previous = NULL;
    map->current = map->first;
    map->counter = 0;
    return map;
}

void mapDestroy(Map map)
{
    if (map != NULL)
    {
        mapClear(map);
        free(map->first);
        free(map);
    }
}

Map mapCopy(Map map)
{
    if (map == NULL)
    {
        return NULL;
    }
    Map new_map = mapCreate(map->copyData, map->copyKey,
                            map->freeData, map->freeKey,
                            map->compareKey);
    if (new_map == NULL)
    {
        return NULL;
    }
    MapResult put;
    map->current = map->first->next;
    while (map->current != NULL)
    {
        put = mapPut(new_map, map->current->key, map->current->data);
        if (put != MAP_SUCCESS)
        {
            mapDestroy(new_map);
            return NULL;
        }
        map->current = map->current->next;
    }
    new_map->counter = map->counter;
    return new_map;
}

int mapGetSize(Map map)
{
    if (map == NULL)
    {
        return NULL_MAP_SIZE;
    }
    return map->counter;
}

bool mapContains(Map map, MapKeyElement element)
{
    if (map == NULL || element == NULL)
    {
        return false;
    }
    return findKey(map, element) != NULL;
}

MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    if (map == NULL || keyElement == NULL || dataElement == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }
    map->current = findKey(map, keyElement);
    if (map->current == NULL)
    {
        map->current = map->first->next;
        Node previous = map->first;
        while (map->current != NULL)
        {
            if (map->compareKey(map->current->key, keyElement) > 0)
            {
                Node new_node = createNode(map, dataElement, keyElement,
                                           map->current->previous, map->current);
                if (new_node == NULL)
                {
                    return MAP_OUT_OF_MEMORY;
                }
                map->current->previous->next = new_node;
                map->current->previous = new_node;
                map->counter++;
                return MAP_SUCCESS;
            }
            previous = map->current;
            map->current = map->current->next;
        }
        map->current = createNode(map, dataElement, keyElement,
                                  previous, NULL);
        if (map->current == NULL)
        {
            return MAP_OUT_OF_MEMORY;
        }
        previous->next = map->current;
        map->current = map->current->next; // move indicator
        map->counter++;
    }
    else
    {
        MapDataElement tmp_data = map->current->data;
        map->current->data = map->copyData(dataElement);
        if (map->current->data == NULL)
        {
            map->current->data = tmp_data;
            return MAP_OUT_OF_MEMORY;
        }
        map->freeData(tmp_data);
    }
    return MAP_SUCCESS;
}

MapDataElement mapGet(Map map, MapKeyElement keyElement)
{
    if (map == NULL || keyElement == NULL)
    {
        return NULL;
    }
    map->current = findKey(map, keyElement);
    if (map->current == NULL)
    {
        return NULL;
    }
    return map->current->data;
}

MapResult mapRemove(Map map, MapKeyElement keyElement)
{
    if (map == NULL || keyElement == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }
    map->current = findKey(map, keyElement);
    if (map->current == NULL)
    {
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    Node current = map->current;
    map->current->previous->next = map->current->next;
    if (map->current->next != NULL)
    {
        map->current->next->previous = map->current->previous;
    }
    map->current = map->current->previous;
    map->freeData(current->data);
    map->freeKey(current->key);
    current->next = NULL;
    current->previous = NULL;
    free(current);
    map->counter--;
    return MAP_SUCCESS;
}

MapKeyElement mapGetFirst(Map map)
{
    if (map == NULL)
    {
        return NULL;
    }
    map->current = map->first->next; // skip dummy item
    if (map->current == NULL)
    {
        return NULL;
    }
    return map->copyKey(map->current->key);
}

MapKeyElement mapGetNext(Map map)
{
    if (map == NULL)
    {
        return NULL;
    }
    if (map->current == NULL)
    {
        return NULL;
    }
    if (map->current->next == NULL)
    {
        return NULL;
    }
    map->current = map->current->next;
    return map->copyKey(map->current->key);
}

MapResult mapClear(Map map)
{
    if (map == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }
    map->current = map->first->next;
    while (map->current != NULL)
    {
        Node tmp = map->current;
        map->current = map->current->next;
        map->freeData(tmp->data);
        map->freeKey(tmp->key);
        tmp->next = NULL;
        tmp->previous = NULL;
        free(tmp);
    }
    map->first->next = NULL;
    map->counter = 0;
    return MAP_SUCCESS;
}
