#!/bin/bash

find . -regex '.*\.\(cpp\|hpp\|cc\|cxx\|c\|h\)' -exec clang-format -style=file -i {} \;
