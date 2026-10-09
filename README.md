## self guide for my laptop system

this document serves as a guide for myself and it is **HEAVILY** applied for my machine. \
this also serves as my personal notes of things i might eventually forget.

#### dwl

my dwl config is pretty basic, i didn't change anything major besides `config.def.h`, where i added printscrn support, volume change and mute support and brightness control. see [patches](./dwl-v0.9/patches/) to see which dwl-patches i've applied.

after locking my screen, the bar was covered by the clients. \
fix is in [this issue's](https://codeberg.org/dwl/dwl-patches/issues/602) replies. \
i notice it still happens when it locks it with the bar hidden, it will then unlock with the bar unhidden but still covered by the clients.

#### fonts

i use `Iosevka Term Expanded` for monospace, `Inter` for sans-serif, `Lora` for sans serif, `Noto CJK` and `Noto Emoji` for extra symbols. i use `Iosevka Term Nerd` for nerd font symbols, and `Terminus TTF` for the bar and mew.

since iosevka comes with bundled font families, i added this script under `/etc/fonts/conf.d/99-iosevka-term-expanded.conf`

```
<?xml version="1.0"?>
<!DOCTYPE fontconfig SYSTEM "urn:fontconfig:fonts.dtd">
<fontconfig>
	<match target="scan">
		<test name="family"><string>Iosevka Term</string></test>
		<test name="width"><const>expanded</const></test>
		<edit name="family" mode="assign" binding="same">
			<string>Iosevka Term Extended</string>
		</edit>
	</match>
</fontconfig>
```

#### power control

my laptop sadly doesnt implement s3 sleep, so i have to resort to hibernation.

`acpid` provides `/etc/acpi/handler.sh`, i changed it to hibernate on click of power button, lid close and low battery (5% threshold)

scripts: \
`/etc/acpi/handler.sh`:

```
#!/bin/sh

PATH="/usr/share/acpid:$PATH"
alias log='logger -t acpid'

hibernate() {
	echo disk > /sys/power/state
}

# <dev-class>:<dev-name>:<notif-value>:<sup-value>
case "$1:$2:$3:$4" in
button/power*)
	log 'Power button pressed'
	hibernate
;;
button/sleep*)
	log 'Sleep button pressed'
	hibernate
;;
button/lid*)
	log 'Lid closed'
	lid-closed && hibernate
;;
esac

exit 0
```

`/etc/init.d/battery-hibernate`:

```
#!/sbin/openrc-run

name="battery-hibernate"
description="Hibernate on critically low battery"

command="/usr/local/bin/battery-hibernate-watch"
command_background=true
pidfile="/run/$RC_SVCNAME.pid"

depend() {
	need localmount
	after acpid
}
```


`/usr/local/bin/battery-hibernate-watch`:

```
#!/bin/sh

THRESHOLD=${BATT_THRESHOLD:-5}
INTERVAL=${BATT_INTERVAL:-30}

bat=
for b in /sys/class/power_supply/BAT*; do
	[ -e "$b/capacity" ] && { bat=$b; break; }
done
[ -n "$bat" ] || { logger "battery-watch: no battery found"; exit 1; }

while :; do
	read -r cap    < "$bat/capacity"
	read -r status < "$bat/status"
	if [ "$status" = "Discharging" ] && [ "${cap:-100}" -le "$THRESHOLD" ]; then
		logger "battery-watch: ${cap}% discharging — hibernating"
		echo disk > /sys/power/state
		sleep 60
	fi
	sleep "$INTERVAL"
done
```
