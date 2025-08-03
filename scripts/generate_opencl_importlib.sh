#!/bin/bash
# OpenCL import library auto-generation script

# - Extract cl function names from source and generate OpenCL.def
# - Create build/windows/libOpenCL.dll.a with dlltool
# - Remove temporary files

set -e


WORKDIR="/tmp/opencl_importlib_gen_$$"
mkdir -p "$WORKDIR"
DEF_FILE="$WORKDIR/OpenCL.def"

mkdir -p build/windows
LIB_FILE="build/windows/libOpenCL.dll.a"

# 1. Extract cl function names from source
grep -rohE "\bcl[A-Z][a-zA-Z0-9_]*\s*\(" --include="*.cpp" --include="*.c" --include="*.h" src/ include/ \
  | sed 's/.*\(cl[A-Za-z0-9_]*\).*/\1/' | sort | uniq > "$WORKDIR/cl_funcs.txt"

# 2. Generate OpenCL.def
echo "LIBRARY OpenCL.dll" > "$DEF_FILE"
echo "EXPORTS" >> "$DEF_FILE"
cat "$WORKDIR/cl_funcs.txt" >> "$DEF_FILE"

# 3. Generate import library with dlltool
x86_64-w64-mingw32-dlltool -d "$DEF_FILE" -l "$LIB_FILE" -k


# 4. Placement (not needed, already generated in build/windows)

# 5. Remove temporary files
rm -rf "$WORKDIR"

echo "[OK] libOpenCL.dll.a generated at build/windows/"
