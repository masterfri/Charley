CXX=g++
LD=$(CXX)

DESTDIR?=
PREFIX?=/usr/local

CXXFLAGS=$(shell pkg-config gl sdl glu --cflags)
LDLIBS=$(shell pkg-config gl sdl glu --libs) -lSDL_ttf -lSDL_mixer -lSDL_image
LDFLAGS=$(LDLIBS)

DIRMODE?=0755
SHAREFILEMODE=0644
BINFILEMODE=0755
OID?=0
GID?=0
INSTALL?=/usr/bin/install
INSTALL_FLAGS=-o $(OID) -g $(GID)

FONTSDIR=Fonts
FONTS=EagleLake-Regular.ttf vinque.ttf
IMAGESDIR=Images
IMAGES=credits.png \
	gui-items.png \
	icon.png \
	loading.png \
	main.png \
	pause.png \
	title.png
LEVELSDIR=Levels
LEVELS=BlueBedRoom.png \
	BlueEatRoom.png \
	BlueLibrary.png \
	CentralCor.png \
	GreenEatRoom.png \
	GuestRoom.png \
	levels.ds \
	NortTower.png \
	OstCor.png \
	OstTower.png \
	RedBedRoom.png \
	RedComeInRoom.png \
	RedLibrary.png \
	SouthCor.png \
	SouthTower.png \
	TeaRoom.png \
	WestCor.png \
	WorkerRoom.png \
	WorkRoom.png
MODELSDIR=Models
MODELS=base.3dm \
	bed.3dm \
	bigtable.3dm \
	block1.3dm \
	block2.3dm \
	block3.3dm \
	bookcase.3dm \
	button.3dm \
	chair.3dm \
	clock.3dm \
	door.3dm \
	elevator.3dm \
	flower.3dm \
	hero.3dm \
	key.3dm \
	table.3dm \
	teleport.3dm \
	wall.3dm \
	wall2.3dm \
	window.3dm
TEXTURESDIR=Models/Textures
TEXTURES=bg1.png \
	bg2.png \
	bg3.png \
	bg4.png \
	bg5.png \
	bg6.png \
	bg7.png \
	bg8.png \
	bg9.png \
	block1.png \
	block2.png \
	block3.png \
	book.png \
	cheese.png \
	clock.png \
	clock_face.png \
	door.png \
	dstone.png \
	earth.png \
	elevator.png \
	Exit_Mark0.png \
	Exit_Mark1.png \
	Exit_Mark2.png \
	Exit_Mark3.png \
	Exit_Mark4.png \
	face.png \
	g_manty.png \
	panel1.png \
	pod.png \
	shestern.png \
	stillage.png \
	stone.png \
	t_cl.png \
	t_disk.png \
	table_box.png \
	tros.png \
	window_glass.png \
	wood.png
SOUNDSDIR=Sounds
SOUNDS=button_down.wav \
	button_up.wav \
	door.wav \
	door2.wav \
	rolling.wav \
	shelkunchik.ogg \
	stone_break.wav \
	stone_col.wav \
	stone_shift.wav \
	teleport.wav

APPNAME=charleygame

DATA=build
BUILD=build
SRCDIR=src
OBJDIR=build
SOURCES=main.cpp \
		matrix.cpp \
		3dml.cpp \
		strproc.cpp \
		level.cpp \
		hero.cpp \
		rc.cpp \
		spectator.cpp \
		controller.cpp \
		display.cpp \
		screen.cpp \
		config.cpp
RCCONF=$(SRCDIR)/rc-conf.h

OBJ=$(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))
BIN=$(BUILD)/charleygame-bin
TARBALL=charley.tar.gz
USRSHAREDIRS=$(addprefix $(DESTDIR)/$(PREFIX)/share/$(APPNAME)/, $(FONTSDIR) $(IMAGESDIR) $(LEVELSDIR) $(MODELSDIR) $(TEXTURESDIR) $(SOUNDSDIR))
USRBINDIRS=$(DESTDIR)/$(PREFIX)/bin
DESKTOP = /usr/share/applications/charleygame.desktop
PIXMAP = /usr/share/pixmaps/charleygame.png

OS=$(shell uname)

ifeq ($(OS),Darwin)
TAR=gnutar
else
TAR=tar
endif

.PHONY: all clean dist install install-dirs install-files install-launcher remove

all: $(BIN)

$(BIN): $(RCCONF) $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(RCCONF): 
	echo '#define RC_ROOT "$(DESTDIR)/$(PREFIX)/share/$(APPNAME)/"' > $@

clean:
	rm -f $(BIN) $(OBJ) $(RCCONF) $(TARBALL)

dist: $(TARBALL)

$(TARBALL):
	ln -s build charley
	$(TAR) --exclude=*.o -czf $@ charley/*
	rm charley

install: $(BIN) install-dirs install-files install-launcher

install-dirs:
	$(INSTALL) -d $(INSTALL_FLAGS) -m $(DIRMODE) $(USRSHAREDIRS) $(USRBINDIRS)

install-files:
	$(INSTALL) -c $(INSTALL_FLAGS) -m $(SHAREFILEMODE) $(addprefix $(DATA)/$(FONTSDIR)/, $(FONTS)) \
		$(DESTDIR)/$(PREFIX)/share/$(APPNAME)/$(FONTSDIR)
	$(INSTALL) -c $(INSTALL_FLAGS) -m $(SHAREFILEMODE) $(addprefix $(DATA)/$(IMAGESDIR)/, $(IMAGES)) \
		$(DESTDIR)/$(PREFIX)/share/$(APPNAME)/$(IMAGESDIR)
	$(INSTALL) -c $(INSTALL_FLAGS) -m $(SHAREFILEMODE) $(addprefix $(DATA)/$(LEVELSDIR)/, $(LEVELS)) \
		$(DESTDIR)/$(PREFIX)/share/$(APPNAME)/$(LEVELSDIR)
	$(INSTALL) -c $(INSTALL_FLAGS) -m $(SHAREFILEMODE) $(addprefix $(DATA)/$(MODELSDIR)/, $(MODELS)) \
		$(DESTDIR)/$(PREFIX)/share/$(APPNAME)/$(MODELSDIR)
	$(INSTALL) -c $(INSTALL_FLAGS) -m $(SHAREFILEMODE) $(addprefix $(DATA)/$(TEXTURESDIR)/, $(TEXTURES)) \
		$(DESTDIR)/$(PREFIX)/share/$(APPNAME)/$(TEXTURESDIR)
	$(INSTALL) -c $(INSTALL_FLAGS) -m $(SHAREFILEMODE) $(addprefix $(DATA)/$(SOUNDSDIR)/, $(SOUNDS)) \
		$(DESTDIR)/$(PREFIX)/share/$(APPNAME)/$(SOUNDSDIR)
	$(INSTALL) -c $(INSTALL_FLAGS) -m $(BINFILEMODE) $(BIN) $(DESTDIR)/$(PREFIX)/bin/$(APPNAME)

install-launcher:
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
	cp $(DATA)/Images/icon.png $(PIXMAP)

remove:
	rm $(DESTDIR)/$(PREFIX)/bin/$(APPNAME) $(DESKTOP) $(PIXMAP)
	rm -rf $(DESTDIR)/$(PREFIX)/share/$(APPNAME)
	
