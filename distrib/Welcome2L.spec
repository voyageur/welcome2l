Summary: Linux ANSI boot logo
Name: Welcome2L
Version: THIS_VERSION
Release: THIS_RPM_RELEASE
#Icon: 
Copyright: GPL
Group: Utilities
Source: Welcome2L-THIS_VERSION.src.tar.gz
URL: http://www.LittleIgloo.org


%description
Welcome2L is a little program that may run at login time to produce a BBS
like ANSI login logo. It's very similar to Linux_Logo. But where Linux_Logo
intends to be portable, Welcome2L intends to produce the best looking
ANSI screens by making full usage of PC graphic characters. Therefore
an architecture able to display those characters (i386, Alpha with TGA adapter,
... ) is required to use it. And, even if it will work on larger screens,
it will only produce 80 column ANSI screens. 


%prep
%setup
make DESTDIR=/usr doc

%build
make DESTDIR=/usr

%install
make DESTDIR=/usr install


%files
%doc AUTHORS README INSTALL COPYING ChangeLog BUGS TODO screenshots
/usr/bin/Welcome2L
/usr/man/man1/Welcome2L.1
/usr/bin/welcome2l
/usr/man/man1/welcome2l.1

