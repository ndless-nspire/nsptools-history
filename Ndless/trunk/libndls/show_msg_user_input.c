/****************************************************************************
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Ndless code.
 *
 * The Initial Developer of the Original Code is Levak
 * Portions created by the Initial Developer are Copyright (C) 2012
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 ****************************************************************************/

/*
  show_msg_user_input allows to invoke a Request popup.

  show_msg_user_input compared to _show_msgUserInput avoids
  manipulations of utf16 strings.
  It initializes a new char array and returns its size or -1 if Cancelled or
  emtpy input.

  How to use:
   const char * title = "Title";
   const char * msg = "Element";
   char * defaultvalue = "default value";
   char * value;
   unsigned len = show_msg_user_input(title, msg, defaultvalue, &value);
   printf("%s (%d)\n", value, len);
   free(value);
*/

#include <os.h>

int show_msg_user_input(const char * title, const char * msg, char * defaultvalue, char ** value_ref) {

  unsigned len_input = strlen(defaultvalue);
  char request_value[(len_input + 1) * 2];
  char title16[(strlen(title) + 1) * 2];
  char msg16[(strlen(msg) + 1) * 2];
  ascii2utf16(title16, title, sizeof(title16));
  ascii2utf16(msg16, msg, sizeof(msg16));
  ascii2utf16(request_value, defaultvalue, sizeof(request_value));
  char **p_msg = (char **) &msg16;
  char **p_value = (char **) &request_value;
  char **request_struct[] = {(char**)&p_msg, (char**)&p_value};  

  int no_error = _show_msgUserInput(0, request_struct, title16, msg16);
 
  unsigned len_out = (no_error) ? utf16_strlen(*request_struct[1]) : 0;
  if(no_error && len_out > 0) {
    char * newvalue;
    newvalue = malloc(sizeof(char) * (len_out + 1));
    *value_ref = newvalue;
    if (!newvalue) return -1;
    utf162ascii(newvalue, *request_struct[1], len_out);
		return len_out;
  }
  else
    return -1;
}	
