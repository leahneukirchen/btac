ALL=btac

CFLAGS=-g -O3 -march=native -Wall -Wno-switch -Wextra -Wwrite-strings

DESTDIR=
PREFIX=/usr/local
BINDIR=$(PREFIX)/bin

all: $(ALL)

clean: FRC
	rm -f $(ALL)

install: FRC all
	mkdir -p $(DESTDIR)$(BINDIR) $(DESTDIR)$(MANDIR)/man1
	install -m0755 $(ALL) $(DESTDIR)$(BINDIR)

FRC:
