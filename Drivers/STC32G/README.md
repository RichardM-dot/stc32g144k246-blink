# Drivers/STC32G — STC32G144K246 寄存器头文件

本目录只放**头文件**，不含官方 C 源库。原因是官方例程包里的 `.c` 文件使用
Keil C251 专有语法（`sfr` / `sbit` / `interrupt N` / `_nop_()` / `#pragma`），
sdcc-c251 不能直接编译；本工程的驱动代码在 `Core/Src/` 用 SDCC 语法重写。

| 文件 | 来源 | 用途 |
|---|---|---|
| `Inc/STC32G144K246.H` | 官方例程包 `COMM/STC32G144K246.H` 原样复制 | 查寄存器地址/位定义/完整外设表（**Keil 语法，不能直接 include**） |
| `Inc/stc32g144k246_regs.h` | 本工程按 SDCC 语法重写 | 工程实际 include 的 SFR/位定义（P0~P7、PnM0/PnM1、WTST、CKCON、P_SW2 等） |

## 官方包位置

`libraries/stc/STC32G144K246/DEMO-CODE/`（1439 个文件，88 个例程 + `COMM/` 公共头文件）。
官方头文件原版 2942 行，包含 122 个 `sfr` 和 343 个 `sbit`；XFR 寄存器用
`#define ... (*(unsigned char volatile far *)0x7EFE00)` 形式定义。

## 为什么不用官方头文件直接编译

1. `sfr P0 = 0x80;` 是 Keil C251 语法，SDCC 报 `Syntax error at 'sfr'`；
   SDCC 写法是 `__sfr __at (0x80) P0;`。
2. `sbit EAXFR = P_SW2^7;` 中 `P_SW2` 地址 0xBA **不是位寻址 SFR**，
   SDCC 不能用 `__sbit` 表示；本工程改用 `P_SW2 |= 0x80;` 宏。
3. 官方头文件还 `#include "stdio.h"` / `"intrins.h"`，并有一个改写 `main`
   的宏（`#define main(x) ...`），会干扰 SDCC 的启动代码。

需要新增外设时，从官方头文件查地址，再把用到的 SFR 按 `__sfr __at` 补进
`stc32g144k246_regs.h`；XFR 寄存器用 `__xdata`/`__far` 指针访问，访问前
`P_SW2 |= 0x80`。
