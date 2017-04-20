SHELL=/bin/sh
CC = gcc
CFLAGS += -Wall -fno-strength-reduce #-pedantic
DESTDIR = /usr
INSTALL_BINPATH = $(DESTDIR)/bin
INSTALL_MANPATH = $(DESTDIR)/man
PROGNAME = Welcome2L
PROGNAME_LNK = welcome2l
VERSION = 3.04
VERSION_DATE = 1999
RPM_RELEASE = 1
RPM_BUILDPATH = /usr/src/redhat
#RPM_ICONNAME = 

$(PROGNAME): main.o ansi.o 	     
	$(CC) $(LDFLAGS) -v *.o -o $(PROGNAME)

install : 
	/usr/bin/install -m 755 $(PROGNAME) $(INSTALL_BINPATH)
	/usr/bin/install -m 644 $(PROGNAME).1 $(INSTALL_MANPATH)/man1
	ln -fs $(PROGNAME) $(INSTALL_BINPATH)/$(PROGNAME_LNK)
	ln -fs $(PROGNAME).1 $(INSTALL_MANPATH)/man1/$(PROGNAME_LNK).1

uninstall : 
	rm -f $(INSTALL_BINPATH)/$(PROGNAME)
	rm -f $(INSTALL_BINPATH)/$(PROGNAME_LNK)
	rm -f $(INSTALL_MANPATH)/man1/$(PROGNAME).1
	rm -f $(INSTALL_MANPATH)/man1/$(PROGNAME_LNK).1

.c.o:   
	$(CC) -c $(CFLAGS) $<

main.o:  main.c version.h ansi/*.h



version.h : FORCE	
	@echo "/* this header is automatically generated */" > version.h
	@echo "/* and recreated for each new compilation */" >> version.h
	@echo "#define PROG_NAME \"$(PROGNAME)\"" >> version.h
	@echo "#define PROG_FULL_NAME \"Welcome to Linux\"">>version.h
	@echo "#define PROG_AUTHOR \"Jean-Marc Jacquet <jm@littleigloo.org>\"">>version.h
	@echo "#define LITTLEIGLOO \"Little Igloo - http://www.littleigloo.org\"">>version.h
	@echo "#define PROG_VERSION \"$(VERSION)\"" >> version.h
	@echo "#define VERSION_DATE \"$(VERSION_DATE)\"">> version.h
	@echo "#define COMPILE_DATE \"`date +%D`\"" >> version.h
	@echo "#define COMPILE_TIME \"`date +%T`\"" >> version.h
	@echo "#define COMPILE_BY \"`whoami`\"" >> version.h
	@echo "#define COMPILE_HOST \"`hostname`\"" >> version.h
	@echo "#define CC_VERSION \"`$(CC) -v 2>&1 | tail -n 1`\"">>version.h
	@echo "#define CODE_LINES `cat *.c *.h | grep -ch -e \"[[:alpha:]|[:punct:]]\"`">>version.h

FORCE:	

clean: dist_clean
	rm -f $(PROGNAME) *.o version.h

dist_clean:
	rm -f *~ core build/* 


###### ONLY FOR BUILDING DISTRIBUTION USAGE ##########

doc:
	@if test -x /usr/bin/pod2man; then \
	    touch distrib/$(PROGNAME).pod;\
	    pod2man distrib/$(PROGNAME).pod \
		--center="$(PROGNAME) User's Manual" \
		--release="$(PROGNAME) Version $(VERSION)" \
		> $(PROGNAME).1; \
	fi;

dist: $(PROGNAME) doc dist_clean
	rm -f build/*.tgz
	mkdir $(PROGNAME)-$(VERSION)
	mkdir $(PROGNAME)-$(VERSION)/ansi
	mkdir $(PROGNAME)-$(VERSION)/distrib
	mkdir $(PROGNAME)-$(VERSION)/screenshots
	cp screenshots/*.* $(PROGNAME)-$(VERSION)/screenshots
	cp ansi/*.* $(PROGNAME)-$(VERSION)/ansi
	cp distrib/*.* $(PROGNAME)-$(VERSION)/distrib
	cp *.c *.h *.lsm *.1\
	   AUTHORS BUGS ChangeLog COPYING INSTALL NEWS \
	   README TODO THANKS Makefile $(PROGNAME)-$(VERSION)
	tar cvf $(PROGNAME)-$(VERSION).src.tar $(PROGNAME)-$(VERSION)
	bzip2 -k $(PROGNAME)-$(VERSION).src.tar 
	mv $(PROGNAME)-$(VERSION).src.tar.bz2 build/
	gzip $(PROGNAME)-$(VERSION).src.tar 
	mv $(PROGNAME)-$(VERSION).src.tar.gz build/
	rm -f $(PROGNAME)-$(VERSION)/*.c $(PROGNAME)-$(VERSION)/*.h
	rm -Rf $(PROGNAME)-$(VERSION)/ansi
	cp $(PROGNAME) $(PROGNAME)-$(VERSION)/$(PROGNAME)
	tar cvf $(PROGNAME)-$(VERSION).bin.tar $(PROGNAME)-$(VERSION)
	bzip2 -k $(PROGNAME)-$(VERSION).bin.tar
	mv $(PROGNAME)-$(VERSION).bin.tar.bz2 build/
	gzip -9 $(PROGNAME)-$(VERSION).bin.tar
	mv $(PROGNAME)-$(VERSION).bin.tar.gz build/
	cp *.lsm build/
	rm -Rf $(PROGNAME)-$(VERSION)
	du -k build/*.*

rpm: dist 
	sed -e s,THIS_VERSION,$(VERSION), \
	    -e s,THIS_RPM_RELEASE,$(RPM_RELEASE), \
	    -e s,THIS_DATA_FILE,$(DATA_FILE), \
	    -e s,THIS_DATA_DIR,$(DATA_DIR), \
	    distrib/$(PROGNAME).spec > $(RPM_BUILDPATH)/SPECS/$(PROGNAME)-$(VERSION).spec
	cp -f build/$(PROGNAME)-$(VERSION).src.tar.gz $(RPM_BUILDPATH)/SOURCES
	#cp -f distrib/$(RPM_ICONNAME) $(RPM_BUILDPATH)/SOURCES
	rpm -ba --clean $(RPM_BUILDPATH)/SPECS/$(PROGNAME)-$(VERSION).spec
	mv $(RPM_BUILDPATH)/RPMS/i386/$(PROGNAME)-$(VERSION)-$(RPM_RELEASE)* build/
	mv $(RPM_BUILDPATH)/SRPMS/$(PROGNAME)-$(VERSION)-$(RPM_RELEASE)* build/      
    
