# stc32g144k246-blink — SDCC MCS-251 最小工程（STC32G144K246 / LQFP100）

STC32G144K246（32 位 80251 内核，246KB Flash，16K edata + 128K xdata）的
**最小 LED 闪烁工程**：P0.0 接 LED（低电平点亮），P4.1 作 LED 电源使能（低有效），
每 500ms 翻转一次。使用 **sdcc-c251（SDCC 4.6.0 的 MCS-251 分支）+ make** 构建，
不依赖 Keil / Windows。

## 当前状态

| 项目 | 状态 |
| --- | --- |
| 编译 / 汇编 / 链接 | 通过（sdcc → .rel → .ihx） |
| 固件格式 | `build/*.ihx` + `build/*.hex` + `build/*.bin` |
| 静态自检 `make verify` | **16 项全部通过** |
| 上板实测 | **尚未进行**（需 STC32G144K246 实验箱/最小系统） |

## 关键配置

| 项 | 值 |
| --- | --- |
| 芯片 | STC32G144K246（LQFP100，246KB Flash / 16K edata + 128K xdata + 4K RAM） |
| 内核 | Intel MCS-251（80251），兼容 1T 8051 |
| 工具链 | `toolchains/sdcc-c251/bin/sdcc`（SDCC 4.6.0，`-mmcs251`） |
| 代码模型 | `--model-small`（16 位数据/代码指针） |
| 代码定位 | `--code-loc 0xFF0000`（STC 复位向量在 `FF:0000`） |
| 片内 RAM | `--iram-size 0x4000`（16KB edata） |
| 主频 | 按 **24MHz HIRC** 校准（ISP 选 48MHz 时改 `MAIN_Fosc`） |
| LED | **P0.0** 强推挽输出，低电平点亮 |
| LED 电源 | **P4.1** 强推挽输出，低电平使能 |
| WTST | 1（i-Cache 常开，硬件禁止写 0） |
| 固件大小 | text/ROM **255 B**（当前版本） |

> ⚠️ **栈的边界**：sdcc-c251 的默认启动代码把 8 位栈指针初始化到 `0x08`，
> `.mem` 报告“初始栈顶 0x08，约 120B 可用”。本 blink 只用少量栈，没问题；
> 但要做大数组/深递归/RTOS 时，必须改启动代码把 `SPH:SP` 指到 edata 顶部
> （官方 Keil 工程一般把栈放 edata 高地址）。详见第 6 节“已知限制”。

## 工程结构

```
stc32g144k246-blink/
├── Makefile                      # 全部构建目标，TOOLCHAIN 指向本地 sdcc-c251
├── README.md                     # 本文档
├── PIN_GUIDE.md                  # LQFP100 全部 100 脚 + 复用功能超级详细指南
├── Config/
│   └── stc32g144k246_conf.h      # 主频、LED 引脚、闪烁周期
├── Core/
│   ├── Inc/delay.h
│   └── Src/{main.c,delay.c}
├── Drivers/
│   └── STC32G/
│       ├── Inc/
│       │   ├── STC32G144K246.H   # 官方原版头文件（Keil 语法，只读参考）
│       │   └── stc32g144k246_regs.h  # SDCC 兼容 SFR 定义（工程实际使用）
│       └── README.md             # 官方头文件来源与兼容说明
├── tools/
│   └── verify.py                 # 16 项静态自检
└── build/                        # 产物（.rel/.ihx/.hex/.bin/.map/.mem/.lst）
```

## 构建 / 自检

```bash
make            # 产出 build/stc32g144k246-blink.{ihx,hex,bin}
make size       # 从 .mem 打印 ROM 占用与初始栈顶
make dis        # 列出 sdcc 生成的 .lst 汇编列表
make symbols    # 打印 map 中的符号地址
make verify     # 16 项静态自检（入口、地址、符号、ROM、bin、SFR 初始化）
make clean
```

### 已验证的构建输出

```text
:0200000400FFFB                      ← 扩展线性地址 0x00FF（即 FF:0000 段）
:070000000200038AFF000D5E            ← FF:0000: LJMP 0x0003; FF:0003: EJMP 0xFF000D
ROM/EPROM/FLASH  0xff0000 0xff00fe  255 B
```

`make verify` 的检查项：

| 检查 | 结果 |
| --- | --- |
| IHX 可解析、地址全部 ≥ 0xFF0000 | ✅ |
| FF:0000 是 LJMP，FF:0003 是 EJMP 到 FF:00xx | ✅ |
| 复位启动目标落在 0xFF0000..0xFF00FF | ✅ |
| `s_HOME == 0xFF0000`，`_main/_delay_ms` 存在 | ✅ |
| ROM 起始 0xFF0000、大小 255B（1..246KB） | ✅ |
| `.bin` 大小 == ROM 字节数 | ✅ |
| `main.lst` 含 WTST/CKCON/P0M0/P4M0 初始化 | ✅ |

