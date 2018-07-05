#800
Dinnin~
0 g 100
~
wait 2 sec
mecho Dinnin's purple eyes look up into yours, seeming to stare deep into your soul.
wait 3 sec
say Well hello there. My name is Dinnin, and as you must have already been informed, I'm in charge here.
wait 2 sec
mecho The flash of a metal draws your eyes away from Dinnin's face, and you see him idly playing with a dagger.
wait 2 sec
says You have two choices. You can either do me a great favour, or attack me, and die on the floor here.
wait 3 sec
say Kadyuka, the leader of the Shaengarn. I want him dead. Do this for me, and I will reward you greatly.
~
#801
Kadyuka greet~
0 g 100
~
wait 2 sec
mecho Kadyuka's huge form turns as he hears you enter, hius cold eyes glaring at you. His two bodyguards grip the hilts of their swords firmly, glaring also.
wait 2 sec
say Ahhh, another assassin. You, wretch, have two choices. Attack me and die, or go to Cerebus and bring me Dinnin's head.
wait 3 sec
mecho Kadyuka turns away again, confident that if you are an assassin, you will die easily.
~
#803
Dinnin's recieve~
0 j 100
~
if (%object.name% == Kadyuka's Head)
wait 2 sec
say I see you have accomplished my task. Here, take this. I hope we can do buisness again.
wait 1 sec
mload obj 803
give ryo %actor.name%
mgoto 10669
drop head
mgoto 844
~
#804
Kadyuka's recieve~
0 j 100
~
if (%object.name% == Dinnin's Head)
wait 2 sec
mecho The head rolls across the floor to stop at Kadyuka's feet. He laughs as he picks it up
wait 1 sec
say So, the Otori dog is dead! Here, take this money and begone!
wait 1 sec
mload obj 803
drop ryo
mgoto 10669
drop head
mgoto 4281
~
$~
