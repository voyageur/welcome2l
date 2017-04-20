SHELL=/bin/sh
CFLAGS += -Wall -fno-strength-reduce #-pedantic
DESTDIR = /usr
INSTALL_BINPATH = $(DESTDIR)/bin
INSTALL_MANPATH = $(DESTDIR)/man
PROGNAME = Welcome2L
PROGNAME_LNK = welcome2l
VERSION = 3.05
VERSION_DATE = 2017

$(PROGNAME): main.o ansi.o 	     
	$(CC) $(LDFLAGS) *.o -o $(PROGNAME)

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

clean:
	rm -f $(PROGNAME) *.o version.h