> 编译时 sdcc-c251 会打印 `__has_builtin redefined` / `__STDC_HOSTED__ redefined`
> 警告（自带预处理器与系统头冲突），**不影响产物**，可忽略。

## 烧录

### 方式 A：官方 AiCube-ISP（Windows，最稳）

1. 打开 AiCube-ISP / STC-ISP，选芯片 **STC32G144K246**；
2. 选 `build/stc32g144k246-blink.hex`；
3. 选下载口：**USB（P3.0/P3.1）** 或串口（P3.0/P3.1，或切换到其它串口脚）；
4. 点“下载”，然后给 MCU 上电/复位（冷启动进入 ISP）。

### 方式 B：USB 直接下载（免 USB-TTL）

- P3.0 = D−、P3.1 = D+（各串 22Ω）；
- **按住 P3.2 再上电**，进入 USB 等待下载模式；
- 电脑端 ISP 工具点下载。

### 方式 C：macOS / 跨平台

- `pip install stcgal`，用串口接 P3.0/P3.1 冷启动下载；
- 注意：stcgal 主流支持 STC8/15/89 等 51 系列，**STC32G 的 251 内核支持需实测**；
  不行就用 Wine/虚拟机跑 AiCube-ISP。
- **OpenOCD 不支持 STC**（无 target/flash 驱动，STC 走 UART ISP），不要用
  `openocd -f target/stc32g...` 之类配置——不存在。详见 `libraries/stc/README.md`。

## 扩展指南

### 改 LED 引脚

只改 `Config/stc32g144k246_conf.h`：

```c
#define LED_ON()     do { P20 = 0; } while (0)   /* P2.0 低电平亮 */
#define LED_OFF()    do { P20 = 1; } while (0)
#define LED_TOGGLE() do { P20 = !P20; } while (0)
```

同时在 `Core/Src/main.c` 的 `LED_Init()` 里把对应口配成推挽（`PnM1=0; PnM0=0xFF;`）。
完整引脚/复用表见 `PIN_GUIDE.md`。

### 加串口

- 串口 1 默认 `P3.0=RxD / P3.1=TxD`，也可 `P_SW1.S1_S[1:0]` 切到
  `P3.6/P3.7`、`P1.6/P1.7`、`P4.3/P4.4`；
- 波特率时钟最高 `FOSC/4`，8 组 USART 均支持同步/异步/SPI/LIN/IrDA/ISO7816；
- 需要 `EAXFR` 才能访问 XFR 区的串口扩展寄存器。

### 加定时器

- T0/T1 有模式 0（16 位自动重装 + 8 位预分频 = 24 位）、模式 1/2/3；
- **T0 模式 3 是不可屏蔽中断（NMI）16 位自动重装**，适合做 RTOS 节拍；
- T2~T11/T17/T18 固定 16 位自动重装；T17/T18 可同步触发双 ADC/双 DAC；
- 精确延时请用定时器，不要用 `delay_ms()`（i-Cache 下不精确）。

### 加中断（重要）

sdcc-c251 的复位启动代码占用 `FF:0000~FF:0006`，传统中断向量从 `FF:0003`
开始每 8 字节一个入口。**不要直接照抄 Keil 的 `interrupt N`**；需要中断时
先确认 sdcc 的向量表布局，必要时用汇编自定义向量区，或参考 sdcc-c251 的
`__interrupt` 用法。当前工程不使能任何中断。

## 已知限制

1. **栈只有约 120B**（sdcc-c251 默认 8 位 SP 初始化到 0x08）。大工程建议：
   - 在启动代码/汇编里把 `SPH:SP` 设为 `0x3FFF`（edata 顶部），或
   - 把大数组/缓冲区放 `xdata`（`__xdata`），不要把大局部变量放栈上。
2. **sdcc-c251 是社区分支**，不是 SDCC 官方版；生成代码的 ABI/优化与 Keil C251
   不完全一致，混合链接 Keil 编译的 `.LIB`（如 `stc_usb_cdc_32g.LIB`）不可行。
3. **无官方 macOS 烧录工具**：编译用 sdcc-c251，烧录仍需 AiCube-ISP（Wine/虚拟机）
   或 stcgal。
4. **未上板实测**：当前只做了编译 + 静态自检；上板后需确认 LED 极性、P4.1 电源
   使能、HIRC 实际频率（影响闪烁周期）。

## 相关文档

- 芯片总览/存储器/外设/工具链/烧录：`libraries/stc/README.md`
- LQFP100 全部 100 脚 + 复用功能索引：`PIN_GUIDE.md`
- 官方例程包：`libraries/stc/STC32G144K246/DEMO-CODE/`
- 官方原版头文件：`Drivers/STC32G/Inc/STC32G144K246.H`
