
/*  ADMIN.C  */

#include <stdio.h>
#include <string.h>
#include "list.h"
#include "cgi-util.h"


#define ADMIN_PASSWORD "listpass3"

char         RootDir[]="d:/webs/thunderfest/cgi-bin/subscribe/";
char         UserList[]="d:/webs/thunderfest/cgi-bin/subscribe/list/list.txt";
char         workbuffer[WORKBUFFER_LEN];


void error_page(char *msg)
{
static char fullname[256];

  // Output the HTML page header
  printf("Content-Type: text/html\n");
  // Copy the header of our HTML page to the user
  strcpy(fullname, RootDir);
  strcat(fullname, "header.html");
  dump(fullname);
  // Copy the message to the user
  printf("%s\n", msg);
  // Copy the footer of our HTML page to the user
  strcpy(fullname, RootDir);
  strcat(fullname, "footer.html");
  dump(fullname);
}


void output_everyone(void)
{
FILE *fp;

  // Output the HTML page header
  printf("Content-type: text/html\n\n");
  if ((fp=fopen(UserList, "r")) == NULL) {
    printf("Couldn't open the list file - please alert our webmaster");
    return;
  }

  printf("<HR>");
  while (1) {
    // Get the data line
    fgets(workbuffer, WORKBUFFER_LEN, fp);
    if (feof(fp))
      break;
    printf("%s<BR>", workbuffer);
  }
  printf("<HR>");

  fclose(fp);
}


int main(int argc, char *argv[])
{
char password[20];

  cgiinit();
       
  getentry(password, "userPassword");
  if (strcmp(password, ADMIN_PASSWORD) != 0) {
    error_page("<P>Wrong password.");
    return 0;
  }

  output_everyone();

  return 1;
}



