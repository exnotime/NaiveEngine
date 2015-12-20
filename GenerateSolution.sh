#!/bin/bash
premake5.exe vs2015
(cd ./source/core/components/ &&
UpdateComponents.sh)
