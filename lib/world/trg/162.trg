#16200
Necrids Soul Reaper Plushy Get Triggers~
1 g 100
~
wait 1 second
oecho "Many will suffer and die by my hand!"
wait 1 second
oecho "I feed on the fear and darkness in your heart."
~
#16201
Necrids Soul Reaper Plushy Hold Triggers~
1 j 100
~
wait 1 second
oecho "Souls of the Innocent...GIVE ME STRENGTH!"
wait 1 second
oecho "You are too weak and pathetic to remain living"
wait 1 second 
oecho "The exact moment remains hidden, but rest assured that I, the bringer of Death, will consume your very essence."
~
#16202
Necrids Soul Reaper Plushy Drop Trigger~
1 h 100
~
wait 1 second
oecho "You cannot handle my Power!"
wait 1 second
oecho "I guess you dont want any cookies then."
~
#16203
Necrids Soul Reaper Plushy Give Trigger~
1 i 100
~
wait 1 second
oecho "Excellent....A fresh soul to satisfy my hunger."
wait 1 second
oecho "Souls and Cookies....My two favorite things in life."
~
#16204
Necrids Soul Reaper Plushy Remove Trigger~
1 l 100
~
wait 1 second
oecho "Death...Hatred....Fear....All will consume you."
wait 1 second
oecho "You cannot handle my power!"
~
#16205
Astral Master trig 1~
0 g 100
~
wait 2 sec
mecho The Astral Fist Master slowly strokes his beard as he studies you.
wait 3 sec
say Welcome young warrior, come to challenge for full membership already have you?
wait 4 sec
say You wouldn't have happened to be someone who already passed trying to get a freebie would you?
wait 2 sec
say Oh nothing, nothing. Just wondering.
wait 5 sec
say Anyway. The rite of ascension is four battles against foes no longer of this universe. Beyond the power of the gods they have tied theirselves to this dojo, for its benefit.
wait 1 sec
cackle
wait 2 sec
say Anyway...
wait 3 sec
say The four rooms beyond this one each contain a magical statue, in which the astral beings you will face are tied to.
wait 5 sec
say Do not attempt to kill the statues... That will most certainly bring down the wrath of the great immortal Iovan.
wait 5 sec
say When you are ready, speak the word ascension. Do not be fooled into thinking you can bring help along, these are to be a test for a solo warrior. There is no way around it.
wait 1 sec
say And when you arrive, speak the same word to activate each statue.
~
#16206
Astral Master trig 2~
0 d 100
ascension~
wait 2 sec
say Alright, but remember. Failure means death....
wait 1 sec
mecho The Astral Fist master makes a couple quick gestures and you disappear in a wave of light.
mteleport all 16265
mgoto 16264
mpurge self
~
#16207
Astral Master trig 3~
0 j 100
~
if (%object.name% == Astral Scroll)
wait 1 sec
say Congratulations, you have managed to ascend into a full member of the dojo.
wait 3 sec
say This is proof of your membership. Treat it well...
wait 1 sec
mload obj 16219
mpurge scroll
give crest %actor.name% 
mpurge self
~
#16208
First Statue 1~
0 d 100
ascension~
wait 3 sec
mecho The statue slowly turns to face you as green energy covers its surface.
wait 2 sec
mecho Suddenly the green anergy leaves the statue and pools in the center of the room, forming an Astral Ghost.
mload mob 16207
mpurge self
~
#16209
Astral Ghost Death~
0 f 100
~
mload mob 16211
~
#16210
First Statue Load~
0 n 100
~
wait 2 sec
mecho The remains of the Astral Ghost's energy are absorbed back into the statue.
wait 1 sec
mecho You are transported to the next step in a flash of light.
mteleport all 16266
mgoto 16265
mpurge self
~
#16211
Second Statue~
0 d 100
ascension~
wait 2 sec
mecho The Green energy around the statue begins to pool into the center of the room, just like the last statue.
wait 2
mecho The final form of an Astral Spectre appears.
mload mob 16208
mpurge self
~
#16212
Spectre Death~
0 f 100
~
mload mob 16212
~
#16213
Second Statue load~
0 n 100
~
wait 2 sec
mecho The remaining energy from the Astral Spectre is absorbed back into the statue.
wait 1 sec
mecho You are transported to the next area in a beam of light.
mteleport all 16267
mgoto 16266
mpurge self
~
#16214
Third Statue~
0 d 100
ascension~
wait 2 sec
mecho The energy around the statue immeadiatly forms into an Astral Phantom, indicating a more agressive form.
mload mob 16209
mpurge self
~
#16215
Phantom Death~
0 f 100
~
mload mob 16213
~
#16216
Third Statue load~
0 n 100
~
wait 2 sec
mecho The energy from Astral Phantom is absorbed back into the statue, and you are teleported in a beam of light.
wait 1
mteleport all 16268
mgoto 16267
mpurge self
~
#16217
Final statue~
0 d 100
ascension~
wait 2 sec
mecho The energy around the Final Statue gathers into a swirling vortex on the ceiling.
wait 2 sec
mecho A shadowy form drops out of the vortex, landing in the middle of the room.
wait 3 sec
mecho Slamming its fist into the ground, the Astral Arbiter floods the room with a flash of light. Revealing its ghostly form.
mload mob 16210
mpurge self
~
#16218
Arbiter death~
0 f 100
~
mload mob 16214
~
#16219
Final statue load~
0 n 100
~
wait 2 sec
mecho The anergy from the Astral Arbiter is absorbed back into the statue, and the room begins to shake.
wait 3 sec
mecho A glowing scroll is spit from the statue's mouth. The scroll slowly hovers to you, and you grab it from the air.
wait 1
mload obj 16218
drop scroll
wait 3 sec
mecho The statue begins to gether energy to teleport you back, as chunks of wood from the ceiling begin to fall down.
wait 5 sec
mecho The statue nears the end of its energy gathering.
wait 3 sec
mecho You are teleported in a beam of light, back to the start.
mteleport all 16264
mgoto 16264
mload mob 16202
mgoto 16268
mpurge self
~
#16220
Fighter purge~
0 b 5
~
if (%random.100% == 10)
mecho The astral being is absorbed back into the statue, time having run out for the match.
mpurge self
else
mecho Some energy is shed off of the astral being...
~
#16221
Ghost attack~
0 k 20
~
baku
~
#16222
Spectre attack~
0 k 20
~
masenko
~
#16223
Phantom Attack~
0 k 20
~
maka
~
#16224
Arbiter~
0 k 2
~
hellsp
~
$~
