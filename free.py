import os
import sys

# Iterate through all possible file descriptors (usually up to 1024, but you can increase this)
for fd in range(3, 1024):  # Start from 3, as 0, 1, 2 are usually stdin, stdout, and stderr
    try:
        os.close(fd)
    except OSError:
        pass  # Ignore error if the file descriptor is already closed
