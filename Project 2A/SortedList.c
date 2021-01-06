/*
NAME:Juan Bai
EMAIL:Daisybai66@yahoo.com
ID:105364754
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include "SortedList.h"

//sorted in ascending order based on associated keys
// SortedList_t *list ... header for the list
//SortedListElement_t *element ... element to be added to the list


void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{

  if(list==NULL || element==NULL)
    return;
  if(list->next==NULL)
  {
    list->next = element;
    list->prev = element;
    element->prev = list;
    element->next = list;
    return;
  }
  SortedListElement_t *temp = list->next;
  while(temp != list && strcmp(temp->key,element->key) < 0)
  {
    temp = temp->next;
  }
  //critical section
  if(opt_yield & INSERT_YIELD)
    sched_yield();

  element->prev = temp->prev;
  element->next = temp;
  temp->prev->next = element;
  temp->prev = element;
}

int SortedList_delete( SortedListElement_t *element)
{
  if(element == NULL)
    return 1;
  
  if(opt_yield & DELETE_YIELD)
    sched_yield();

  if(element->next->prev == element && element->prev->next == element)
    {  
      element->next->prev = element->prev;
      element->prev->next = element->next;
      return 0;
    }
  else
    return 1;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
  if(list==NULL || key==NULL)
    return NULL;
  SortedList_t* temp = list->next;

   while(temp != list)
  {
    if(opt_yield & LOOKUP_YIELD)
      sched_yield();
    if(strcmp(temp->key, key)==0)
      return temp;
    else
      temp = temp->next;

    if(temp==NULL)
      return NULL;
  }
  return NULL;
}

int SortedList_length(SortedList_t *list)
{
  if(list==NULL)
    return -1;
  int length=0;
  SortedList_t* temp = list->next;

  while(temp != list)
  {
      if(temp==NULL)
	return -1;
      if(temp->prev->next != temp || temp->next->prev != temp)
        return -1;      
      if(opt_yield & LOOKUP_YIELD) 
	sched_yield();
      length++;
      temp = temp->next;
      if(temp==NULL)
	return -1;
  }
  return length;
}

