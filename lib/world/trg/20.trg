#2000
Cookie~
1 g 100
~
wait 1
oecho The gods have noticed the theft of the key. 20 secs remain before it will disappear.
wait 10 sec
oecho 10 seconds remain.
wait 10 sec
opurge self
~
#2001
Guardian trigger 1~
0 d 100
liquidice~
wait 1
give flag %actor.name%
~
#2002
Guardian Trigger 2~
0 d 100
mountain~
wait 1
give flag %actor.name%
~
#2003
Guardian Trigger 3~
0 d 100
citrus~
wait 1
give flag %actor.name%
~
#2004
Guardian flag 4~
0 d 100
blaster~
wait 1
give flag %actor.name%
~
#2005
Guardian Flag 5~
0 d 100
thirst~
wait 1
give flag %actor.name%
~
#2006
Guardian Flag 6~
0 d 100
radical~
wait 1
give flag %actor.name%
~
#2007
Guardian Flag 7~
0 d 100
flavored~
wait 1
give flag %actor.name%
~
#2008
Guardian Flag 8~
0 d 100
liters~
wait 1
give flag %actor.name%
~
#2019
flux open~
0 d 0
quintessence~
wait 2
say I see...very well then.  Proceed quickly, I do not wish to leave the flux open.
unlock flux
open flux
wait 50
close flux
lock flux
~
#2020
hyperbolic greet~
0 g 100
~
close doorway
lock doorway
wait 5
mecho Kami respectfully nods his head at you, crossing his arms over his chest.
wait 5
say If you wish to train in the Hyperbolic Time Chamber, speak to me of infinity.
wait 2 sec
say But if you wish to train against beings of old in the room of Spirit and Time, say quintessence. Or forever hold your piece.
~
#2021
hyperbolic entrance~
0 d 100
infinity~
wait 3
nod
wait 3
unlock seal
open seal
~
#2022
Ethereal Soldier~
0 m 7000
~
wait 3
say Very well.  I call upon the powers of time to bring to this warrior a Soldier of the Ether!
wait 3
mload mob 2019
mecho The Ethereal Soldier wavers into existence, calmly setting itself into a battle stance.
wait 1
frown
~
#2023
infinity close~
2 g 100
~
wdoor 2020 up flags b
~
#2050
Craps Dealer~
0 g 100
~
wait 1 sec
say Step right up! Shoot 'em, win some money. Nice clean game of craps. Just enter "rolldice" to play.
~
#2051
Slot machine~
1 b 20
~
wait 1 sec
oecho The Slot machine flashes a message across its screen. "Enter 'bet' to play."
~
#2070
Final Masamune~
1 g 100
~
wait 1
oecho A blazing aura fills the surrounding area as a new owner claims the power of Final Masamune!
wait 4 sec
oecho The aura dies down as the power is absorbed back into the blade, ready to be used.
~
#2071
Sephiroth~
0 g 100
~
wait 2 sec
say Return to the planet...and tell it I am coming.
wait 3 sec
say You are not worthy of a painless release. Your ending will be painful....
~
#2072
Final Masamune 2~
1 j 100
~
wait 1 sec
oecho The Final Masamune hums with the sound of a thousand wailing souls as its power is brought to use.
wait 2 sec
oecho Terror creeps into your mind as you witness the power of the Final Masamune!
~
#2073
Final Masamune 3~
1 l 100
~
wait 1 sec
oecho The Final Masamune loses its deadly hum as it is put back to sleep.
~
#2074
Final Masamune 4~
1 h 100
~
wait 1 sec
oecho The Final Masamune stabs into the ground, sending a small quake throughout the area.
wait 2 sec
oecho The person who dropped this feels pretty stupid now...
~
#2075
Sephiroth Randomness~
0 b 15
~
eval line %random.7%
switch %line%
case 1
wait 1
mecho Sephiroth smiles as he places two fingers on his forehead and disappears!
mgoto soldier
break
case 2
wait 1
mecho Sephiroth smiles as he places two fingers on his forehead and disappears!
mgoto bass
break
case 3
wait 1
mecho Sephiroth smiles as he places two fingers on his forehead and disappears!
mgoto angel
break
case 4
wait 1
mecho Sephiroth smiles as he places two fingers on his forehead and disappears!
mgoto namek
break
case 5
wait 1
mecho Sephiroth smiles as he places two fingers on his forehead and disappears!
mgoto shark
break
case 6
wait 1
mecho Sephiroth smiles as he places two fingers on his forehead and disappears!
mgoto man
break
default
wait 1
mecho Sephiroth smiles as he places two fingers on his forehead and disappears!
mgoto woman
break
done
~
#2076
Sephiroth Heal~
0 b 15
~
wait 1
2health
2health
2health
2health
~
#2090
OFP~
0 g 100
~
wait 1 sec
say Hey, shrimp. You seen my team!? They were supposed to meet me to play....
wait 2 sec
say The game is kinda lonely without them.
wait 4 sec
say Nobody plays with poor Todo anymore...
wait 2 sec
say SUCK IT UP! ARGGGGGGG!
wait 1 sec
mecho Oozaru Football Player slams into a nearby building, tackling it.
wait 3 sec
say YEAH YEAH TAKE THAT BIATCH!
~
#2092
Gerardi~
0 k 10
~
wait 1 sec
say You will die my friend, oh yes you will...
wait 5 sec
say It is only a matter of time... all who face me die, you are no different.
~
$~
