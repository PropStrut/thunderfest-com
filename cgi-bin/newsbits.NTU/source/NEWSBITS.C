
// Use chmod and unlink for faster program compilation? (in windows.h)

/*  NEWSBITS.C  */

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
//#include <io.h>         // for chmod()
//#include <dos.h>        // for file attributes
#include "cgi-util.h"


#define PASSWORD           "test"
#define NEWSBITSDIR        "d:/webs/thunderfest/cgi-bin/newsbits/"
#define WORKBUFFER_LEN     4096
#define FCLOSE_BOTH_FILES  fclose(fpo);fclose(fpi);


int build_new_newsbits_page(char *msg, char *name)
{
static char inname[256], outname[256], ftpstr[256], workbuffer[WORKBUFFER_LEN];
unsigned int i, flag;
FILE *fpi, *fpo;

  // Get ready to do work.
  strcpy(inname, NEWSBITSDIR);
  strcat(inname, "newsbits.html");
  strcpy(outname, NEWSBITSDIR);
  strcat(outname, "newstemp.html");
  SetFileAttributes(outname, FILE_ATTRIBUTE_NORMAL);
  DeleteFile(outname);

  // Open the files.
  if ((fpi=fopen(inname, "r")) == NULL)
    return 0;
  if ((fpo=fopen(outname, "w")) == NULL) {
    fclose(fpi);
    return 0;
  }

  // Copy each data line to the output until we find our indicator.
  flag = 0;
  while (1) {
    fgets(workbuffer, WORKBUFFER_LEN, fpi);
    if (feof(fpi)) {
      FCLOSE_BOTH_FILES;  // error, we didn't find the indicator in the file.
      return 0;
    }
    fputs(workbuffer, fpo);
    if (flag == 1)  // flag lets us copy the blank line after the indicator.
      break;
    if (strcmp(workbuffer,"<!-- **** NEW NEWS BIT GOES HERE **** -->\n") == 0)
      flag = 1;
  }

  //// Put the date and time
  fprintf(fpo, "<P>\n");
  fprintf(fpo, "<FONT SIZE=2 COLOR=\"#808080\">\n");
  fprintf(fpo, "[");
  _strdate(workbuffer);
  fprintf(fpo, workbuffer);
// PUT IN THE TIME
//  // Now the time (strip off seconds).  We only do this in June and July.
//  if (workbuffer[1] >= '6'  &&  workbuffer[1] <= '7') {
    fprintf(fpo, " ");
    _strtime(workbuffer);
    flag = 0;
    for (i=0; i<8; i++) {
      if (workbuffer[i]==':' && flag==1) {
        workbuffer[i] = 0;
        break;
      }
      if (workbuffer[i]==':' && flag==0)
        flag = 1;
    }
    fprintf(fpo, workbuffer);
//  }
  fprintf(fpo, "]");

  // Now the contributors name, if there is one.
  if (strlen(name) > 0)
    fprintf(fpo, " : By %s", name);
  fprintf(fpo, "\n");
  fprintf(fpo, "</FONT>\n");
  fprintf(fpo, "<BR>\n");

  //// Copy the message to the page.
  // If CR/LF combo's are found as a pair then output <P>, else output <BR>.
  // If it's not a CR or LF then output the character.
  for (i=0; i<strlen(msg); i++) {
    switch (msg[i]) {
      case 10:                      // Ignore the LFs
        break;
      case 13:
        if (msg[i+2] == 13)  // See if this is a pair of CR/LFs
          fprintf(fpo, "\n<P>");
        else {
          if (i > 1)
            if (msg[i-2] != 13) // Dont do a <BR> after a <P>
              fprintf(fpo, "\n<BR>");
        }
        break;
      default:
        fprintf(fpo, "%c", msg[i]);
    }
  }
  fprintf(fpo, "\n\n");

  // Copy the rest of the file to the output.
  while (1) {
    fgets(workbuffer, WORKBUFFER_LEN, fpi);
    if (feof(fpi))
      break;      // Fall through to rest of routine.
    fputs(workbuffer, fpo);
  }

  // Finish up.
  FCLOSE_BOTH_FILES;

  // Rename the temp file
  SetFileAttributes(inname, FILE_ATTRIBUTE_NORMAL);
  DeleteFile(inname);
  rename(outname, inname);

  // Build the FTP command, display it, and send the file
  strcpy(ftpstr, "ftp -s:");
  strcat(ftpstr, NEWSBITSDIR);
  strcat(ftpstr, "newsbits.ftp");
  printf("<CENTER>\n");
  printf("<P>(FTP Command: %s)\n", ftpstr);
  printf("</CENTER>\n");
  system(ftpstr);

  return 1;
}


int main(int argc, char *argv[])
{
static char name[50], password[20], msg[WORKBUFFER_LEN];

  //// Get the FORM data - make sure we have a good password and message
  cgiinit();
  getentry(name, "userName");
  getentry(password, "userPassword");
  getentry(msg, "NewsBit");
  if (strcmp(password, PASSWORD) != 0) {
    // Output the HTML page header and message
    printf("Content-type: text/html\n\n");
    printf("Wrong password!");
    return 0;
  }
  if (strlen(msg)==0
   || (strlen(msg)==1 && msg[0]==32)
   || (strlen(msg)==2 && msg[0]==32 && msg[1]==32)) {
    // Output the HTML page header and message
    printf("Content-type: text/html\n\n");
    printf("You forgot the message!");
    return 0;
  }


  //// Output the page header first
  printf("Content-type: text/html\n\n");
  printf("<HEAD>\n");
  // printf("<META HTTP-EQUIV=\"refresh\" content=\"5;URL=http://www.thunderfest.com/cgi-bin/newsbits/newsbits.html\">\n");
  // printf("<META HTTP-EQUIV=\"refresh\" content=\"5;URL=http://209.69.202.116/hydro/newsbits/newsbits.html\">\n");
  printf("<META HTTP-EQUIV=\"refresh\" content=\"5;URL=http://www.reedonline.com/hydro/newsbits/newsbits.html\">\n");
  printf("</HEAD>\n");
  printf("<BODY BGCOLOR=\"#FFFFFF\">\n");

  //// Build the new page
  build_new_newsbits_page(msg, name);

  //// Output the page footer
  printf("<FONT SIZE=4>\n");
  printf("<CENTER>\n");
  printf("<P>Your NewsBit has been received.  Thank You!\n");
  printf("<P>The NewsBits page will display in a few seconds\n");
  printf("</CENTER>\n");
  printf("</FONT>\n");
  printf("</BODY>\n\n");


  return 1;
}


