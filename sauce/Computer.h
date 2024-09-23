#ifndef COMPUTER_H
#define COMPUTER_H

class Computer {	/*structure for Computers*/
public:
	int type;		/*CPU type*/
	int os;			/*current OS*/
	int x, y;		/*location*/
	int busy;		/*is the computer being used?*/
	int setup(int i);
	void draw(Boolean fixed);
	int find_stray();
	int oncomputer (int locx, int locy);
	int compatible(int system);
	int determineOS();
	static const int TOASTER = 0;	/* computer 0 is a toaster */
	static const int PC = 6;	/* type>=PC means the computer is a PC*/
	static const int OFFSET = 2;	/* offset of screen from 0,0 */
	static const int BORDER = 20;	/* BORDER pixels free on all sides*/
};

#endif
