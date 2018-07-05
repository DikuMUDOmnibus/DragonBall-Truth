#16806
Silph (Path Side)~
0 jm 100
~
if (%amount% = 2500000)
wait 3 sec
mecho The Silph nods, her faceted silver face calm.
wait 2 sec
say Come Master, we will travel.
wait 2 sec
mecho The Silph scoops you up in one silver apendage, and plunges you into the silver liquid which makes up her very being.
wait 2 sec
mecho You float for a time, suspended without thought in pure, unaltered bliss, before re-emerging into the the world once more.
mteleport all 16804
wait 1 sec
mgoto 10173
~
#16808
Rishi Open~
0 j 100
~
(%object.name% == Proof Winds)
wait 1 sec
mecho Rishi nods his head, and tucks the book beneath his robes.
wait 2 sec
say Hold on, because here you go.
wait 1 sec
mecho The air around you seems to oscillate, and bend.
wait 2 sec
mecho In a flash of lightning, you stand inside of a massive building.
wait 1 sec
mteleport all 16811
wait 1 sec
mecho Another flash of lightning rips the air, and Rishi is gone.
wait 1 sec
mgoto 16810
~
#16810
Rishi Greet~
0 g 100
~
wait 2 sec
mecho The Legate Rishi looks at you.
wait 2 sec
say Hail and well met. Bring me the Proof of the Winds, and I shall grant you access to the Tagenricht ost fuer Mosst."
~
#16811
new trigger~
0 n 100
~
wait 1 sec
say Welcome.
wait 2 sec
say If you are here, then I am afraid that you will be tested.
~
#16812
loaded ann~
0 n 100
~
wait 1 sec
say Well done child. Verna has fallen, and now you fight me.
wait 2 sec
say Well, let's get on with it.
~
#16813
loaded nathan~
0 n 100
~
wait 1 sec
say So, Verna and Ann have failed.
wait 2 sec
say You will not find me as easy to defeat.
~
#16814
loaded zedd~
0 n 100
~
wait 1 sec
mecho Zeddicus' brow furrows.
wait 3 sec
say So you've defeated Verna, Nathan and Ann?
wait 2 sec
say Wel, now you face the "Wind of Death".
~
#16815
load ann~
0 f 100
~
wait 1 sec
say Very well. Ann shall defeat you.
wait 3 sec
mech The second door to the left opens, and an older woman, nearly eighty, enters.
wait 1 sec
mload mob 16812
~
#16816
load Nathan~
0 f 100
~
wait 1 sec
say So I have failed.
wait 2 sec
mecho Ann smiles.
wait 2 sec
say So be it. Nathan will not.
wait 1 sec
mecho The second door from the right opens, and a tall man with jaw length white hair enters.
wait 1 sec
mload mob 16813
~
#16817
load zedd~
0 g 100
~
wait 1 sec
say I fall, but Zedd will not.
wait 1 sec
mecho The farthest door to the right opens, and a skinny old man enters.
wait 1 sec
mload mob 16814
~
#16818
load richard~
0 f 100
~
wait 1 sec
say Bags.
wait 1 sec
say Nothing is ever easy.
wait 1 sec
mecho The center door opens, and a tall man, with brown hair, enters.
wait 1 sec
mload mob 16805
~
#16819
loaded richard~
0 n 100
~
wait 2 sec
mecho Richard surveys the area, a grim look upon his face.
wait 2 sec
say You've managed to survive fighting Zedd, Ann, Verna and Nathan.
wait 1 sec
say I am the last.
wait 1 sec
mload obj 16800
~
#16874
Bettle begins~
2 g 100
~
wait 2 sec
wecho The first door to the left opens, and woman of about thrity-eight or so enters.
wait 1 sec
wload mob 16811
~
$~
