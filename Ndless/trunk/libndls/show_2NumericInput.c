/*
  show_2NumericInput allows to invoke a 2 numeric input popup.

  show_2NumericInput compared to _show_2NumericInput avoid 
  manipulations of utf16 strings.
  It returns 1 if OK, 0 if Canceled

  Values like -1 or 0 for min_values will cancel the popup.
  Otherwise, you can have negative intervals (but not using -1 or 0)

  How to use :
   const char * title = "Title";
   const char * subtitle = "Sub Title";
   const char * msg1 = "Element 1";
   const char * msg2 = "Element 2";
   int value1 = 7;
   int value2 = 42;
   int result = show_2NumericInput(title, subtitle, msg1, &value1, 1, 42, msg2, &value2, -42, 9001);
   printf("%d(%s) : %d; %d\n", result, (result == 5103)?"OK":"CANCELED", value1, value2); 

  Contributor : Levak
*/

#include <os.h>

int show_2NumericInput(const char * title, const char * subtitle, const char * msg1, int * value1_ref, int min_value1, int max_value1, const char * msg2, int * value2_ref, int min_value2, int max_value2) {
  char title16[(strlen(title) + 1) * 2];
  char subtitle16[(strlen(subtitle) + 1) * 2];
  char msg116[(strlen(msg1) + 1) * 2];
  char msg216[(strlen(msg2) + 1) * 2];
  ascii2utf16(title16, title, sizeof(title16));
  ascii2utf16(subtitle16, subtitle, sizeof(subtitle16));
  ascii2utf16(msg116, msg1, sizeof(msg116));
  ascii2utf16(msg216, msg2, sizeof(msg216));
  int cheatMode = 0;
  int real_min_value1 = min_value1;
  int real_min_value2 = min_value2;
  if(min_value1 == 0 || min_value1 == -1) {
    real_min_value1 = -2;
    cheatMode = 1;
  }
  if(min_value2 == 0 || min_value2 == -1) {
    real_min_value2 = -2;
    cheatMode = 1;
  }
  int result = (_show_2NumericInput(0, title16, subtitle16, msg116, value1_ref, 1, real_min_value1, max_value1, msg216, value2_ref, 1, real_min_value2, max_value2) == 5103);
  if(cheatMode){
    if(*value1_ref < min_value1)
      *value1_ref = min_value1;
    if(*value2_ref < min_value2)
      *value2_ref = min_value2;
  }

  return result;
}
