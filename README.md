## self guide for my laptop system

this document serves as a guide for myself and it is **HEAVILY** applied for my machine. \
this also serves as my personal notes of things i might eventually forget.

#### os

as the world file might suggest, this is an alpine config (you might also get it work on any other os, but the world file doesnt apply).

#### rebuilding the kernel

my laptop needs very specific modules for the integrated mic that arent activated in alpine's kernel. \
for that purpose, we need to rebuild the kernel with these modules on.

clone [aports](https://git.alpinelinux.org/aports/) (swap <version> for the actual version):

```ash
~ $ git clone --depth 1 --branch <version>-stable https://git.alpinelinux.org/aports/
```

then we decide which kernel we want to modify. in my case, version 3.24, i want to modify linux-stable, which lives in /community:

```ash
~ $ cd aports/community/linux-stable
```

copy the default configuration to a different .config file and add the modules:

```ash
~/aports/community/linux-stable $ cp stable.x86_64.config stable-mic.x86_64.config

~/aports/community/linux-stable $ cat >> stable-mic.x86_64.config <<'EOF'
CONFIG_SND_AMD_ASOC_ACP70=m
CONFIG_SND_SOC_SOF_AMD_ACP70=m
CONFIG_SOUNDWIRE_AMD=m
CONFIG_SND_SOC_AMD_LEGACY_MACH=m
CONFIG_SND_SOC_AMD_SOF_SDW_MACH=m
CONFIG_SND_SOC_AMD_LEGACY_SDW_MACH=m
CONFIG_IKCONFIG_PROC=y
EOF
```

we also need to update [APKBUILD](https://git.alpinelinux.org/aports/tree/community/linux-stable/APKBUILD?h=3.24-stable) file, since in this version it is hardcoded to detect only `stable.x86_64.conf`.

change `_flavor`: \
from: stable \
to: stable-mic

change `source`: \
from: stable.x86_64.config \
to: stable-mic.x86_64.config

```ash
_flavor=stable-mic
source= "... 
         stable-mic.x86_64.config
         "
```

then recalculate checksums and build it:

```ash
~/aports/community/linux-stable $ abuild checksum
~/aports/community/linux-stable $ time abuild -rK 2>&1 | tee ~/build.log
```

#### dwl

my dwl config is pretty basic, i didn't change anything major besides `config.def.h`, where i added printscrn support, volume change and mute support and brightness control. see `patches` to see which dwl-patches i've applied.

#### slstatus

in slstatus, i added to `components/wifi.c` a piece of code to show a nerd font icon alongside wifi ESSID (make sure to add the function prototype to `slstatus.h` so the config files can find it):

```c
const char *
wifi_essid_icon(const char *interface)
{
	const char *perc, *essid;
	unsigned long ul_perc, idx;
	static const char *icons[5] = {
		"󰤮", "󰤟", "󰤢", "󰤥", "󰤨",
	};

	if (!(perc = wifi_perc(interface)) || !(essid = wifi_essid(interface)))
		return bprintf("%s ", icons[0]);

	ul_perc = strtoul(perc, NULL, 10);

	// map 0 - 100 to 0 - 4; factor = 25
	idx = 1 + ul_perc / 25;
	if (idx > 4)
		idx = 4;

	return bprintf("%s  %s", icons[idx], essid);
}
```

#### portals and themes

i use `xdg-desktop-portal` and `xdg-desktop-portal-wlr` and these work for now.

for day-night theme switching, i use `darkman`. \
`darkman` has its own config under `.config/darkman/config.yaml`, where we define our geolocation (i live in Lisbon):

```ash
lat: 38.736946
lng: -9.142685
usegeoclue: false
```

to share the theme with the portals, where other apps also read from, add a `.config/xdg-desktop-portal/$XDG_CURRENT_DESKTOP-portals.conf` (changes $XDG_CURRENT_DESKTOP for your own) with the contents:

```ash
[preferred]
org.freedesktop.impl.portal.Settings=darkman
```

#### greeter & tuigreet

i use greetd with tuigreet.

contents of `/etc/greetd/config.toml`:

```ash
[terminal]
vt = 7

[default_session]
command = "tuigreet --remember --time --cmd /usr/local/bin/dwl-run"
user = "greetd"
```

contents of `/etc/conf.d/greetd`

```ash
rc_need=seatd
```

greetd uses pam, and the config files for pam are (i simply added the `fprintd` line):

`/etc/pam.d/greetd`

```ash
#%PAM-1.0

auth     sufficient  pam_fprintd.so max-tries=2
auth     include     base-auth
account  include     base-account
password include     base-password
session  include     base-session
```

#### fonts

i use `font-iosevka-base` for monospace, `font-inter` for sans-serif, `font-noto-cjk` and `font-noto-emoji` for extra symbols. these are from apk. \
i use `iosevka-nerd-font` for nerd font symbols, and this one is (obviously) from NerdFonts. \
i use `font-lora` from google fonts for serif font.

#### librewolf

i had to manually change `font.name-list.monospace.x-{unicode, western}` to `Iosevka Term Extended` in about:config because, even though i specifically define it in [fontconfig](./.config/fontconfig/fonts.conf), librewolf defaults to Iosevka Regular
