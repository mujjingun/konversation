Summary:   A user friendly IRC Client for KDE3.x
Name:      konversation
Version:   0.12
Release:   1
Copyright: GPL
Packager:  Dario Abatianni <eisfuchs@tigress.com>
Group:     X11/Applications/Networking
Requires:  libpng, kdebase3 >= 3.0
BuildRequires: libpng-devel, kdelibs-devel, arts-devel, libjpeg-devel
BuildRequires:  XFree86-devel, zlib-devel, qt-devel >= 3.0.2

%description
Konversation is a user friendly IRC client for KDE 3.x

%files
/opt/kde3/bin/konversation
/opt/kde3/share/apps/konversation/images/*
/opt/kde3/share/apps/konversation/konversationui.rc
/opt/kde3/share/icons/*/*/apps/konversation.png
/opt/kde3/share/applnk/Internet/konversation.desktop
/opt/kde3/share/locale/*/LC_MESSAGES/konversation.mo
