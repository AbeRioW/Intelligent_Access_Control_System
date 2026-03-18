#include "ui.h"    

void welcome(void)
{
	 OLED_ShowString(10, 0, (uint8_t*)"Welcome", 8, 1);
   OLED_ShowString(0, 32, (uint8_t*)"Press KEY16 for PIN", 8, 1);
   OLED_Refresh();
}
