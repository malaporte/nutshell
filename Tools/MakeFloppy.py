#*******************************************************************************
# Copyright (C) Martin Laporte.
#*******************************************************************************

import sys
import math
import struct

# Read the bootstrap
bootstrap = open("Bootstrap/Bootstrap.bin", "rb").read()

# Read the kernel
kernel = open("Kernel/Kernel.bin", "rb").read()

# TODO: Find a way to avoid that!
# Skip the PE headers
#kernel = kernel[0x1000:]

# Compute their size in 512K sectors
bootstrapSize = int(math.ceil(len(bootstrap) / 512.0))
kernelSize = int(math.ceil(len(kernel) / 512.0))

# Open the output file
output = open("FloppyImage.img", "wb")

# Write the bootstrap and the kernel into it
output.write(bootstrap)
output.seek(bootstrapSize * 512)
output.write(kernel)
output.seek((bootstrapSize + kernelSize) * 512)
output.truncate()

# Write the kernel loader and kernel size to the boot sector
output.seek(506)
output.write(struct.pack("H", bootstrapSize - 1))
output.write(struct.pack("H", kernelSize))

# Output some debug information
print "Bootstrap is %d bytes and fits in %d sectors." % (len(bootstrap), bootstrapSize)
print "Kernel is %d bytes and fits in %d sectors." % (len(kernel), kernelSize)
