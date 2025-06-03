# This contains the default build configuration. It can be edited directly, or
# the file local.mk can be used.

# Select the compiler to be used. See compiler.mk for supported compilers.
# Even if your compiler is not officially supported, it might still work; C++14
# support is required, however.
#
CXX=		g++

# Select the optimization level. Because of how the program is structured, it
# benefits very much from automatically inlined functions, and even more so if
# the compiler is able to do link-time optimization. On the other hand, most
# interactive fiction does not need a fast interpreter. For good results from
# g++ and clang++, the following is recommended:
#
# -O3 -flto=auto
#
OPT=		-O2

# Select the target platform. Valid values are:
#
# • unix (for POSIX systems)
# • win32 (for Win32 systems)
# • dos (for DOS systems)
#
# Any other value (or none at all) will result in the use of standard C++14
# functions only.
#
PLATFORM=	unix

# Set the target directories for the install of the binary and the man page.
PREFIX=		/usr/local
BINDIR=		$(PREFIX)/bin
MANDIR=		$(PREFIX)/man/man6

# If defined, this variable controls which Glk implementation will be used. The
# Glk library must exist in a directory of the same name (so if Glk is set to
# glktermw, the Glk implementation will be found in glktermw/).
# If this value is empty or not defined, I/O will be done solely in terms of
# C++’s standard library.
#
GLK=

# Glk is almost platform neutral, but does require a small amount of
# platform-specific startup code. Most implementations provide a so-called Unix
# startup (even on non-Unix platforms), but Windows Glk provides its own. Bocfel
# will assume a Unix startup unless $GLK is set to winglk, in which case it
# assumes a Windows startup. This can be overridden here if necessary. For Unix,
# this must be “unix”. For Windows, it must be “windows”.
#
GLKSTARTUP=

# The Glk specification recommends calling glk_tick() every instruction, because
# there may be some platforms that need to do some processing every now and
# again. However, glk_tick() in the following implementations does not actually
# do anything:
#
# cheapglk, glkterm(w), GlkDOS, XGlk, Gargoyle (Mac), Spatterlight
#
# On the other hand, the following do require glk_tick() to be called:
#
# Windows Glk, Gargoyle (Qt)
#
# By default, for performance reasons, glk_tick() is not called. If the
# following variable is defined (with any value), glk_tick() will be called.
#
# GLK_TICK=		1

# While it’s not required, most Glk libraries include support for Blorb
# files as described by the Glk spec. Bocfel assumes that if Glk is
# being used, the Blorb functions are available. If they’re not, this
# setting will instruct Bocfel to not make use of them.
#
# GLK_NO_BLORB=		1

# On Unix platforms, when not building against Glk, Curses can be used
# in order to get styles and colors at the terminal. If the following
# variable is defined (with any value), Curses support will be disabled.
#
# NO_CURSES=		1

# By default, many safety checks are performed, such as verifying that a story
# is not overflowing its stack, performing invalid memory accesses, and so on.
# If this variable is defined (with any value), these checks are not performed.
# This speeds up the interpreter, but misbehaving story files could cause
# unpredictable results, including crashing.
#
# NO_SAFETY_CHECKS=	1

# Rudimentary cheating support is available. This allows certain parts of
# memory to be “frozen”, meaning no matter what the game does, they will always
# contain specific values. The intended use for this is to freeze hunger and
# thirst daemons: assuming that the daemons work by using a increasing or
# decreasing value to indicate hunger or lack thereof, freezing this counter can
# forever report that the user is sated. It is possible to build without
# support for cheating, which might be useful because cheating slows down memory
# access. If the following variable is defined (with any value), cheating will
# be disabled.
#
# NO_CHEAT=		1

# Debugging watchpoints are available, which means that values in the Z-machine
# can be watched for change, and reported on when such changes occur. This
# slows down memory access, so can be disabled. If the following variable is
# defined (with any value), watchpoints will be disabled.
#
# NO_WATCHPOINTS=	1

# Some Glk implementations extend Glk to support opening files with an arbitrary
# filename, bypassing any modifications that normally occur, such as adding an
# extension. The impetus for the creation of this extension was to support
# systems without standard I/O routines. In general this is not necessary, since
# most implementations do support standard I/O; but in some cases (e.g. inside
# of a web browser), this is necessary. If the following variable is defined
# (with any value), then all I/O will be done through Glk instead of a mixture
# of Glk and standard I/O. This requires the GLKUNIX_FILEREF_CREATE_UNCLEANED
# extension.
#
# NO_STDIO=		1

# The Z-machine requires user input to be converted to lowercase. By default
# Bocfel uses a Unicode translation function adapted from Kevin Bracey’s
# Zip2000. This covers Latin, Greek, and Cyrillic. Bocfel can also use the ICU
# project’s Unicode library, which provides a wider range of Unicode support.
# This is probably not useful, but if a game is ever written in Armenian (for
# example), it might come in handy. The ICU libraries and headers must be
# present.
#
# ICU=			1

# Bocfel has built-in patches for bugs in Infocom games, and can also load
# patches from the user’s configuration file. There is, optionally, a third way
# to load user patches: from a file containing only patches. The intent of this
# file is for those distributing Bocfel (which mainly means Gargoyle and
# Spatterlight) to provide up-to-the-minute patches for all sorts of games. The
# rationale here is that there are hundreds of non-Infocom games, with any
# number of possible bugs in them, and including patches directly in Bocfel
# isn’t the most efficient way to deal with this, both because it would bloat
# the hard-coded patches list, and because patches may need to be released on a
# different schedule than Bocfel itself. If this variable is set, it is taken to
# be a path to a patch file.
#
# PATCH_FILE=

-include local.mk
