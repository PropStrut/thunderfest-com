
/*  CONTACT.C  */

#include <stdio.h>
#include <string.h>
#include <process.h>
#include "cgi-util.h"


#define WORKBUFFER_LEN 65536
char subject[256], organization[256], server[256], sender[256];
char RootDir[512];
char workbuffer[WORKBUFFER_LEN];


void output_page(char *msg)
{
static char fullname[256];

#if (1)
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
#else
  printf("<HEAD></HEAD><BODY><P>\n");
  // Copy the message to the user
  printf("%s\n", msg);
  printf("</BODY>");
#endif
}


void my_fgets(char *buf, int len, FILE *fp)
{
  fgets(buf, len, fp);
  len = strlen(buf);
  if (len > 0)
    if (buf[len-1]==13 || buf[len-1]==10)
      buf[len-1] = 0;
}


// Get subject, organization, server, sender
int get_dat_file_settings(char *contactfile)
{
FILE *fp;

  if ((fp=fopen(contactfile, "r")) == NULL)
    return 0;
  // Get the 1st line
  my_fgets(workbuffer, WORKBUFFER_LEN, fp);
  strcpy(subject, workbuffer);
  if (feof(fp) || (subject[0]==0)) {
    fclose(fp);
    return 0;
  }
  // Get the 2nd line
  my_fgets(workbuffer, WORKBUFFER_LEN, fp);
  strcpy(organization, workbuffer);
  if (feof(fp) || (organization[0]==0)) {
    fclose(fp);
    return 0;
  }
  // Get the 3rd line
  my_fgets(workbuffer, WORKBUFFER_LEN, fp);
  strcpy(server, workbuffer);
  if (feof(fp) || (server[0]==0)) {
    fclose(fp);
    return 0;
  }
  // Get the 4th line
  my_fgets(workbuffer, WORKBUFFER_LEN, fp);
  strcpy(sender, workbuffer);
  if (feof(fp) || (sender[0]==0)) {
    fclose(fp);
    return 0;
  }
  fclose(fp);
  return 1;
}

int main(int argc, char *argv[])
{
static char userEmail[256];
static char userText[65536];
static char BlatExe[512], TextFile[512], ContactData[512];
FILE *fp;

  // Get the user entries
  cgiinit();
  getentry(RootDir, "RootDir");
  getentry(userEmail, "userEmail");
  getentry(userText, "userText");
  //if (strlen(subject) < 1) {
  //  output_page("<P>You must enter a subject.");
  //  return 0;
  //}
  if (strlen(userText) < 1) {
    output_page("<P>You must enter a text message.  Click on your BACK button.<P>");
    return 0;
  }

  // Form filenames with full path
  strcpy(BlatExe, RootDir);
  strcat(BlatExe, "blat.exe");
  strcpy(TextFile, RootDir);
  strcat(TextFile, "msgbody2.wrk");
  strcpy(ContactData, RootDir);
  strcat(ContactData, "contact.dat");

  // Get the DAT file entries
  if (get_dat_file_settings(ContactData) == 0) {
    output_page("<P>System error: Data file missing.  Mail not sent.  Sorry.<P>");
    return 0;
  }

  // Copy userText to the work message file
  // Write the new file
  unlink(TextFile);
  if ((fp=fopen(TextFile, "w")) == NULL) {
    output_page("Couldn't open the temp file.  (Error processing request)");
    return 0;
  }
  if (strlen(userEmail) > 0)
    fprintf(fp, "\n****  User Email: %s  ****\n\n", userEmail);
  else
    fprintf(fp, "\n****  User did not enter their email address  ****\n\n");
  fprintf(fp, "%s", userText);
  fclose(fp);

  // Do the action
  sprintf(workbuffer, "Thank you for writing to Spirit Of Detroit Thunderfest.<P>");
  output_page(workbuffer);
  sprintf(workbuffer, "%s \"%s\" -s \"%s\" -t %s -f %s -o \"%s\" -server %s -noh -q",
    BlatExe, TextFile, subject, sender/*userEmail*/, sender, organization, server);
  system(workbuffer);

  // Delete the work file
  unlink(TextFile);
  return 1;
}


