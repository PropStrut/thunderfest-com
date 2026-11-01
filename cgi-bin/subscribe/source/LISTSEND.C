
/*  LISTSEND.C  */

#include <stdio.h>
#include <string.h>
#include <process.h>
#include "list.h"
#include "cgi-util.h"


#define ADMIN_PASSWORD "listpass3"


char         organization[PARAM_LEN];     // ="Spirit Of Detroit THUNDERFEST";
char         server[PARAM_LEN];           // ="mail.thunderfest.com";
char         sender[PARAM_LEN];           // ="webmaster@thunderfest.com";
char         from[PARAM_LEN];             // ="webmaster@thunderfest.com";
char         workbuffer[WORKBUFFER_LEN];
char         workbuffer2[WORKBUFFER_LEN];
USER_RECORD  users[MAX_USERS];
int          user_count;


void output_page(char *msg1, char *msg2)
{
static char fullname[256];

  // Output the HTML page header
  printf("Content-type: text/html\n\n");
  printf("<HTML><HEAD><TITLE>ListSend response page</TITLE></HEAD><BODY>\n");
  // Copy the message to the user
  printf("%s\n", msg1);
  if (msg2)
    printf("<P>%s\n", msg2);
  printf("<P></BODY></HTML>\n");
}


void mystrip(char *str)
{
static char tmp[256];

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
        output_page("Maximum count of users has been exceeded.", NULL);
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
static char text[65536];
static char subject[256], password[256];
static char RootDir[512];
static char BlatExe[512], IniFile[512], TextFile[512], UserList[512];
FILE *fp;
int u;

  // Get the user entries
  cgiinit();
  getentry(password, "userPassword");
  getentry(subject, "userSubject");
  getentry(text, "userText");
  if (strcmp(password, ADMIN_PASSWORD) != 0) {
    output_page("Wrong password.", NULL);
    return 0;
  }
  if (strlen(subject) < 1) {
    output_page("You must enter a subject.", NULL);
    return 0;
  }
  if (strlen(text) < 1) {
    output_page("You must enter a text message.", NULL);
    return 0;
  }

  // Get the hidden form data
  getentry(RootDir, "RootDir");   // RootDir to all ListSend data files
  getentry(organization, "Organization");
  getentry(server, "Server");
  getentry(sender, "Sender");
  getentry(from, "From");

  // Form filenames with full path
  strcpy(BlatExe, RootDir);
  strcat(BlatExe, "myblat.exe");
  strcpy(UserList, RootDir);
  strcat(UserList, "list/list.txt");
  strcpy(IniFile, RootDir);
  strcat(IniFile, "listsend.ini");
  strcpy(TextFile, RootDir);
  strcat(TextFile, "msgbody.wrk");

  // Copy userText to the work message file ("msgbody.wrk" as TextFile)
  // Write the new file
  unlink(TextFile);
  if ((fp=fopen(TextFile, "w")) == NULL) {
    output_page("Couldn't open the temp file.", NULL);
    return 0;
  }
  fprintf(fp, "%s", text);
  fclose(fp);

  // Read in the userlist file
  if (read_database(UserList) == 0) {
    sprintf(workbuffer, "Problem with the '%s' file.<P>\nPlease alert the webmaster!", UserList);
    output_page(workbuffer, NULL);
    return 0;
  }

  // Do the action
  for (u=0; u<user_count; u++) {
    sprintf(workbuffer, "%s \"%s\" -s \"%s\" -t %s -f %s -o \"%s\" -server %s -noh -q",
      BlatExe, TextFile, subject, users[u].address, sender, organization, server);
    /*
    This has a FROM parameter
    sprintf(workbuffer, "%s \"%s\" -s \"%s\" -t %s -f %s -i %s -o \"%s\" -server %s -noh -q",
      BlatExe, TextFile, subject, users[u].address, sender, from, organization, server);
    */

    // Output a message and sample command line
    if (u == 0) {
      sprintf(workbuffer2, "Sending to %d subscribers.  Command line for first email:<P>\n", user_count);
      output_page(workbuffer2, workbuffer);
    }

    // Perform the command
    system(workbuffer);
  }

  // Delete the work file
  unlink(TextFile);
  return 1;
}


