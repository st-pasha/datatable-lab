#!/usr/bin/env python3
import re
import subprocess
import sys
import colorama

NMIN = 45

def get_cmd(n):
    return [c.replace("{N}", str(n)).replace("--n={N}", "--n=" + str(n))
            for c in sys.argv[1:]]

try:
    if len(sys.argv) > 2 and ("{N}" in sys.argv or "--n={N}" in sys.argv):
        print("Command example: %s" % " ".join(get_cmd(42)))
    else:
        print("Usage:")
        print("    python munch.py ./sort --n {N} ...")
        print("where ... indicates pass-through parameters\n")
        exit(0)

    colorama.init()
    headers_printed = []
    ns = (list(range(2, 17)) + list(range(20, 100, 4)) +
          [100, 128, 160, 196, 256, 512, 1024])
    ns += [1 << i for i in range(11, 28)]
    ns += [3 << i for i in range(7, 27)]
    ns.sort()

    for n in ns:
        if n < NMIN:
            continue
        cmd = get_cmd(n)
        try:
            out = subprocess.check_output(cmd).decode()
        except subprocess.CalledProcessError as e:
            print(" %-8d | %s" % (n, e))
            continue
        headers = []
        values = []
        for line in out.split("\n"):
            mm = re.search(r"^\[\s*([\w\-/:#@]+)\]\s*([\d\.]+)", line)
            if mm:
                name = mm.group(1)
                value = mm.group(2)
                headers.append(name)
                if value == "-":
                    values.append(None)
                else:
                    v = float(value)
                    values.append(int(v*10 + 0.5)/10)
        if headers_printed:
            if headers != headers_printed:
                values = [values[headers.index(h)] if h in headers else None
                          for h in headers_printed]
        else:
            headers_printed = list(headers)
            divider = "+".join(["-" * 10] + ["-" * 14] * len(headers))
            print(divider)
            print(" n" + " " * 8, end="")
            for h in headers:
                print("| %12s " % h, end="")
            print()
            print(divider)
        print(" %-8d " % n, end="")
        minval = min(v for v in values if v is not None)
        for v in values:
            if v is None:
                print("|" + " " * 14, end="")
            elif v <= minval * 1.25:
                # highlight green values not too far away from min
                print("|\x1B[32m %12s \x1B[m" % v, end="")
            else:
                print("| %12s " % v, end="")
        print()
except KeyboardInterrupt:
    print("\r\x1B[33m-- Stopped.\x1B[m")
