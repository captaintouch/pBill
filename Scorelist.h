#ifndef SCORELIST_H
#define SCORELIST_H
#include <Common.h>
#include <System/SysAll.h>
#include <UI/UIAll.h>

#define NUM_SCORES 4
class Scorelist {
	int changed;
public:
	unsigned levels[NUM_SCORES];
	unsigned long scores[NUM_SCORES];
	DateType dates[NUM_SCORES];

  void reset();
	void load();
	void save();
	int add();
	void draw(int i);
};

#endif
