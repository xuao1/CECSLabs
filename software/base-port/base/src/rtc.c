#include <base.h>
#include <dev-mmio.h>

void __timer_init() {
}

void __timer_uptime(DEV_TIMER_UPTIME_T *uptime) {
  // Lab6 TODO: implement uptime MMIO
  // hint: use function `inl` and RTC_ADDR to access RTC MMIO
  uptime->lo = inl(RTC_ADDR);                  
  uptime->hi = inl(RTC_ADDR + 4 * sizeof(uint32_t)); 
}

void __timer_rtc(DEV_TIMER_RTC_T *rtc) {
  // Lab6 TODO: implement RTC MMIO
  // hint: use function `inl` and RTC_ADDR with offset  to access RTC MMIO
    rtc->second = inl(RTC_ADDR + 8 * sizeof(uint32_t));  
    rtc->minute = inl(RTC_ADDR + 12 * sizeof(uint32_t)); 
    rtc->hour   = inl(RTC_ADDR + 16 * sizeof(uint32_t)); 
    rtc->day    = inl(RTC_ADDR + 20 * sizeof(uint32_t)); 
    rtc->month  = inl(RTC_ADDR + 24 * sizeof(uint32_t)); 
    rtc->year   = inl(RTC_ADDR + 28 * sizeof(uint32_t)); 

}