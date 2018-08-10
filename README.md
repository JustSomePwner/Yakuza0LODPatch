# Yakuza 0 ( Steam ) LOD Patch
`community does what Sega QA don't`
an external program that modifies the game Yakuza 0 on runtime to fix certain issues with the PC port

# Features
* does not alter any files on disk
* restarting the game will reset all changes
* fixes LOD of detailed NPC models on settings higher than "low"
* increases LOD distance by altering the LOD system

# Usage
simply start the game, and then run this patch
it will need to ran everytime you play
no permanent changes are done, and no traces are left by the program
once the program is done, it can be closed and the changes will remain in effect until the Yakuza 0 is closed

# Why this is needed
Yakuza 0 is a port of a Playstation 3 game with a very aggressive LOD system
pop in of higher detailed models is very common and distracts from the experience
considering this is a PC port with decent performance, coming out many years after the original release, there is no reason for this system to still be active
on top of that, the PC port introduced a bug where if your settings are above "low", the lowest detailed character models are forced, making the game appear much worse than it really is

# What this patch does
when running the program, it will break certain code on runtime ( changes are only within RAM, after closing the game these changes no longer exist ), to fix these problems
models will appear in higher detail at a distance, and the ingame settings will also work properly now

