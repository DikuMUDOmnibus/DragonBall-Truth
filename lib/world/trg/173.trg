#17300
Archmage~
0 g 100
~
wait 1 sec
say Alas, it is too late for you. You violated the sacred laws by entering this place.
wait 2 sec
say Pray to whatever gods hold your soul in this dimension, you are doomed.
~
#17301
Smith~
0 g 100
~
wait 1 sec
say Greetings...
wait 2 sec
say I can infused a weapon with ether, if you give me it first, and then pay me for my services...
wait 3 sec
say I only have enough ether left to infuse a small weapon though.
wait 2 sec
say Like a sai. Bring me a sai, and I will infuse it. After you give me the sai, pay me 5 million zenni.
wait 2 sec
say Pay me only after you give me the sai....
~
#17302
Smith 2~
0 j 100
~
if (%object.name% == Platinum Sai)
wait 2 sec
say Alright, now pay me the 5 million zenni.
mload mob 17305
mgoto 13000
else
wait 1 sec
say No you fool. I do not have enough ether to infuse this...
wait 1 sec
drop %object.name%
~
#17303
Smith money drop~
0 m 100
~
if (%amount% >= 1)
wait 1 sec
say I said the ore first you dumbass!
wait 1 sec
drop %amount% zenni
~
#17304
Smith 3~
0 m 100
~
if (%amount% == 5000000)
wait 1 sec
say Alright, I will begin.
wait 2 sec
mecho The smith takes the Platinum Sai and puts it into the ether oven.
wait 3 sec
wait 2 sec
mecho Bright flashing lights can be seen through the cracks of the Ether Oven as the weapon is infused.
wait 3 sec
mecho The smith slowly pulls out the Platinum Sai and begins to hammer it. The sai's surfaces glow with an aqua green light.
wait 2 sec
mecho The smith finishes up quickly.
wait 2 sec
say Here you go.
wait 1 sec
mload obj 17302
drop ether
mload mob 17304
mgoto 13000
mpurge Ethera
mpurge self
else
wait 1 sec
say This was NOT the required amount. I need 5mil zenni from you...
wait 1 sec
drop %amount% zenni
~
#17306
Guardian~
0 g 100
~
wait 1 sec
say Malakar Vesthmos, Blastarius.
wait 2 sec
say You shall not pass....
~
$~
