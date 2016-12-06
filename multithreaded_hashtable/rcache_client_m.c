#include "rcache.h"
#include <stdio.h>
#define MAX_BUFFER 60

// Client-side test program for the rCache system

int main(int argc, char** argv) {

  printf("inserting (a, one)\n");
  rInsert("a", "one");
  printf("inserting (b, two)\n");
  rInsert("b", "two");
  printf("inserting (c, three)\n");
  rInsert("c", "three");
  printf("inserting (d, four)\n");
  rInsert("d", "four");
  printf("printing table\n");
  rPrint();

  // Add more tests here...
  char val[MAX_BUFFER];
  rRemove("b", val);
  printf("Removing key 'b'. Value: %s\n", val);
  
  rPrint();
  
  char val1[MAX_BUFFER];
  rLookup("f", val1);
  printf("Lookup nonexistent key 'f'. Value should be null: %s\n", val1);
  
  char val4[MAX_BUFFER];
  rRemove("d", val4);
  printf("Removing key 'd'. Value: %s\n", val4);
  
  char val3[MAX_BUFFER];
  rLookup("c", val3);
  printf("Lookup key 'c'. Value: %s\n", val3);
  
  char val2[MAX_BUFFER];
  rLookup("a", val2);
  printf("Lookup key 'a'. Value: %s\n", val2);
  
  printf("End of requests\n");

  return 0;
}