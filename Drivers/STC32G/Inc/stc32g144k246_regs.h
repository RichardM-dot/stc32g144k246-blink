/**
 * @file    stc32g144k246_regs.h
 * @brief   STC32G144K246 的 SDCC (MCS-251) 兼容寄存器定义
 *
 * 官方头文件 COMM/STC32G144K246.H 使用 Keil C251 的 `sfr` / `sbit` 语法，
 * SDCC 不识别。本文件只保留工程实际使用到的标准 SFR，全部改用 SDCC 的
 * `__sfr __at (addr)` / `__sbit __at (bitaddr)` 写法。
 *
 * 注意：
 *  1. P_SW2 (0xBA) 不是位寻址 SFR，不能用 sbit 定义 EAXFR；
 *     要用 `P_SW2 |= 0x80;` 打开扩展寄存器访问。
 *  2. XFR (0x7EFE00~) 不在本文件内，需要时用 `__xdata`/`__far` 指针访问。
 *  3. 完整寄存器表见 Drivers/STC32G/Inc/STC32G144K246.H（官方原版，仅供参考）。
 */
#ifndef STC32G144K246_REGS_H
#define STC32G144K246_REGS_H

/* ---- 端口数据寄存器 ---- */
__sfr __at (0x80) P0;
__sfr __at (0x90) P1;
__sfr __at (0xA0) P2;
__sfr __at (0xB0) P3;
__sfr __at (0xC0) P4;
__sfr __at (0xC8) P5;
__sfr __at (0xE8) P6;
__sfr __at (0xF8) P7;

/* ---- 端口模式寄存器：1=输入/0=输出，M1:M0 组合决定四种模式 ---- */
__sfr __at (0x91) P1M1;
__sfr __at (0x92) P1M0;
__sfr __at (0x93) P0M1;
__sfr __at (0x94) P0M0;
__sfr __at (0x95) P2M1;
__sfr __at (0x96) P2M0;
__sfr __at (0xB1) P3M1;
__sfr __at (0xB2) P3M0;
__sfr __at (0xB3) P4M1;
__sfr __at (0xB4) P4M0;
__sfr __at (0xC9) P5M1;
__sfr __at (0xCA) P5M0;
__sfr __at (0xCB) P6M1;
__sfr __at (0xCC) P6M0;
__sfr __at (0xE1) P7M1;
__sfr __at (0xE2) P7M0;

/* ---- 核心 SFR ---- */
__sfr __at (0x81) SP;
__sfr __at (0x82) DPL;
__sfr __at (0x83) DPH;
__sfr __at (0x84) DPXL;
__sfr __at (0x85) SPH;
__sfr __at (0x87) PCON;
__sfr __at (0x88) TCON;
__sfr __at (0x89) TMOD;
__sfr __at (0x8E) AUXR;
__sfr __at (0x98) SCON;
__sfr __at (0x99) SBUF;
__sfr __at (0xBA) P_SW2;
__sfr __at (0xD0) PSW;
__sfr __at (0xE0) ACC;
__sfr __at (0xF0) B;

/* ---- 等待/总线/时钟相关 ---- */
__sfr __at (0xE9) WTST;     /* Flash 读取等待周期，i-Cache 开启时必须 >=1 */
__sfr __at (0xEA) CKCON;    /* 片内 XRAM 总线时钟控制，0 = 最快 */

/* ---- 位寻址：P0~P7 的 8 位 ---- */
__sbit __at (0x80) P00; __sbit __at (0x81) P01;
__sbit __at (0x82) P02; __sbit __at (0x83) P03;
__sbit __at (0x84) P04; __sbit __at (0x85) P05;
__sbit __at (0x86) P06; __sbit __at (0x87) P07;

__sbit __at (0x90) P10; __sbit __at (0x91) P11;
__sbit __at (0x92) P12; __sbit __at (0x93) P13;
__sbit __at (0x94) P14; __sbit __at (0x95) P15;
__sbit __at (0x96) P16; __sbit __at (0x97) P17;

__sbit __at (0xA0) P20; __sbit __at (0xA1) P21;
__sbit __at (0xA2) P22; __sbit __at (0xA3) P23;
__sbit __at (0xA4) P24; __sbit __at (0xA5) P25;
__sbit __at (0xA6) P26; __sbit __at (0xA7) P27;

__sbit __at (0xB0) P30; __sbit __at (0xB1) P31;
__sbit __at (0xB2) P32; __sbit __at (0xB3) P33;
__sbit __at (0xB4) P34; __sbit __at (0xB5) P35;
__sbit __at (0xB6) P36; __sbit __at (0xB7) P37;

__sbit __at (0xC0) P40; __sbit __at (0xC1) P41;
__sbit __at (0xC2) P42; __sbit __at (0xC3) P43;
__sbit __at (0xC4) P44; __sbit __at (0xC5) P45;
__sbit __at (0xC6) P46; __sbit __at (0xC7) P47;

__sbit __at (0xC8) P50; __sbit __at (0xC9) P51;
__sbit __at (0xCA) P52; __sbit __at (0xCB) P53;
__sbit __at (0xCC) P54; __sbit __at (0xCD) P55;
__sbit __at (0xCE) P56; __sbit __at (0xCF) P57;

__sbit __at (0xE8) P60; __sbit __at (0xE9) P61;
__sbit __at (0xEA) P62; __sbit __at (0xEB) P63;
__sbit __at (0xEC) P64; __sbit __at (0xED) P65;
__sbit __at (0xEE) P66; __sbit __at (0xEF) P67;

__sbit __at (0xF8) P70; __sbit __at (0xF9) P71;
__sbit __at (0xFA) P72; __sbit __at (0xFB) P73;
__sbit __at (0xFC) P74; __sbit __at (0xFD) P75;
__sbit __at (0xFE) P76; __sbit __at (0xFF) P77;

/* ---- 扩展 SFR 访问开关（P_SW2.7，非位寻址，必须整字节操作）---- */
#define EAXFR_ENABLE()   do { P_SW2 |= 0x80; } while (0)
#define EAXFR_DISABLE()  do { P_SW2 &= (unsigned char)~0x80; } while (0)

/* ---- 空操作 ---- */
#define NOP()            do { __asm nop __endasm; } while (0)

#endif /* STC32G144K246_REGS_H */
