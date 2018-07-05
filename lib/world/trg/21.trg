#2100
Glug's greeting~
0 g 100
~
wait 2 sec
say Who would you be?
wait 2 sec
say Just someone come into my volcano, looking for my head to take back to your bounty dogs?
wait 2 sec
say Well you have made the biggest mistake of your life... It is too bad you won't realise this until just before the end...
~
#2101
Flarn's greeting~
0 g 100
~
wait 1 sec
say Hello and welcome to the Grand Cicle City Hotel. We advise you to be careful in your outings on Subzero, there are not very many places like this on the planet.
wait 3 sec
say God I hate my job....
wait 2 sec
say I mean, come on. This blows.... Everyone else is out having fun, killing things. What do I get to do? Greet half wits that stay here....
wait 1 sec
say Oh but that is just between you and me, got that?
wait 1 sec
mecho Flarn brandishes a titanium club from under the counter.
wait 1 sec
wink
~
#2102
Flarn's greet~
0 g 100
~
wait 1 sec
say Hello and welcome to the Grand Cicle City Hotel. We advise you to be careful in your outings on Frigid, there are not very many places like this on the planet.
wait 4 sec
say God I hate my job....
wait 3 sec
say I mean, come on. This blows.... Everyone else is out having fun, killing shit.. What do I get to do? Greet half wits that stay here....
wait 2 sec
say Oh but that is just between you and me, got that?
wait 2 sec
mecho Flarn brandishes a titanium club from under the counter.
wait 1 sec
wink
~
#2103
cold check~
0 j 100
~
if (%object.name% == Glug's Saber)
wait 1 sec
say Ah, so you managed to kill Glug, eh?
wait 1 sec
say Well let's see now. A deal is a deal, isn't it....
wait 1 sec
say Here you are, the armor as I promised....
wait 1 sec
mload obj 2145
give armor %actor.name%
mpurge saber
else if (%object.name% == Glog's Plasma Light)
wait 2 sec
say WOW! You managed to kill Glog!?
wait 2 sec
say Well, alright. Take this....
wait 1 sec
mload obj 2241
give vulcan %actor.name%
mpurge plasma
~
#2104
deathball~
0 k 8
~
death2
~
#2105
Slave trigger~
0 b 20
~
wait 1 sec
say ....I will escape....you bastards....
wait 10 sec
sigh
~
#2106
female slave~
0 b 20
~
wait 1 sec
say Don't touch me!
wait 3 sec
growl
wait 5 sec
mecho Saiyan Female Slave flicks her tail irritably.
~
#2107
Garn's trigger~
0 g 100
~
wait 1 sec
say Hey, You look like a decently strong person.... Come here for a second.
wait 2 sec
say How would you like to make a deal?
wait 3 sec
say All you need to do, is bring me the saber of Glug. Bring me it to prove he is dead. So I can collect my fee.
wait 2 sec
say I am too busy on another job to take care of it myself. Bring me the saber, and I will give you my armor. I feel close to retireing anyway.
wait 1 sec
say Oh, and if you find his brother, try killing him too.
wait 2 sec
say Bring me his light to prove he is dead, I will give you something for it...
~
#2108
Holo greet~
0 g 100
~
wait 5
mecho The Holo Machine hums to life, a robotic voice sounding from its speakers.
wait 5
say Welcome to the Hologram Generation Device.  Please deposit 1,500 coins if you wish a combat practice hologram.
~
#2109
Holo Load~
0 m 1500
~
wait 3
Lights beams flash from the machine before merging together and forming a saiba.
mload mob 2109
olc
wait 5
mecho The Holo Saiba shrieks and leaps forward.
~
#2110
mobs!1!1!1!~
0 k 10
~
wait 1 sec
say Guards! Guards! I am being attacked!
wait 4
mload mob 2149
~
#2111
Slave Merchant~
0 g 100
~
wait 1 sec
say Hey there! Want a slave? I got plenty! Fresh from their respective planets!
wait 1 sec
say I have humans, or nameks. They will give you some entertainment......
~
#2112
Slave Merchant 2~
0 m 1900
~
if (%amount% == 1900)
wait 1 sec
say Ah a human eh? Coming right up!
mecho A Human slave trudges forward.
mload mob 2146
else if (%amount% == 2000)
wait 1 sec
say Ah a namek, eh? Here you are.
mload mob 2145
mecho A Namek slave trudges forward.
end
~
#2113
Ticket Taker~
0 j 100
~
if (%object.name% == Rampage Ticket)
wait 1 sec
say Ah, come to see the show? Well here you are...
mecho She takes the ticket, and rips the stub off.
wait 1 sec
mecho %actor.name% is allowed through the rotating doors.
mteleport %actor.name% 2533
mpurge rampage
~
#2114
Ticket Taker 2~
0 g 100
~
wait 1 sec
say Ah, if you want into the arena, you will need to get a ticket.
wait 2 sec
say They are probably sold out, but you might be able to get a hold of one some how.
wait 2 sec
say Bring me it if you do get one.
smile
~
#2115
Icer Pilot Greet~
0 g 100
~
wait 15
say Welcome to the Frigid Transport station. For 2,500 coins I will lead you to my transport ship, and take you wherever you wish to go.
~
#2116
Icer Pilot~
0 jm 2500
~
if (%amount% == 2500)
wait 3
mecho The Frigid Transport Pilot nods and taps a few numbers on the control panel, the room glows as the transportation device is activated.
mteleport all 2116
say Here we are, just go south to the ship and say <destination> and we will be there in no time.
mgoto 2115
~
#2118
Jiors~
0 g 100
~
wait 1 sec
say Attention cadet!
wait 2 sec
say Oh you aren't a cadet, at ease civilian.
wait 3 sec
say Hmm, would you be interested in getting something for me? I can't go get it myself, I have an Academy matter to attend to. It's the teeth of one of those dreaded Wererats that live in the sewer.
wait 4 sec
say I need them for a collection.
wait 5
mecho Jiors points to a case against the wall, full of teeth.
wait 2 sec
say Get them for me, and you can have this Sporting event ticket I confiscated from one of my cadets.
~
#2119
Jiors 2~
0 j 100
~
if (%object.name% == Wereratteeth)
wait 1 sec
say Ah you got them. You are pretty good to have gotten these.
wait 1 sec
mecho Jiors, carefully places the teeth in his collection case.
wait 2 sec
say Alright, well I did make a deal. And I am an icer of my word. Here you are civilian.
wait 1 sec
mload obj 2153
drop ticket
mpurge were
~
#2120
Ring Keeper 1~
0 g 100
~
wait 1 sec
say Here we have someone from the audience, with the guts to face our champion!
wait 2 sec
mecho The crowd cheers wildly.
wait 2 sec
say When you are ready, say 'bringiton' and the champion will come forth for you to challenge!
wait 2 sec
mecho The Ring Keeper looks towards the crowd.
wait 2 sec
say Are you ready for some action!?
wait 4
mecho The crowd cheers louder.
~
#2121
Ring Keeper 2~
0 d 100
bringiton~
wait 1 sec
say Here he is! Our champion! Yargth!
wait 1 sec
mecho Crowd goes wild as Yargth steps forward.
mload mob 2159
~
#2126
king cold greet~
0 g 100
~
wait 2 sec
say Ah, I see we have a visitor....
wait 4 sec
say So, are you here to pledge undying loyalty to me. Or to fill my dungeon with your rotting corpse?
wait 4 sec
say Hmmm you might be useful, how much power do you have?
wait 2 sec
say No matter. I have a job for you. The Royal labs west of Icetip Mountain has fallen into disarray....
wait 3 sec
say General Danton has apparently taken over the facility, and used several of the research projects on himself. I want you to kill that treacherous dog, and bring me his bleeding heart!
wait 2 sec
say Bring me his heart, and I will give you something.....rare.
wait 1 sec
say The Lab password is....Hell I forget, something to do with mountains, and snow.
~
#2130
Ice queen~
0 h 100
~
wait 1 sec
say Why hello there mortal......
wait 1 sec
mecho A cold breeze blows from the Ice Queen
wait 2 sec
say Is it your wish to die, that you invade my territory?
wait 3 sec
say Leave unless you wish to die. And you most certainly will die, mortal....
~
#2131
Lightsaber~
1 j 100
~
wait 1 sec
oecho The lightsaber hums, and glows brightly as it is turned on. It gives off a vibe of power and death in the same instant.
~
#2132
Lightsaber off~
1 l 100
~
wait 1 sec
oecho The lightsaber makes a whooshing sound as it is turned off and returned to it's owner's belt.
~
#2133
Smith Icor~
0 j 100
~
if (%object.name% == silver ore)
wait 2 sec
say Ah so you found some silver ore? Well let's see, give me the 1500000 and I will make you the silver bangles like promised.
mload mob 2186
mgoto 2748
else
wait 1 sec
say You are pretty stupid young warrior..... I said Silver Ore, not this trash!
wait 1 sec
drop %object.name%
~
#2134
Smith Icor greet~
0 g 100
~
wait 3 sec
say Ah, a visitor... I almost thought my old eyes were playing tricks on me.
wait 2 sec
say So, what can I do for you? Oh wait, you look perfect for one of my famous silver bangles.
wait 3 sec
say Now don't go thinking that silver can't make some good armor.... My silver can.
wait 5
chuckle
wait 2 sec
say Anyhow..... Bring me some silver ore, and then give me 1500000 zenni, and I will make you my best bangle yet.
wait 3 sec
say Now remember, bring me the ore first. Money without that prized ore is insulting to one of my profession. I would probably keep the money and laugh in your face because of it.
~
#2135
Smith Icor~
0 m 1500000
~
wait 1 sec
say Ah, well just a momment, I will get started....
wait 3 sec
mecho Icor takes the silver ore and begins heating it in his furnace.
wait 5 sec
mecho Icor hums a happy tune as he pulls the ore out, and begins to break the silver element out of the rest of the rock.
wait 5 sec
mecho Icor begins to heat the cleaned silver in the furnace again.
wait 3 sec
mecho Icor takes his tongs and pulls the silver out, and begins hammering it inbetween reheatings.
wait 4 sec
mecho Icor takes the finished silver bangle and cools it off in a tub of water.
wait 2 sec
say Here you are, my special silver bangles.
mload obj 2200
drop bangle
mload mob 2185
mgoto 2748
~
#2136
Royal Labs password door~
2 d 100
avalanche~
wait 1 sec
wecho The floor suddenly shifts underneath the entrancway, as metal panels slide open revealing a clear screen.
wait 2 sec
wecho The screen flickers different colors as a humming sound fills the area.
wait 1 sec
wecho A sudden bright flash comes from the screen as it teleports everything in the entrancway into the laboratory.
wteleport all 2766
~
#2137
Natasha~
0 g 100
~
wait 1 sec
say Oh thank god! Another living person. General Danton, the Administrator of this facility has gone crazy!
wait 3 sec
mecho Natasha Looks around nervously.
wait 1 sec
say He has turned loose all the biological experiments, and ordered the droids to kill anything living!
wait 2 sec
say I know he locked himself up in his office. I can't imagine what his goal in all of this is, but if you can get me the security rifle that command droid carries....
wait 1 sec
say I will help you access the main elevator to his office. It requires a password, I am the only one other than the General, who is left and knows this password.
wait 4 sec
say Hurry! I don't want to die in this awful place!
~
#2138
Natasha Trigger 2~
0 j 100
~
if (%object.name% == Security Rifle)
wait 1 sec
say Oh thank you so much! I think I can get out as long as I have a weapon.
wait 3 sec
say Now what was the password for the elevator?
wait 2 sec
say Oh yes, it was ascention.
wait 1 sec
say Good luck, don't worry about me.
wait 1 sec
north
mgoto 2748
junk rifle
wait 30 sec
mgoto 2777
~
#2139
Elevator trigger, Royal Labs~
2 d 100
ascention~
wait 1 sec
wecho The elevator begins to hum as it rises. 
wait 2 sec
wecho The elevator stops suddenly, and a chime sounds as the doors open.
wteleport all 2806
~
#2140
General Danton~
0 g 100
~
wait 1 sec
say Ah, a visitor....
wait 2 sec
say You wouldn't happen to have been sent by King Cold would you? Does he fear me that much?
wait 3 sec
say He should now. I am the strongest being alive thanks to the research this lab put together.
wait 3 sec
say It was such a boring job, watching over the scientists gathered here, and my meager salary. I suddenly had a stroke of genius.
wait 1 sec
say Why leave the research here to be squandered away strengthening that royal family!? When I can become stronger than anyone, and use it to suit MY needs!
wait 4 sec
say Not even the gods can stop me now....
stand
wait 2 sec
say Well come on assassin. Try and kill me, if you value your life so little!
~
#2141
King Cold scouter~
0 j 100
~
if (%object.name% == Danton's Heart)
wait 2 sec
say So you managed to kill him..... Good good.
wait 3 sec
say Well here you go.
mload obj 2204
drop scouter
wait 1 sec
say Now leave
wait 1 sec
smile
wait 1 sec
mecho Cold eats the heart. After a momment energy crackles about his body as his power increases.
mgoto 2748
drop heart
mgoto 2335
~
#2142
Froz Canyon Geysers~
2 b 25
~
wecho A geyser suddenly spews boiling hot water twenty feet into the air.
wait 6 sec
wecho The geyser's water slowly drops back down as the pressure has been relieved.
~
#2143
Captin Geist~
0 g 100
~
wait 1 sec
say ....Is it life I sense?
wait 3 sec
say Yes..... Hello there mortal......
wait 2 sec
say It has been so long since one of your type has wandered into this ship.
wait 3 sec
say Even longer since one made it to meeee......
wait 1 sec
mecho A chilling breeze blows around the bridge.
wait 4 sec
say The gods will not pull us from our ship..... Your life force shall strengthen me for even longer...
wait 2 sec
say Now, give me your life!
stand
~
#2144
Phantom Shopkeeper~
0 g 100
~
wait 1 sec
say Greetings mortal!
wait 2 sec
mecho Phantom Shopkeeper rubs it's hands together while grinning.
wait 1 sec
say So, what do you wish to buy? Nothing matters more than money.
~
#2145
George the Sewage Guy~
0 g 100
~
wait 1 sec
say Oh hello, I rarely receive visitors.
wait 2 sec
say Hey! Whould you like to go on an adventure?
wait 3 sec
say Come on now, it will be easy. I get so bored here at my job, bring me a uh.....An umm, porno magazine, and I will give you the key to the sewers.
wait 2 sec
say That's a great deal, you never know what people might have thrown down there. I would go myself, but I am not allowed to keep anything I find down there.
~
#2146
George 2~
0 j 100
~
if (%object.name% == Porno Mag)
wait 1 sec
say Alright, cool!
wait 2 sec
say Now i have something to......do while I work.
wait 1 sec
mload obj 12921
drop key
wait 1 sec
say Now get out of here.
mpurge porno
~
#2147
Second Floor Guard~
0 g 100
~
wait 1 sec
say If you desire to pass on to the next floor, you must first best me.
wait 2 sec
say Only preserverance will carry one to the top.....
~
#2148
Wheeling Gull Master~
0 g 100
~
wait 1 sec
say Ah, you have done well to make it to me.....
wait 2 sec
say Now you can choose to train among the best here, or you can face me in an attempt to replace me.
wait 3 sec
say But do not be fooled, I will not be an easy target.
wait 1 sec
smile
~
#2160
Roshi greet~
0 g 100
~
wait 2 sec
say Well hello there....
wait 1 sec
say What can I do for you?
wait 3 sec
say Come to train!? Well I don't know about that.....
wait 2 sec
say Pay me 500 zenni, and I will let you train off of these saiba seed monsters Bulma cloned for me.
~
#2161
Roshi 2~
0 m 500
~
if (%amount% == 500)
wait 2 sec
say Here you are.
wait 1 sec
mload mob 12879
mecho Midget Saiba jumps out of a jar at you!
smile
~
$~
