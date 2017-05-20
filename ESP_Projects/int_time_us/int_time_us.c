/******************************************************************************
* PV` * Test intr TM1
*******************************************************************************/
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "gpio.h"


#include "user_interface.h"

//XXX: 0xffffffff/(80000000/16)=35A
#define US_TO_RTC_TIMER_TICKS(t)          \
    ((t) ?                                   \
     (((t) > 0x35A) ?                   \
      (((t)>>2) * ((APB_CLK_FREQ>>4)/250000) + ((t)&0x3) * ((APB_CLK_FREQ>>4)/1000000))  :    \
      (((t) *(APB_CLK_FREQ>>4)) / 1000000)) :    \
     0)

//FRC1
#define FRC1_ENABLE_TIMER  BIT7

typedef enum {
    DIVDED_BY_1 = 0,
    DIVDED_BY_16 = 4,
    DIVDED_BY_256 = 8,
} TIMER_PREDIVED_MODE;

typedef enum {
    TM_LEVEL_INT = 1,
    TM_EDGE_INT   = 0,
} TIMER_INT_MODE;
/******************************************************************************
*******************************************************************************/
void ICACHE_FLASH_ATTR set_new_time_int_us(uint32 us)
{
    RTC_REG_WRITE(FRC1_LOAD_ADDRESS, US_TO_RTC_TIMER_TICKS(us));
}
/******************************************************************************
*******************************************************************************/
void pwm_tim1_intr_handler(void)
{
    RTC_CLR_REG_MASK(FRC1_INT_ADDRESS, FRC1_INT_CLR_MASK);
	REG_SET_BIT(0x60000304, BIT12);
	REG_SET_BIT(0x60000308, BIT12);
}
/******************************************************************************
*******************************************************************************/
void ICACHE_FLASH_ATTR int_us_disable(void)
{
	ETS_FRC1_INTR_DISABLE();
	TM1_EDGE_INT_DISABLE();
}
/******************************************************************************
*******************************************************************************/
void ICACHE_FLASH_ATTR int_us_init(uint32 us)
{
    int_us_disable();
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	REG_SET_BIT(0x6000030C, BIT12);
    RTC_REG_WRITE(FRC1_CTRL_ADDRESS,
                  DIVDED_BY_16
                  | BIT6 // автоповтор
                  | FRC1_ENABLE_TIMER
                  | TM_EDGE_INT);
    set_new_time_int_us(us);
    ETS_FRC_TIMER1_INTR_ATTACH(pwm_tim1_intr_handler, NULL);
    TM1_EDGE_INT_ENABLE();
    ETS_FRC1_INTR_ENABLE();
}
