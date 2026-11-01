
#ifndef _LIST_H
#define _LIST_H


#define WORKBUFFER_LEN   4096
#define PARAM_LEN         128

#define MAX_USERS        2000
#define MAX_ADDRESS        60
#define MAX_NAME           60
#define MAX_AFFILIATION    30


// Each table entry in the database
typedef struct {
  char address[MAX_ADDRESS];
  char name[MAX_NAME];
  char affiliation[MAX_AFFILIATION];
} USER_RECORD;



#endif

