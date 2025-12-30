#!/usr/bin/python

import sys
import subprocess

WHITELIST = ('ECHO', 'RETURN', 'PHP', 'NOP')

def check(filename):
    try:
        output = subprocess.check_output(["php",
            "-d", "vld.active=1",
            "-d", "vld.execute=0",
            "-d", "extension=vld.so",
            "-d", "vld.format=1",
            "-d", "vld.col_sep=@",
            "-d", "log_errors=0",
            "-d", "error_log=/dev/null",
            "-B", "if (!extension_loaded('vld')) die('no vld');",
            filename],
            stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        print("Error: %s" % e)
        return 2

    for line in output.splitlines()[8:]:
        sp = line.split('@')
        if len(sp) < 5:
            continue
        opcode = sp[4]  # ,line, #, EIO, op, fetch, ext, return, operands
        if opcode not in WHITELIST:
            print("Upload_validation: Found an opcode: %s" % opcode)
            return 1
    return 0


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Usage: %0 file_to_test.php', sys.argv[0])
    else:
        sys.exit(check(sys.argv[1]))
