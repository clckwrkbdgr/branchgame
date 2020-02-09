VERSION = 0.1.0
TARGET = branch
CONFIG = qt uic release

OBJECTS_DIR = tmp
RCC_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp

TRANSLATIONS = branchgame_ru.ts
FORMS = mainwindow.ui settings.ui
HEADERS = mainwindow.h branchwidget.h branchgame.h spritegen.h
SOURCES = mainwindow.cpp branchwidget.cpp branchgame.cpp spritegen.cpp

deb.depends = $(TARGET)
deb.commands = @debpackage.py \
		branchgame \
		-v 0.1.0 \
		--maintainer \'umi041 <umi0451@gmail.com>\' \
		--bin $(TARGET) \
		--build-dir tmp \
		--dest-dir . \
		--description \'Branch-like game.\'
QMAKE_EXTRA_TARGETS += deb
