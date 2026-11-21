/**
 * @file    delay.c
 * @brief   粗略毫秒延时
 *
 * 说明：i-Cache 开启后指令执行时间不是完全线性的，本延时只用于 LED 闪烁，
 *       不要用于 UART/定时器等对时间精度有要求的场合。
 *       需要精确延时请用定时器（T0~T11/T17/T18）。
 */
#include "stc32g144k246_conf.h"
#include "delay.h"

void delay_ms(unsigned int ms)
{
    unsigned int i;

    do {
        i = (unsigned int)(MAIN_Fosc / 6000UL);
        while (--i) {
            ;
        }
    } while (--ms);
}
