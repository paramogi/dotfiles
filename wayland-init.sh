#!/bin/sh
export XDG_CURRENT_DESKTOP=wlroots
export XCURSOR_SIZE=20

exec dbus-run-session sh -c 'slstatus -s | dwl'
