/*
  show_msgUserInput allows to invoke a Request popup.

  show_msgUserInput compared to _show_msgUserInput avoid 
  manipulations of utf16 strings.
  It initializes a new char array and returns its size or -1 if Canceled

  How to use :
   const char * title = "Title";
   const char * msg = "Element";
   char * defaultvalue = "default value";
   char * value;
   unsigned len = show_msgUserInput(title, msg, defaultvalue, &value, 1);
   printf("%s (%d)\n", value, len);
   free(value);
   
  Contributor : Levak
*/

#include <os.h>

int show_msgUserInput(const char * title, const char * msg, char * defaultvalue, char ** value_ref, int convertToAscii) {

  unsigned len_input = strlen(defaultvalue);
  char RequestValue[(len_input + 1) * 2];
  char title16[(strlen(title) + 1) * 2];
  char msg16[(strlen(msg) + 1) * 2];
  ascii2utf16(title16, title, sizeof(title16));
  ascii2utf16(msg16, msg, sizeof(msg16));
  ascii2utf16(RequestValue, defaultvalue, sizeof(RequestValue));
  char ** p_msg = (char **) &msg16;
  char ** p_value = (char **) &RequestValue;
  char ** RequestStruct[] = {&p_msg, &p_value};  

  int no_error = _show_msgUserInput(0, RequestStruct, title16, msg16);
 
  unsigned len_out = (no_error) ? utf16_strlen(*RequestStruct[1]) : 0;
  if(no_error && len_out > 0) {
    char * newvalue;
    if(convertToAscii) {
      printf("%x\n", RequestStruct[1]);
      newvalue = malloc(sizeof(char) * (len_out + 1));
      utf162ascii(newvalue, *RequestStruct[1], len_out);
      printf("%x\n", newvalue);
      *value_ref = newvalue;
    }
    else {
      newvalue = malloc(sizeof(char) * (len_out + 1) * 2);
      memcpy(newvalue, RequestValue, (len_out + 1)*2);
      *value_ref = newvalue;
    }
    return len_out;
  }
  else
    return -1;
}
