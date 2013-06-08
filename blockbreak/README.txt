This is the BlockBreak breakout clone by Snoolas. It is done in C++ with SDL and
SDL_ttf. It is available at www.gpwiki.org. This is the Linux distribution. At
the time of writing there were no Windows binaries, but on the off chance that
they are added, check out the C++/SDL area of www.gpwiki.org for any new stuff.

This is version 1.01, fixing a huge memory leak at the game over screen.

The majority of the game's code is taken from Paddles, my pong clone. It shares
a lot of Paddles's quirks and also some of its bugs. One of the "features" is
the strange paddle movement with the acceleration and friction and such. This
makes it extremely hard sometimes to move accurately enough to catch a falling
ball, and on top of that, the balls can get going pretty fast sometimes, so the
end result is pretty frequent ball losses. As such, I have made it give you tons
and tons of balls. You seriously need them. Ahem... 40 to start out with and
then 10 after each level. 

It has a couple of known bugs/quirks:

One is when the ball gets going pretty fast,
sometimes it will just "jump over" either you paddle or a block. I know exactly
what causes this, but I don't know of a good way to fix it. The problem is
caused by a velocity higher than the width (or height) of whatever object it
jumps over. Sometimes the velocity will carry it right over the object, without a
chance to be picked up by the collision detection. 

Another bug happens with the collision detection of the blocks. There isn't an
easy way to detect from what side of the block the ball collided into. So I made
it always bounce it by the Y axis, since that is what is most often proper. This
means that if the Y velocity of the ball is low enough and the ball enters a
block from the side, the ball will simply plow through the entire row of blocks
until it reaches the side. 

There's also a bit of a quirk in that at a certain point in the levels (I haven't
reached it yet) the game will quit unexpectedly and spit out "MAPINDEX isn't high
enough" onto the command line. This problem is easily solved by changing the
definition of MAPINDEX at the top of main.cpp to a higher number and recompiling.
I just made the array of blocks as large as the MAPINDEX constant to save memory.

To compile blockbreak, just get onto the command line (*nix.) in the same
directory and say "sh compile.sh" and it will compile. You'll need SDL, 
SDL_image, and SDL_ttf and all of their dependencies for it to work. 

If you have any problems or further questions, ask them on the talk page for
BlockBreak on www.gpwiki.org or in the GPWiki forums.
