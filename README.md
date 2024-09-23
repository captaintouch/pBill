# pBill: The Palm OS xBill port

This is a PalmOS port of the great and addictive xBill game for the X Windowing System.

Welcome to pBill...

# ORIGINAL PALM OS PORT
pBill v1.0 23rd Dec 1999

Rob Nielsen

rnielsen@cygnus.uwa.edu.au

http://www.cygnus.uwa.edu.au/~rnielsen/

# 2024 PALM OS VERSION
pBill v1.1 Sep 2024

Captain's Quarters

Mastodon: 
https://social.linux.pizza/@rxpz

Web:
https://ctrl-c.club/~captain/

## Changes made
- OS 5 support
- Game now runs on color palms as well
- Hi-Res graphics

# THE GAME
Ever get the feeling that nothing is going right? You're a sysadmin, and someone's trying to destroy your computers. The little people running around the screen are trying to infect your computers with Wingdows [TM], a virus cleverly designed to resemble a popular operating system. Additionally, some computers are connected with network cables. When one computer on a network becomes infected, a spark will be sent down the cable, and will infect the computer on the other end when it reaches there. 

The status bar at the bottom tells the following:
- Number of Bills on/off the screen
- Number of Computers running their OS/off/Wingdows
- Level
- Score

# PALMOS SPECIFICS
If you change to another application while you are playing, when you return the game will start paused at the start of the level you were on with the score you had at the start of the level.
Pressing the pause icon in the top right corner will pause the game.
Pressing the Menu key during a game will quit immediately.

# ORIGINAL AUTHORS
Main Programmer: Brian Wellington (bwelling@xbill.org)

Programming and graphics: Matias Duarte (matias@hyperimage.com)

http://www.xbill.org

# LICENSE
pBill is distributed under the GNU General Public Licence (http://www.gnu.org/copyleft/gpl.html) which means that you can freely make copies of this software and the source code is available on GitHub.

# SOURCE NOTES
pBill is a reasonably direct port of xBill 2.0 which was written in C++.
Most of the PalmOS specific code is in the pBill.cc file and the rest was left mostly intact, with the exception of UI.cc which was mostly gutted.
