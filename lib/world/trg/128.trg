#12800
Victim~
0 h 100
0~
wait 1 sec
say Help me please! They're going to KILL me!!!!
wait 1 sec
mecho Sacrificial victim writhes against here restraints.
wait 2 sec
mecho Sacrificial victim writhes against here restraints.
wait 4 sec
say PLEASE HELP ME!
~
#12801
High Priest~
0 h 100
~
wait 3 sec
say Infidel! You will die first!
wait 2 sec
say Die you infidel pig!
wait 3 sec
say Destruction....
wait 2 sec
say Omri, kamnai, valas, tupon, mylas!
wait 3 sec
say Chaos....
wait 2 sec
say Valnis, uprey, fotos, mylas!
wait 6 sec
say Death..........
wait 2 sec
kill virgin
~
#12805
Konos greet~
0 h 100
~
wait 2 sec
say Ah...hello there young man.....
wait 2 sec
say So, what can I do for you? I am not able to work as much as I used to, but I can make you a nice rare blade if you bring me some Jet ore.
wait 3 sec
say The blade I will make will cost 5000000 zenni, but give me the ore first. I don't want to see money till I have the materials to work with.
~
#12806
Ore receive~
0 j 100
~
if (%object.name% == Jet Ore)
wait 2 sec
say Ah, you managed to get some ore eh?
wait 1 sec
say Alright, give me the 5,000,000 promised and I will make the blade.
mload mob 12822
mgoto 13000
mpurge konos
else
wait 1 sec
say You fool, I wanted Jet Ore, not this worthless junk....
wait 1 sec
drop %object.name%
~
#12807
Ore work~
0 m 100
5000000~
if (%amount% == 5000000)
wait 1 sec
say Alright, here I go.
wait 2 sec
mecho Konos starts to break the ore down, clearing away impuities and dirt.
wait 3 sec
mecho Finishing that Konos takes the refined ore and begins to soften it in his furnace.
wait 2 sec
mecho Konos begins to hum as he takes the glowing ore out.
wait 2 sec
mecho Konos brings his hammer up and begins to beat the ore into a different shape.
wait 2 sec
say Ah, this is the life I enjoy. Make you a great blade I will. Cut through those fancy space metals....
wait 3 sec
mecho After several minutes konos finished softening the ore and begins hammering it into it's blade shape.
wait 3 sec
mecho Konos finishes shaping the blade and plunges it into water.
wait 2 sec
mecho After a few momments Konos sets up the blade in a vise and begins to sharpen it.
wait 2 sec
mecho After a few other preperations Konos brings the new sword forward.
wait 1 sec
say Here you go. My speciality.
mload obj 12850
drop jet
mload mob 12821
mgoto 13000
mpurge konos
~
#12808
Konos money drop~
0 m 100
~
if (%amount% >= 1)
wait 1 sec
say I said the ore first you dumbass!
wait 1 sec
drop %amount% zenni
~
#12810
Teryal~
0 g 100
~
wait 2 sec
say Ah, have you come to learn the way of the dragon's talon sword style?
wait 3 sec
say Well to bad. I am not accepting any more students.
wait 1 sec
say If you wish to fight me that would be alright though, I havn't had anyone that stupid or brave in so long.
~
#12811
Blue Ninja~
0 g 100
~
wait 1 sec
say Ah, a new trainee, eh?
wait 2 sec
say Well, I am your first opponent. Defeat me to gain access to the cave. Defeat me not and you will surely die.
~
#12812
Gate Keeper~
0 g 100
~
wait 1 sec
say The toll is 300 zenni to pass.
~
#12813
Gate keeper 2~
0 m 300
~
if (%amount% == 300)
wait 1 sec
say Here you are
unlock gates
open gates
else
say This is not the right amount.
drop %amount% zenni
~
#12814
Red Ninja~
0 g 100
~
wait 1 sec
say Hmmm, you look kinda weak to have made it here.
wait 2 sec
say Well, no problem. If you beat me, then you have conquered the cave. You may even get something special, if fate is with you.
~
#12815
Tarsius~
0 g 100
~
wait 1 sec
say Hmm, hello there. Do you happen to know the way to Tetsigen?
wait 3 sec
say Oh no matter, probably out of anything good too.
wait 2 sec
say See I am a picky merchant, it's how I became so successful. I only purchase and resell valuable and rare items.
wait 3 sec
say Nothing is rare enough for me anymore.
wait 3 sec
say But maybe.... If you bring me a Silver Bangle. I heard those are extremely rare, and I have never seen one.
wait 2 sec
say I will give you something in exchange, something equally rare.
~
#12816
Tarsius 2~
0 j 100
~
if (%object.name% == Silver Bangle)
wait 2 sec
say YOU FOUND ONE! Excellent.
wait 3 sec
say Ok here you go, now remember, this is just as rare so take care of it.
mload obj 12885
drop Rune
mpurge bangle
~
#12817
Ifrit~
0 g 100
~
wait 1 sec
say Who dares enter my lair....
wait 3 sec
say You will die, and the heat will devour your bones.
wait 2 sec
say It is futile to resist. I bring you the eternal flame, and you shall not stand against it.
~
#12818
Bahamut~
0 g 100
~
wait 2 sec
say Another one comes to challenge the king of the dragons?
wait 1 sec
growl
wait 3 sec
say I thought I left all foolish knaves such as yourself behind on the world I fled.
wait 2 sec
say No longer will I bow to the whims or commands of mere mortals.
wait 3 sec
say You will die....
~
#12819
Mr Popo greet~
0 g 100
~
wait 1 sec
say Greeting's warrior. My don't you look strong.
wait 3 sec
say If you would like to train, pay me 7000 zenni. I will then bring forth a warrior from another time.
~
#12820
Rudy 1~
0 g 100
~
wait 2 sec
say Why hello there. You look like a strong adventurer.
wait 3 sec
say Hey I am in need of some pelts, they fetch a good penny when sold to off worlders. How about you bring me some, I will be the middle man har har.
wait 2 sec
say I will give you.....5000 zenni for each.
~
#12821
Gero's entrance~
0 d 100
goku~
wait 2 sec
unlock door
open door
wait 10 sec
close door
lock door
~
#12822
Freddy's Trigger~
0 g 100
~
wait 2 sec
say HAHAHAHAHAHA! Now that's a face only a mother could love!
wait 1 sec
mecho Freddy swipes his claws through one of the movie goers, cutting their jugular vien.
wait 3 sec
mecho Freddy looks towards you, a grin coming to his nasty face.
wait 1 sec
say Well now! Let's see what you fear!
~
#12823
Ticket Clerk~
0 g 100
~
wait 1 sec
say Greetings. Go on through, all the tickets are free today. To celebrate the protection of earth by the great Iovan.
wait 3 sec
say Nothing better than a free movie, eh?
~
#12830
Narg trigger~
0 g 100
~
wait 2 sec
say Ah hello there.....
wait 1 sec
mecho Narg continues to beat upon a great axe he is forging.
wait 2 sec
say Oh you are still here.....
mecho Narg slowly puts down his hammer, and tiredly walks over to you.
wait 3 sec
say Hmm.... If you want something forged, you gotta bring me some ore.
wait 2 sec
say I have given up on regular ores. Only special ores will do now.
wait 2 sec
say Hmmm, bring me some Xombi ore, and 6000000 zenni. Give me the ore first too
wait 1 sec
say I won't accept money first. No respectable smith will.
~
#12831
Narg Trigger 2~
0 j 100
~
if (%object.name% == Xombi ore)
wait 2 sec
say Ah, you finally brought it.
wait 1 sec
mecho Narg puts the ore down on a table.
wait 1 sec
say Ok, wait here while I work on it. After I am done, you will pay me for it.
wait 2 sec
mecho Narg takes the ore and begins to purify it.
wait 1 sec
mecho Narg cleans the dirt and rock out from the metallic ore.
wait 2 sec
mecho After he has it clean, Narg takes it over to the furnace and inserts it into the fire.
wait 3 sec
mecho Narg taked the heated ore out of the fire, and begins to work on it.
wait 2 sec
mecho Narg begins beating the metal into a round shape.
wait 3 sec
mecho Narg finishes up rather quickly, showing how skilled he truly is.
wait 1 sec
say Ok, give me the money.....
mload obj 8365
~
#12832
Narg Trigger 2~
0 m 6000000
~
if (%amount% == 6000000)
wait 1 sec
drop crown
wait 1 sec
mpurge ore
~
#12840
Infusion Truffle~
0 g 100
~
wait 2 sec
say Ahhhh, it feels good to be back. These fields are just like when I was a small truffle boy.
wait 3 sec
sigh
wait 2 sec
say Oh hello. I didn't notice you...
wait 2 sec
say Oh. A damn outsider. I bet that's what you are.
wait 3 sec
say To many truffles are being born outside of Neo Nirvana anymore. That only increases the outsiders who come to visit.
wait 3 sec
say Spreading the word of this great place.....
spit
wait 3 sec
say Leave my sight, you are ruining this great place of my memories.
~
#12841
Infused Truffle 2~
0 k 10
~
giga
~
#12842
Lightsaber~
1 j 100
~
emote waits. 1 sec
oecho The lightsaber hums as it's blade flashes to full length from the button being pushed.
emote waits. 2 sec
oecho The lightsaber continues to hum as it is twirled around in a few warm up swings. The air sizzles from being burnded by the lightsaber.
~
#12850
Transdim Pilot Greet~
0 g 100
~
wait 15
say Welcome to the Trans-Dimensional Transport Station, your one way ticket straight outta Hell.  For 2,500 coins I will lead you to my transport ship and take you wherever you want.
~
#12851
Transdim Transport Pilot~
0 m 2500
~
wait 3
mecho You step upon the platform as the pilot presses a few numbers on the keypad.
wait 3 sec
mecho A blazing flame leaps up from the ground, engulfing you before quickly dying away, leaving you in a room with a large ship resting before a portal.
mteleport all 12826
wait 15
say Here we are, just go east to the ship and say destination and we will be there in no time.
mgoto 12825
~
#12890
Sparring dummy~
0 g 100
~
wait 1 sec
mecho A speaker in the dummy's chest crackles to life.
wait 1 sec
say Greetings warrior! If you would like to spar me, give me 2000 zenni!
~
#12891
Sparring dummy 2~
0 m 2000
~
if (%amount% == 2000)
wait 1 sec
mecho The sparring dummy whirs as it gets into a sparring posistion.
wait 1 sec
spar ready
wait 20 sec
say Hiyaa!
wait 20 sec
say Huzaaa! Hiyaaaa!
wait 20 sec
mecho A green light flashes on the dummy's chest.
wait 20 sec
mecho A orange light flashes on the dummy's chest.
wait 20 sec
mecho A red light flashes on the dummy's chest.
wait 20 sec
say Sparring ovvvveeeerrrrr......
spar stop
~
$~
