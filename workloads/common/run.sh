#!/bin/bash

if [ $# -eq 0 ]; then
	echo "No argument supplied!" >&2
	poweroff
fi

if [ ! -e "$1" ]; then
	echo "Couldn't find $1!" >&2
	poweroff
fi

./$1

poweroff
