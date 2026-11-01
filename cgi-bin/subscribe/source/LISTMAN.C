
/*  LISTMAN.C  */

#include <stdio.h>
#include <string.h>
#include "list.h"
#include "cgi-util.h"


char         RootDir[]="d:/webs/thunderfest.com/htdocs/cgi-bin/subscribe/";
char         UserList[]="d:/webs/thunderfest.com/htdocs/cgi-bin/subscribe/list/list.txt";
USER_RECORD  users[MAX_USERS];
int          user_count;
char         workbuffer[WORKBUFFER_LEN];


void output_page(char *msg)
{
static char fullname[256];

  // Output the HTML page header
  printf("Content-type: text/html\n\n");
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


int read_database(void)
{
USER_RECORD user;
FILE *fp;
char *ptr;

  user_count = 0;
  if ((fp=fopen(UserList, "r")) == NULL)
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
      strcpy(users[user_count].address, user.address);
      strcpy(users[user_count].name, user.name);
      strcpy(users[user_count].affiliation, user.affiliation);
      user_count++;
    }
  }

  fclose(fp);
  return 1;
}


int write_database(void)
{
FILE *fp;
int u;

  if ((fp=fopen(UserList, "w")) == NULL)
    return 0;
  if (user_count > 0) {
    for (u=0; u<user_count; u++) {
      fprintf(fp, "\"%s\", \"%s\", \"%s\"\n",
        users[u].address, users[u].name, users[u].affiliation);
    }
  }
  fclose(fp);
  return 1;
}


int user_is_in_database(USER_RECORD *user)
{
int u;

  for (u=0; u<user_count; u++) {
    if (stricmp(user->address, users[u].address) == 0)
      return 1;
  }
  return 0;
}


void subscribe(USER_RECORD *user)
{
  if (user_is_in_database(user)) {
    sprintf(workbuffer, "<P>%s: '%s'<BR>You're already in our database.",
      user->name, user->address);
    output_page(workbuffer);
    return;
  }
  if (user_count > MAX_USERS-3) {
    output_page("Sorry, out of database space.  Please alert our webmaster.<BR>You have not been subscribed.");
    return;
  }
  strcpy(users[user_count].address, user->address);
  strcpy(users[user_count].name, user->name);
  strcpy(users[user_count].affiliation, user->affiliation);
  user_count++;
  sprintf(workbuffer,
    "<P>%s: '%s'<BR>Thank you for subscribing to SIGNAL FLAG ONLINE!",
    user->name, user->address);
  output_page(workbuffer);
}


void unsubscribe(USER_RECORD *user)
{
int u, i;

  if (!user_is_in_database(user)) {
    sprintf(workbuffer, "<P>%s: '%s'<BR>You're not in our database with that email address.",
      user->name, user->address);
    output_page(workbuffer);
    return;
  }
  if (user_count > 0) {
    for (u=0; u<user_count; u++) {
      if (strcmp(users[u].address, user->address) == 0) {
        for (i=u; i<user_count-1; i++)
          users[i] = users[i+1];
        user_count--;
        break;  // quit the for loop
      }
    }
  }
  sprintf(workbuffer,
    "<P>%s: '%s'<BR>You have been unsubscribed from the SIGNAL FLAG ONLINE list",
    user->name, user->address);
  output_page(workbuffer);
}


int main(int argc, char *argv[])
{
USER_RECORD user;
char *ptr;
char sub[20], unsub[20];
int current_count;

  // Get the FORM data
  cgiinit();

  getentry(workbuffer, "userAddress");
  if (strlen(workbuffer) >= (MAX_ADDRESS-1)) {
    output_page("<P>Sorry, your email address is too long.  Sure it's correct?");
    return 0;
  }
  strcpy(user.address, workbuffer);

  getentry(workbuffer, "userName");
  if (strlen(workbuffer) >= (MAX_NAME-1)) {
    output_page("<P>Sorry, please use a shorter version of your name.");
    return 0;
  }
  strcpy(user.name, workbuffer);

  getentry(workbuffer, "userAffiliation");
  strcpy(user.affiliation, workbuffer);

  getentry(sub, "SUB");       // The SUBSCRIBE button
  getentry(unsub, "UNSUB");   // The UNSUBSCRIBE button

  // Check the email address
  if (goodemailaddress(user.address) == 0) {
    output_page("<P>Sorry, that is not a valid email address.  Please go back and try again.<P>A valid email address looks like \"your_name@your_provider.com\"");
    return 0;
  }
  ptr = strchr(user.name, '"');
  if (ptr) {
    output_page("<P>Sorry, you cannot use a double-quote (\") in your email address.  Please go back and try again.");
    return 0;
  }

  // Check the name
  ptr = strchr(user.name, ',');
  if (ptr) {
    output_page("<P>Sorry, you cannot use a comma (,) in your name.  Please go back and try again.");
    return 0;
  }
  ptr = strchr(user.name, '"');
  if (ptr) {
    output_page("<P>Sorry, you cannot use a double-quote (\") in your name.  Please go back and try again.");
    return 0;
  }

  // Read in data file
  if (read_database() == 0) {
    output_page("<P>Missing \"list.txt\".  Please alert the webmaster!");
    return 0;
  }
  current_count = user_count;

  // Do the chosen action
  if (strcmp(sub, "Subscribe") == 0) {
    subscribe(&user);
  }
  else if (strcmp(unsub, "Unsubscribe") == 0) {
    unsubscribe(&user);
  }

  // If we added or deleted one, re-save data file
  if (current_count != user_count)
    write_database();

  return 1;
}


