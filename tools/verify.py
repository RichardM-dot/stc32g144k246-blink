#!/usr/bin/env python3
"""
STC32G144K246 blink 工程静态自检。

检查项：
  1. IHX 只包含 0xFF0000 以上的地址（用户程序区）
  2. FF:0000 是复位向量：LJMP -> EJMP 0xFF00xx
  3. map 中 s_HOME == 0xFF0000，且 _main/_delay_ms 存在
  4. .mem 中 ROM 大小在 1..246KB 之间
  5. .bin 大小与 ROM 占用一致
  6. 汇编列表里能看到 WTST/CKCON/P0M0/P4M0 的初始化写入

用法：python3 -I tools/verify.py build/xxx.ihx build/xxx.map
"""
import os
import re
import sys
from pathlib import Path


def parse_ihx(path):
    mem = {}
    base = 0
    with open(path, "r", encoding="ascii", errors="replace") as f:
        for lineno, line in enumerate(f, 1):
            line = line.strip()
            if not line:
                continue
            if not line.startswith(":"):
                raise SystemExit(f"IHX 第 {lineno} 行不是 Intel HEX 记录: {line!r}")
            raw = bytes.fromhex(line[1:])
            count = raw[0]
            addr = (raw[1] << 8) | raw[2]
            rectype = raw[3]
            data = raw[4:4 + count]
            if len(data) != count:
                raise SystemExit(f"IHX 第 {lineno} 行长度不符")
            if rectype == 0x00:
                for i, b in enumerate(data):
                    mem[base + addr + i] = b
            elif rectype == 0x01:
                break
            elif rectype == 0x02:
                base = int.from_bytes(data, "big") << 4
            elif rectype == 0x04:
                base = int.from_bytes(data, "big") << 16
            else:
                # 03/05 是入口地址记录，忽略
                pass
    return mem


def parse_map_symbols(path):
    syms = {}
    pat = re.compile(r"^C:\s+([0-9A-Fa-f]+)\s+(\S+)")
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        for line in f:
            m = pat.match(line.rstrip())
            if m:
                syms[m.group(2)] = int(m.group(1), 16)
    return syms


def parse_mem(path):
    text = open(path, "r", encoding="utf-8", errors="replace").read()
    m = re.search(r"ROM/EPROM/FLASH\s+([0-9a-fA-Fx]+)\s+([0-9a-fA-Fx]+)\s+(\d+)", text)
    if not m:
        return None
    return {
        "start": int(m.group(1), 16),
        "end": int(m.group(2), 16),
        "size": int(m.group(3)),
        "stack": re.search(r"initial stack starts at: 0x([0-9a-fA-F]+)", text).group(1)
        if "initial stack starts at" in text else "?",
    }


def main():
    if len(sys.argv) < 3:
        raise SystemExit("用法: verify.py <ihx> <map>")
    ihx, mapf = sys.argv[1], sys.argv[2]
    build = Path(ihx).parent
    ok = True

    def check(cond, msg):
        nonlocal ok
        print(("  ok   " if cond else "  FAIL ") + msg)
        ok = ok and cond

    mem = parse_ihx(ihx)
    check(bool(mem), f"IHX 可解析，共 {len(mem)} 字节")
    check(min(mem) >= 0xFF0000, f"最低地址 0x{min(mem):06X} >= 0xFF0000（用户程序区）")
    check(max(mem) <= 0xFFFFFF, f"最高地址 0x{max(mem):06X} <= 0xFFFFFF")

    lo = min(mem)
    vec = [mem.get(lo + i) for i in range(8)]
    check(vec[0] == 0x02, f"FF:0000 是 LJMP（实际 {vec[0]:#04x}）")
    # sdcc 的复位跳转链：LJMP 0x0003; EJMP 0xFF00xx
    check(vec[3] == 0x8A and vec[4] == 0xFF and vec[5] == 0x00,
          f"FF:0003 是 EJMP 到 FF:00xx（实际 {vec[3]:#04x} {vec[4]:#04x} {vec[5]:#04x}）")
    target = 0xFF0000 | vec[6]
    check(0xFF0000 <= target <= 0xFF00FF,
          f"复位启动目标 0x{target:06X} 落在 0xFF0000..0xFF00FF")

    syms = parse_map_symbols(mapf)
    check(syms.get("s_HOME") == 0xFF0000,
          f"s_HOME = 0x{syms.get('s_HOME', 0):06X}（应为 0xFF0000）")
    for name in ("_main", "_delay_ms"):
        check(name in syms, f"符号 {name} 存在")

    info = parse_mem(str(build / (Path(ihx).stem + ".mem")))
    if info:
        check(1 <= info["size"] <= 246 * 1024,
              f"ROM 占用 {info['size']} B（1..245760）")
        check(info["start"] == 0xFF0000,
              f"ROM 起始 0x{info['start']:06X}（应为 0xFF0000）")
        print(f"  info 初始栈顶 0x{info['stack']}（sdcc 默认 8 位栈，详见 README 说明）")

    binf = build / (Path(ihx).stem + ".bin")
    if binf.exists():
        check(binf.stat().st_size == len(mem),
              f".bin 大小 {binf.stat().st_size} B == ROM 字节数 {len(mem)}")

    # 汇编列表：确认关键 SFR 被初始化
    main_lst = build / "main.lst"
    if main_lst.exists():
        text = main_lst.read_text(encoding="utf-8", errors="replace")
        for reg in ("WTST", "CKCON", "P0M0", "P4M0"):
            check(reg in text, f"main.lst 包含 {reg} 的初始化")

    print("\nALL CHECKS PASSED" if ok else "\nSOME CHECKS FAILED")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
