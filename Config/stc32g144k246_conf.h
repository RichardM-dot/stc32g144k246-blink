/**
 * @file    stc32g144k246_conf.h
 * @brief   工程级配置：主频、LED 引脚、延时参数
 *
 * 修改 LED 接线只需要改这三个宏，不需要动 main.c。
 */
#ifndef STC32G144K246_CONF_H
#define STC32G144K246_CONF_H

/*
 * 用户程序实际运行频率。
 * STC32G 复位后使用“上次 ISP 下载时修调的 HIRC 频率”：
 *   - AiCube-ISP 里选 24MHz  -> 这里写 24000000UL
 *   - AiCube-ISP 里选 48MHz  -> 这里写 48000000UL
 * 如果频率不一致，delay_ms() 的闪烁周期会按比例偏快/偏慢。
 */
#define MAIN_Fosc            24000000UL

/* LED：默认接 P0.0，低电平点亮；P4.1 作 LED 电源使能（低有效） */
#define LED_PORT             P0
#define LED_PIN              0
#define LED_ON()             do { P00 = 0; } while (0)
#define LED_OFF()            do { P00 = 1; } while (0)
#define LED_TOGGLE()         do { P00 = !P00; } while (0)

/* LED 电源控制脚：官方实验箱用 P4.1，低电平使能 */
#define LED_PWR_PORT         P4
#define LED_PWR_PIN          1
#define LED_PWR_ON()         do { P41 = 0; } while (0)

/* 闪烁半周期（ms） */
#define BLINK_HALF_MS        500U

#endif /* STC32G144K246_CONF_H */
