.PHONY: all
.PHONY: install
.PHONY: clean

ISP_PREFIX ?= /opt/isp/

INSTALL_EXCLUDES := /Makefile
INSTALL_EXCLUDES += .git
INSTALL_EXCLUDES += .gitignore
INSTALL_EXCLUDES += build
INSTALL_EXCLUDES += *~

RSYNC_EXCLUDE = $(patsubst %,--exclude '%',$(INSTALL_EXCLUDES))

all:

install:
	rm -rf $(ISP_PREFIX)/FreeRTOS
	rsync -rupE . $(ISP_PREFIX)/FreeRTOS $(RSYNC_EXCLUDE)

clean:
	find -name "*~" -delete
