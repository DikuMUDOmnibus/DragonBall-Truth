#3800
new trigger~
0 g 100
~
wait 1 sec
say Oh thank goodness.....
wait 2 sec
say I am near death...but I have a request for you....
wait 3 sec
say Dr. Wily has taken over the factory, and is making robots so he can take over the world. With megaman retired.... I hope you can take him out for me.
wait 3 sec
say Here.... take the key to the.... factory.... good luck.
wait 1 sec
drop key
wait 2 sec
mecho Dr. Light falls down, breathing his last as blood flows from his severed wrist arterey.
mgoto 10669
~
#3801
Protoman~
0 g 100
~
wait 2 sec
say So, you made it to me, eh?
wait 1 sec
say Well it is about time I got to have some fun!
wait 1 sec
smile
~
#3802
Protoman 2~
0 f 100
~
mload mob 3820
~
#3803
Wily's Ship~
0 n 100
~
wait 1 sec
mecho The floor begins to shake as cracks form along the walls.
wait 2 sec
mecho Suddenly a ship rips up through the floor, carrying you with it!
mteleport all 3889
wait 2 sec
mecho The hatch on the ship opens, and a short old man's head pops up out of it.
wait 1 sec
mecho Wily says "So, instead of that infernal Megaman I have you to deal with!? Let's go then...."
~
#3804
Wily's Drop~
0 f 100
~
mload mob 3821
~
#3805
Megabot~
0 n 100
~
if (%random.100% <= 10)
mload obj 3810
wait 3 sec
mecho Suddenly, a green pod shoots out of the remains of the ship.
wait 1 sec
mecho The green pod spins around faster and faster as the pieces of the ship are drawn to it.
wait 2 sec
mecho In it's place a gigantic robot floats. After a moment the robot drops down crashing to the roof.
wait 1 sec
say Now you will face the wrath of my greatest creation!!!
else
wait 3 sec
mecho Suddenly, a green pod shoots out of the remains of the ship.
wait 1 sec
mecho The green pod spins around faster and faster as the peices of the ship are drawn to it.
wait 2 sec
mecho In it's place a gigantic robot floats. After a moment the robot drops down crashing to the roof.
wait 1 sec
say Now you will face the wrath of my greatest creation!!!
~
#3806
Megabot Drop~
0 f 50
~
if (%random.100% <= 10)
mload obj 3811
else
mload obj 14201
~
#3807
Megabot Drop 2~
0 n 100
~
if (%random.100% == 5)
mload obj 3811
else
say You will be dieing now!
~
#3808
Teleporter~
0 n 100
~
mload mob 3822
~
#3809
Teleporter 2~
0 n 100
~
wait 1 sec
mecho As the Megabot hits the ground the roof begins to collapse
wait 3 sec
mecho The factory begins to shake as parts of the roof catch fire.
wait 6 sec
mecho The roof finally completely collapses, dropping you to the ground...
mteleport all 3800
mgoto 10669
mpurge light
mpurge self
~
#3810
Megabot drop 2~
0 f 50
~
if (%random.100% <= 10)
mload obj 3811
else
mload obj 14201
~
#3811
Dr. Light reset~
0 ab 10
~
say hahahahahaha someone gets my key now bitches!
mpurge self
~
#3812
Purged~
0 b 5
~
say You will not win!
~
#3891
Agni~
2 d 100
say i challege thee~
mload mob 14450
~
#3898
Kain's Soul Reaver~
1 l 100
~
wait 2 sec
oecho As Kain relinquishes his hold on the Soul Reaver the Hilt snakes it's way back to normal size as the screaming souls are once again silenced forever.
~
#3899
Kain's Soul Reaver~
1 j 100
~
wait 1 sec
oecho As Kain wields the Soul Reaver the handle snakes down wrapping it's hilt around his arm making them inseperable.
wait 2 sec
oecho The sound of souls screaming for their last will and judgement are heard as Kain forces the Soul Reaver to reap the souls it contains.
~
$~
