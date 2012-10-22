# You can edit these two lines below to customize installation directory
INSTALLDIR = /usr/local/lib/charleygame
STARTUP = /usr/local/bin/charleygame

CPPFLAGS = $(shell pkg-config gl sdl glu --cflags)
LDLIBS = $(shell pkg-config gl sdl glu --libs) -lSDL_ttf -lSDL_mixer -lSDL_image
SOURCES = main.cpp matrix.cpp 3dml.cpp strproc.cpp level.cpp hero.cpp rc.cpp spectator.cpp controller.cpp display.cpp screen.cpp config.cpp
APP = build/charleygame-bin
DESKTOP = /usr/share/applications/charleygame.desktop
PIXMAP = /usr/share/pixmaps/charleygame.png

all:
	g++ $(SOURCES) $(CPPFLAGS) $(LDLIBS) -o $(APP)

clean:
	rm -f $(APP) *.o
	
install:
	[ -d $(INSTALLDIR) ] || mkdir $(INSTALLDIR)
	cp -R build/* $(INSTALLDIR)
	echo "#!/bin/sh" > $(STARTUP)
	echo "cd $(INSTALLDIR)" >> $(STARTUP)
	echo "./charleygame-bin" >> $(STARTUP)
	chmod a+x $(STARTUP)
	echo "[Desktop Entry]" > $(DESKTOP)
	echo "Version=1.0" >> $(DESKTOP)
	echo "Encoding=UTF-8" >> $(DESKTOP)
	echo "Name=Charley The Ghost" >> $(DESKTOP)
	echo "Comment=3D puzzle game" >> $(DESKTOP)
	echo "Exec=charleygame" >> $(DESKTOP)
	echo "Terminal=false" >> $(DESKTOP)
	echo "Type=Application" >> $(DESKTOP)
	echo "Icon=charleygame.png" >> $(DESKTOP)
	echo "Categories=Game;" >> $(DESKTOP)
	echo "StartupNotify=false" >> $(DESKTOP)
	cp build/Images/icon.png $(PIXMAP)
	
remove:
	rm -rf $(INSTALLDIR)
	rm $(STARTUP)
	rm $(DESKTOP)
	rm $(PIXMAP)
