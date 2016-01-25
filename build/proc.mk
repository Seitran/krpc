all:
	for dir in $(SUBDIRS); \
	do $(MAKE) -C $$dir all || exit 1; \
	done;
