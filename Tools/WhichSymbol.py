#*******************************************************************************
# Copyright (C) Martin Laporte.
#*******************************************************************************

import sys

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

# Enter the user request loop
while 1:
    # Read an user entry
    sys.stdout.write("Enter an address: ")
    entry = sys.stdin.readline()

    # If the user entered nothing, get out of the loop
    if entry.strip() == "" : break

    # Convert the entry to a number
    entry = int(entry, 0)

    # This will be the symbol we found
    symbol = None;

    # Go through the symbols and find the one that is the nearest
    for x in symbols :
        # Check if the current symbol better than what we already have
        if symbol is None or (x[0] <= entry and x[0] > symbol[0]) : symbol = x

    # Check if we found something
    if not symbol is None :
        print "%08X\t%s\n" % symbol
    else :
        print "Nothing was found!\n"
