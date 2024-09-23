#include "objects.h"

void Monster::get_border() {
	int i=game.RAND(0,3);
	if (i%2==0) target_x=game.RAND(0, game.scrwidth-bill.width);
	else target_y=game.RAND(0, game.scrheight-bill.height);
	switch (i) {
		case 0: target_y=-bill.height-8; break;
		case 1: target_x=game.scrwidth+1; break;
		case 2: target_y=game.scrwidth+1; break;
		case 3: target_x=-bill.width-1; break;
	}
}

/*  Adds a bill to the in state*/
void Monster::enter(){
	state = IN;
	get_border();
	x = target_x; y=target_y;
	index = 0;
	cels = bill.lcels;
	cargo = OS.WINGDOWS;
	x_offset = -1;
	y_offset = -7;
	target_c = game.RAND(0, net.units-1);
	target_x = net.computers[target_c].x+net.width-XOFFSET;
	target_y = net.computers[target_c].y+YOFFSET;
	bill.on_screen++; bill.off_screen--;
}

/*  Moves bill toward his target - returns whether or not he moved */
int Monster::move (int mode) {
	int xdist = target_x - x;
	int ydist = target_y - y;
	int step = step_size(game.level);
	int dx, dy;
	int signx = xdist >= 0 ? 1 : -1;
	int signy = ydist >= 0 ? 1 : -1;
	xdist = game.ABS(xdist);
	ydist = game.ABS(ydist);
	if (!xdist && !ydist) return 0;
	else if (xdist<step && ydist<step) {
		x = target_x;
		y = target_y;
	}
	else {
		dx = (xdist*step*signx)/(xdist+ydist);
		dy = (ydist*step*signy)/(xdist+ydist);
		switch(mode) {
			case SLOW: break;
			case FAST: dx = 5*dx/4; dy = 5*dy/4; break;
		}
		x+=dx;
		y+=dy;
		if (dx<0)
			cels = bill.lcels;
		else if (dx>0)
			cels = bill.rcels;
	}
	return 1;
}

void Monster::draw() {
	switch (state) {
		case IN:
		case OUT:
		case DYING: draw_std(); break;
		case AT: draw_at(); break;
		case STRAY: draw_stray(); break;
		default: break;
	}
}

/*  Update Bill's position */
void Monster::update_in() {
	int moved = move(SLOW);
	if (!moved && (net.computers[target_c].os != OS.WINGDOWS)
		&& !(net.computers[target_c].busy))
	{
		net.computers[target_c].busy=1;
		cels = bill.acels;
		index=0;
		state = AT;
		return;
	}
	else if (!moved) {
		int i;
		do {
			i=game.RAND(0, net.units-1);
		} while (i == target_c);
		target_c = i;
		target_x = net.computers[target_c].x + net.width-XOFFSET;
		target_y = net.computers[target_c].y + YOFFSET;
	}
	index++;
	index%=bill.WCELS;
	y_offset+=(4*(index%2)-2);
}

/*  Update Bill standing at a computer  */
void Monster::update_at() {
	int sys;
	if (index==0 && net.computers[target_c].os == OS.OFF) {
		index=6;
		sys = net.computers[target_c].find_stray();
		if (sys<0) cargo = -1;
		else {
			cargo = bill.list[sys].cargo;
			bill.list[sys].state = OFF;
		}
	}
	else index++;
	if (index == 13) {
		y_offset = -7;
		x_offset = -1;
		get_border();
		index = 0;
		cels = bill.lcels;
		state = OUT;
		net.computers[target_c].busy=0;
		return;
	}
	y_offset = bill.height - OS.height;
	switch (index) {
	case 1 :
	case 2 : x -= 4; x_offset +=4; break;
	case 3 : x -= 5; x_offset +=5; break;
	case 4 : x += 2; x_offset -=2; break;
	case 5 : x += 1; x_offset -=1; break;
	case 6 :
		if (net.computers[target_c].os != OS.OFF) {
			net.base--; net.off++;
			cargo = net.computers[target_c].os;
		}
		else {
			x-=10; x_offset+=10;
		}
		net.computers[target_c].os = OS.OFF;
		y_offset = -7;
		x += 10;
		x_offset -=10;
		break;
	case 7 : sy = y_offset; sx = -1; break;
	case 8 : sy = -7; sx = -1; break;
	case 9 : sy = -4; sx = -4; x -= 4; x_offset +=4; break;
	case 10 : sy = 0; sx = -4; x -= 7; x_offset +=7; break;
	case 11 : sy = 0; sx = -4;
		net.computers[target_c].os = OS.WINGDOWS;
		net.off--; net.win++;
		break;
	case 12 : x += 6; x_offset -=6;
	}
}

/* Updates Bill fleeing with his ill gotten gain */
void Monster::update_out() {
	if (game.INTERSECT(x, y, bill.width, bill.height, 0, 0, game.scrwidth,
		game.scrheight))
	{
		move(FAST);
		index++;
		index%=bill.WCELS;
		y_offset+=(4*(index%2)-2);
	}
	else {
		state = OFF;
		bill.on_screen--; bill.off_screen++;
	}
}


/* Updates Bill who is dying */
void Monster::update_dying() {
	if (index < bill.DCELS-1){
		y_offset += (index*GRAVITY);
		index++;
	}
	else {
		y+=y_offset;
		if (cargo<0 || cargo == OS.WINGDOWS) state = OFF;
		else state = STRAY;
		bill.on_screen--;
	}
}

void Monster::update() {
	switch (state) {
		case IN: update_in(); break;
		case AT: update_at(); break;
		case OUT: update_out(); break;
		case DYING: update_dying(); break;
		default: break;
	}
}

int Monster::clicked(int locx, int locy) {
	return (locx>=x-2 && locx<=x+bill.width+4 && locy>=y-2 && locy<=y+bill.height+4);
}

int Monster::clickedstray(int locx, int locy) {
	return (locx>=x && locx<=x+OS.width && locy>=y && locy<=y+OS.height);
}

int Monster::step_size(unsigned int lev) {
	return game.MIN(7+lev/2, 9);
}

void Monster::draw_std() {
	if (cargo>=0)
		ui.draw(OS.os[cargo], x + x_offset, y + y_offset);
	ui.draw(cels[index], x, y);
}

void Monster::draw_at() {
	if (index>6 && index<12)
		ui.draw(OS.os[0], x + sx, y + sy);
	if (cargo>=0)
		ui.draw(OS.os[cargo], x + x_offset, y + y_offset);
	ui.draw(cels[index], net.computers[target_c].x,
		net.computers[target_c].y);
}

void Monster::draw_stray() {
	if (game.grabbed==-1 || x != bill.list[game.grabbed].x)
		ui.draw(OS.os[cargo], x, y);
}
