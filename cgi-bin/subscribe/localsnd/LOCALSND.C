
/*  LISTSEND.C  */

#include <stdio.h>
#include <string.h>
#include <process.h>
#include "list.h"
#include "cgi-util.h"



/*
char         organization[PARAM_LEN];     // ="Spirit Of Detroit THUNDERFEST";
char         server[PARAM_LEN];           // ="mail.thunderfest.com";
char         sender[PARAM_LEN];           // ="webmaster@thunderfest.com";
char         from[PARAM_LEN];             // ="webmaster@thunderfest.com";
*/
char         workbuffer[WORKBUFFER_LEN];
USER_RECORD  users[MAX_USERS];
int          user_count;


void mystrip(char *str)
{
char tmp[128];

  // Take off a leading space (if there is one)
  if (str[0] == ' ') {
    strcpy(tmp, &str[1]);
    strcpy(str, tmp);
  }

  // Take off a leading double-quote
  if (str[0] == '"') {
    strcpy(tmp, &str[1]);
    strcpy(str, tmp);
  }

  // Take off a trailing double-quote
  if (str[strlen(str)-1] == '"')
    str[strlen(str)-1] = 0;
  if (str[strlen(str)-2] == '"')
    str[strlen(str)-2] = 0;
}


int read_database(char *filename)
{
USER_RECORD user;
FILE *fp;
char *ptr;

  user_count = 0;
  if ((fp=fopen(filename, "r")) == NULL)
    return 0;
  while (1) {
    // Get the data line
    fgets(workbuffer, WORKBUFFER_LEN, fp);
    if (feof(fp))
      break;
    // Parse the line
    ptr = strtok(workbuffer, ",");
    if (ptr)
      strcpy(user.address, ptr);
    ptr = strtok(NULL, ",");
    if (ptr)
      strcpy(user.name, ptr);
    ptr = strtok(NULL, ",");
    if (ptr)
      strcpy(user.affiliation, ptr);
    mystrip(user.address);
    mystrip(user.name);
    mystrip(user.affiliation);

    // If it's good data, add them to the RAM database
    if (goodemailaddress(user.address) != 0) {
      if (user_count < MAX_USERS-1) {
        strcpy(users[user_count].address, user.address);
        strcpy(users[user_count].name, user.name);
        strcpy(users[user_count].affiliation, user.affiliation);
        user_count++;
      }
      else {
        printf("Maximum count of users has been exceeded.\n");
        fclose(fp);
        return 0;
      }
    }
  }

  fclose(fp);
  return 1;
}


int main(int argc, char *argv[])
{
/*
static char text[65536];
static char subject[256], password[256];
static char RootDir[512];
static char BlatExe[512], IniFile[512], TextFile[512], UserList[512];
*/
static char UserList[512]="list.txt";
FILE *fp;
int u;

  // Read in the userlist file
  if (read_database(UserList) == 0) {
    sprintf(workbuffer, "Problem with the '%s' file.<P>\nPlease alert the webmaster!", UserList);
    printf(workbuffer);
    return 0;
  }

  // Do the action
  for (u=0; u<user_count; u++) {
    printf("myblat.exe msgbody.wrk -s \"SIGNAL FLAG ONLINE #45\" -t %s -f news@thunderfest.com -o \"Spirit Of Detroit Thunderfest\" -server secure2.developerschoice.net -noh -q\n", users[u].address);
  }

  return 1;
}


