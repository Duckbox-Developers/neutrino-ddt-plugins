AUTOMAKE_OPTIONS = gnu

SUBDIRS = \
	include tuxwetter input getrc shellexec fx2
#	include tuxcom tuxwetter msgbox input getrc shellexec fx2
#	include tuxmail tuxtxt tuxcom tuxcal tuxclock fx2 vncviewer enigma shellexec tuxwetter sysinfo clock logomask blockads links

if BOXTYPE_ARMBOX
SUBDIRS += \
	stb-startup \
	stb-backup \
	pr-auto-timer
endif

if BOXTYPE_MIPSBOX
SUBDIRS += \
	pr-auto-timer
endif

#if BOXTYPE_ARMBOX
#SUBDIRS += \
#	stb-startup-tuxbox
#endif

#if BOXTYPE_DREAMBOX
#SUBDIRS += pip mosaic
#else
#SUBDIRS += pluginx
#endif

#pkgconfigdir = $(libdir)/pkgconfig
#pkgconfig_DATA = \
#	tuxbox-plugins.pc
