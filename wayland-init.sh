#!/bin/sh
export XDG_CURRENT_DESKTOP=wlroots

exec dbus-run-session sh -c 'slstatus -s | dwl > dwl.log 2>&1'
