OBJS = pics.o pBill.o win2.o Game.o Horde.o Library.o Monster.o Network.o Spark.o UI.o \
  Scorelist.o Bucket.o Cable.o Computer.o Picture.o

CC = m68k-palmos-coff-gcc
CXX= m68k-palmos-coff-gcc
CSFLAGS = -O2 -S

CFLAGS = -O2
CXXFLAGS = -O2

PILRC = pilrc
OBJRES = m68k-palmos-coff-obj-res
BUILDPRC = build-prc

ICONTEXT = "pBill"
APPID = pBil

all: pBill.prc

Game.o: Game.h
Horde.o: Horde.h
Library.o: Library.h
Monster.o: Monster.h
Computer.o: Computer.h
Network.o: Network.h

pics.cc: bill.bmp swap.bmp systems.bmp extras.bmp os.bmp
	perl bitmap.pl pics bill.bmp 11 swap.bmp 13 systems.bmp 8 extras.bmp 4 os.bmp 11

install: mulg.prc
	pilot-xfer -i mulg.prc

pBill.prc: res.stamp obj.stamp
	$(BUILDPRC) pBill.prc $(ICONTEXT) $(APPID) *.pBill.grc *.bin

obj.stamp: pBill
	$(OBJRES) pBill
	touch obj.stamp

res.stamp: pBill.rcp pBill.h icon.bmp icon_sm.bmp title.bmp
	$(PILRC) pBill.rcp .
	touch res.stamp

pBill: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o pBill

clean:
	rm -rf *.[oa] pBill *.bin *.stamp *.grc *~ pics.h pics.cc
