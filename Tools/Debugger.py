#*******************************************************************************
# Copyright (C) Martin Laporte.
#*******************************************************************************

import sys
import os

#*******************************************************************************
# Main program.
#*******************************************************************************

# Initialize the list of symbols to empty
symbols = []

# Go through all the lines in the kernel map file
for x in open("Kernel/Kernel.map").readlines() :
    # Get a stripped version of the line
    line = x.strip()

    # Check if the line begins with an hexadecimal
    if line.startswith("0x") :
        # Split the line in tokens
        tokens = line.split(" ", 1)

        # Strip the tokens
        tokens[0] = tokens[0].strip()
        tokens[1] = tokens[1].strip()

        # Check if the second token isn't an hexadecimal
        if not tokens[1].startswith("0x") :
            # First token is the address, and the second is the symbol
            symbols.append((int(tokens[0], 0), tokens[1]))

# Sort the symbol list
symbols.sort()





# Open the named pipe connected to the serial port
fd = os.open("\\\\.\\pipe\\nutshell", os.O_RDWR)
inp = os.fdopen(fd, "r", 0)
out = os.fdopen(fd, "w", 0)

while 1 :
    sys.stdout.write("> ")
    line = sys.stdin.readline()
    out.write(line.strip() + "\0")

    while 1 :
        resp = inp.readline().strip()
        sys.stdout.write(resp)

        entry = int(resp.strip(), 0)

        continue

        # This will be the symbol we found
        symbol = None;

        # Go through the symbols and find the one that is the nearest
        for x in symbols :
            # Check if the current symbol better than what we already have
            if symbol is None or (x[0] <= entry and x[0] > symbol[0]) : symbol = x

        # Check if we found something
        if not symbol is None :
            sys.stdout.write(" %08X\t%s\n" % symbol)
        else :
            sys.stdout(" Nothing was found!\n")

        if resp.strip() == "" : break