#ifndef OBJECTS_H
#define OBJECTS_H

#include "Horde.h"
#include "Network.h"
#include "Library.h"
#include "Bucket.h"
#include "Spark.h"
#include "Game.h"
#include "UI.h"
#include "Scorelist.h"

typedef struct
{
  char playing;
	unsigned level;
	unsigned long score;
} SaveGameType;

extern Scorelist hscores;
extern Horde bill;
extern Network net;
extern Library OS;
extern Bucket bucket;
extern Spark spark;
extern Game game;
extern UI ui;
extern SaveGameType savegame;

#define NONE 0
#define CREATOR 'pBil'


#endif

