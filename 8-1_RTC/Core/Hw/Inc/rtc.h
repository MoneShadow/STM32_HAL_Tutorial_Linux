#ifndef  __RTC_H
#define  __RTC_H

extern RTC_HandleTypeDef hrtc;

void RTC_Init(void);
struct tm *Get_Localtime(void);

#endif
