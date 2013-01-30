# makefile for FLAT (https://github.com/Cheeseness/FLAT)
# created by Marcus Møller (https://github.com/marcusmoller)

CC = g++
CFLAGS = -Wall -Wno-switch
OBJECTS = main.o                \
          Game.o                \
          utilities.o           \
          StaticObject.o        \
          vec2.o                \
          vec3.o                \
          AudioManager.o        \
          BitmapBuffer.o        \
          Character.o           \
          DetailObject.o        \
          DragonHead.o          \
          DragonSpine.o         \
          DrawableBitmap.o      \
          DrawableCircle.o      \
          DrawableLine.o        \
          DrawableObject.o      \
          DrawableText.o        \
          DrawStack.o           \
          DrawStack3D.o         \
          Floor.o               \
          FrameContainer.o      \
          FrameSuperContainer.o \
          GraphicsManager.o     \
          Pickup.o              \
          Player.o              \
          Projectile.o          \
          Splat.o               \
          Weapon.o
    


LIBS = -L/usr/local/lib -lallegro -lallegro_ttf -lallegro_audio -lallegro_acodec -lallegro_image -lallegro_main -lallegro_primitives -lallegro_font

SRCDIR = src

FLAT: $(OBJECTS)
    $(CC) $(CFLAGS) $(OBJECTS) -o out/FLAT.x86 $(LIBS)

%.o: $(SRCDIR)/%.cpp
    $(CC) $(CFLAGS) -c $<

clean:
    rm -rf *.o main
