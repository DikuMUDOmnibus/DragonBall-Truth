#16705
Slug Flee~
0 f 100
~
mload mob 16713
~
#16707
Slug echos~
0 n 100
~
if (%random.100% <= 5)
mload obj 16705
mload obj 16706
wait 1 sec
mecho Slug snarls and leaps into the air boarding his escape pod.
wait 2 sec
mecho The Escape Pod rises up and out of the main ship.
mgoto 10623
else
wait 1 sec
mecho Slug snarls and leaps into the air boarding his escape pod.
wait 2 sec
mecho The Escape Pod rises up and out of the main ship.
mgoto 10623
~
#16708
Slug drop 2~
0 n 100
~
Nothing.
~
#16709
Medamatchas spawning trig~
0 g 100
~
mpurge
wait 1
say Hey You! Aren't you lucky? You get to be the first victim in my conquest of this planet!
wait 2 sec
say Now i will destroy you!
mload mob 16714
mload mob 16714
mload mob 16714
~
#16710
Medamatcha 2~
2 g 100
~
Nothing.
~
#16713
makasappo~
0 k 10
~
wait 1 sec
scatter
~
#16717
load zedd~
0 f 100
~
wait 1 sec
say I fall, but Zedd will not.
wait 1 sec
mecho The farthest door to the right opens, and a skinny old man enters.
wait 1 sec
mload mob 16814
~
#16777
Corridor Entry~
2 d 100
Porunga~
wait 2 sec
wecho A moving sidewalk starts up and moves everything on it forwards into the next room.
wait 3 sec
wteleport all 16912
~
$~
