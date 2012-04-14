
#include <os.h>
#include <libndls.h>

int main(int argc, char *argv[]) {
   const char * title = "Title";
   const char * subtitle = "Sub titile"; 
   const char * msg1 = "Element 1";
   const char * msg2 = "Element 2";
   char * defaultvalue = "default value";
   char * value;
   unsigned len;

   len = show_msgUserInput(title, msg1, defaultvalue, &value, 0);
   printf("%x %s (%d)\n", value, value, len); // utf16
   show_msgbox(title, value);
   free(value);
   len = show_msgUserInput(title, msg1, defaultvalue, &value, 1);
   printf("%s (%d)\n", value, len); // ascii
   show_msgbox(title, value); // y u no work
   free(value);

   int value1 = 42;
   int value2 = 1337;
   int result;
   char popup1_result[256];
   char popup2_result[256];
   result = show_1NumericInput(title, subtitle, msg1, &value1, -42, 9001);
   printf("%d(%s) : %d\n", result, (result == 1)?"OK":"CANCELED", value1);
   sprintf(popup1_result, "%s:%d", msg1, value1);
   show_msgbox(title, popup1_result);
   result = show_2NumericInput(title, subtitle, msg1, &value1, -42, 9001, msg2, &value2, -42, 9001);
   printf("%d(%s) : %d; %d\n", result, (result == 1)?"OK":"CANCELED", value1, value2);
   sprintf(popup2_result, "%s:%d\n%s:%d", msg1, value1, msg2, value2);
   show_msgbox(title, popup2_result);

   show_msgbox("Congratulation", "Everything went better than expected !");
   return 0;
}
