#5000
Airplane Trigger 1~
0 g 100
~
wait 1 sec
mecho The airplane's ramp slowly drops down.
wait 2 sec
mecho A small speaker pops out of the airplane's side.
wait 1 sec
say If you would like to travel to otherworld, please say fly.
~
#5001
Airplane Trigger 2~
0 d 100
fly~
wait 1 sec
mecho The airplane's ramp slowly retracts.
wait 2 sec
mecho the airplane begins to take off from the runway.
wait 1 sec
mecho The airplane flys faster than any normal airplane ever could, closing in on Otherworld.
mteleport all 5003
wait 1 sec
mecho The airplane slowly comes in for a landing on Otherworld.
wait 1 sec
mecho The airplane's ramp opens, letting it's passengers off. Shortly after the airplane takes back off towards King Yema's check in.
mgoto 5002
~
#5002
Otherworld Announcer Trigger~
0 g 100
~
wait 1 sec
say Here comes the next fighter!
wait 2 sec
mecho The crowd cheers wildly, the deafening roar filling the entire stadium.
wait 2 sec
say What is your name warrior, you don't seem to be on the roster?
wait 3 sec
say Well nevermind that, one of the North warriors got sick, so you can fill in, I won't tell.
wait 2 sec
say Alright people! The final match of round 3 is about to begin!
wait 1 sec
say When you are ready, say ready. And we can start the final match, ok?
~
#5003
Information Attendant~
0 g 100
~
wait 1 sec
say Welcome to Otherworld!
wait 2 sec
say Otherworld has many famous structures, built by the gods to add beauty to otherworld.
wait 1 sec
say It is also home of the famous Otherworld Tournament held every once in a while by the great Grand Kai.
wait 3 sec
say Make sure not to get lost on Otherworld, as we are not responsible for your well being while here.
wait 2 sec
say Oh if you happen to find a clipboard laying around anywhere, please return it to me. I don't want to be fired.....
~
#5004
Otherworld Announcer Trigger 2~
0 d 100
ready~
wait 1 sec
say Alright! Let the final round begin!
wait 1 sec
mecho The crowd cheers louder, much of the spectators stand up to cheer also.
wait 2 sec
mecho Galnos slowly walks up to the platform. He hops up after only a momment of crowd pleasing.
wait 3 sec
mecho Galnos stops in the center of the platform.
wait 1 sec
say Alright, you two know the rules, no touching the outside of the ring.
wait 1 sec
say New warrior, take it easy on Galnos, this is his first time to the final round, since Pikkon isn't here to get in his way.
wait 2 sec
mgoto 10669
mload mob 5016
wait 1 sec
give belt galnos
wait 1 sec
mteleport galnos 5065
mgoto 5065
say Fight!
mgoto 10669
wait 10 sec
say 10
wait 10 sec
say 9
wait 10 sec
say 8
wait 10 sec
say 7
wait 10 sec
say 6 5 4 3 2 1! let's go!
wait 10 sec
mpurge self
~
#5005
Grand Kai~
0 g 100
~
wait 1 sec
say Get out of my view, man....
wait 2 sec
say This year's tournment sucks pretty hard, man...
wait 3 sec
say I just hope that loser Galnos doesn't win, what an embarassment that would be for the tournament.
~
#5006
Janitor~
0 g 100
~
wait 1 sec
say Aww damn, they fucking lost again....
wait 3 sec
mecho Gog turns his head towards the door.
wait 2 sec
say Who are you?
wait 2 sec
say Oh wait, you are the assistant for that annoying information dude aren't you?
wait 3 sec
say Well, if he wants his damn clipboard back he should give me a mushroom pizza. I am tired of him leaving it in the mansion on those damn tours he gives.
~
#5007
Janitor 2~
0 j 100
~
if (%object.name% == Mushroom Pizza)
wait 1 sec
say Ah, thanks dude. I really was needing one of these before the next game.
wait 2 sec
say It is always more enjoyable with a pizza.
wait 1 sec
say Here you go, and tell that asshole not to leave it in here anymore.
drop clipboard
wait 1 sec
mecho Gog leaves for the bathroom.
mpurge pizza
mpurge self
~
#5008
Information Attendant~
0 j 100
~
if (%object.name% == Clipboard)
wait 1 sec
say Oh wow, you found it!
wait 2 sec
say You're a life saver pal! Here, take this in return, you can go watch the Otherworld tournament with this.
wait 1 sec
mload obj 5003
drop key
mpurge clip
~
#5009
Darkbringer~
0 h 100
~
wait 1 sec
say Ahhh another comes to die....
wait 1 sec
say The light will be your doom! Let me bring you to everlasting darkness!
wait 1 sec
hellspear
wait 3 sec
say Your freedom may be painful, for you...
~
#5010
Darkbringer 2~
0 k 20
~
hellspear
~
#5011
Asslah~
0 g 100
~
wait 1 sec
say Hello.... Mortal.
wait 2 sec
say I am sorry for you and your kind.... You will not survive to see the perfect universe.....
wait 3 sec
say Ah well, you may leave if you wish. I will get you all eventually.....
~
#5012
Asslah~
0 k 15
~
2health
~
#5013
Gatekeeper~
0 g 100
~
wait 1 sec
say Turn back, Shadowlas Temple is for only specialy selected and trained Konatsus.
wait 2 sec
say And even right now, we are not admiting anyone. We are at full capacity, so leave.
~
#5014
Prime~
0 g 100
~
wait 1 sec
say How did you get in here? The gate is supposed to be sealed.....
wait 2 sec
say Hmm well you can stay, you must be powerful to have gotten past the gatekeeper. But you are not permited into the training area.
wait 2 sec
say And nothing will change my mind on that....
~
#5020
Gohan~
0 g 100
~
wait 1 sec
say Hello.
wait 2 sec
say I hope we can be friends.
wait 2 sec
say You wouldn't dare be attacking this planet with me here, now would you?
~
#5021
Masenko~
0 k 20
~
masenko
~
#5040
bells~
0 d 100
bells~
wait 5 sec
mecho The band starts playing again.
wait 5 sec
say &16Make his fight on the hill in the early day! Constant &00&04c&12h&16i&15l&00&07l&16 deep inside! Sounding gun, on they run through the endless &16g&15r&00&07e&16y On the fight, for they're right, yes, by who's to say?
wait 5 sec
say &16For a &00&03h&11i&16l&15l&16 men would &00&01k&09i&16l&15l&16, why? They do not know!  Suffered wounds test there their &00&01p&09r&16i&15d&00&07e&16!
wait 5 sec
say &16Men of five, still alive through the raging &00&03g&11l&16o&15w&16! Gone &00&04i&12n&16s&15a&12n&00&04e&16 from the &00&01p&09a&16i&15n&16 that they surely know!
wait 5 sec
say &16Men of five, still alive through the raging &00&03g&11l&16o&15w&16! Gone &00&04i&12n&16s&15a&12n&00&04e&16 from the &00&01p&09a&16i&15n&16 that they surely know!
wait 5 sec
mecho James and Jason turn towards each other only a foot apart with the micraphone between  them as they each play at their best almost as if challanging each other.
wait 5 sec
say &16Take a look to the &00&06s&14k&15y&16 just before you die! It is the last time you will! B&15l&00&07a&16c&15k&16e&00&07n&15e&16d&16 roar massive roar fills the crumbling &00&06s&14k&15y&16!
wait 5 sec
say &16Shattered goal fills his &00&06s&14o&16u&15l&16 with a ruthless cry! Stranger now, are his eyes, to this mystery! He hears the silence so loud!!!
wait 5 sec
say &16Crack of &00&05d&13a&16w&15n&16, all is gone except the will to be! Now the will see what will be, blinded eyes to see!
wait 5 sec
say &16F&15o&16r &00&06w&14h&16o&15m &16t&15h&16e B&15e&00&07l&16l&16 tolls! T&15i&00&07m&16e marches on! &16F&15o&16r &00&06w&14h&16o&15m &16t&15h&16e B&15e&00&07l&16l&16 tolls!!
~
#5041
Harvester~
0 d 100
Harvester~
wait 5 sec
mecho The band starts playing again.
wait 5 sec
say &16My Life Suffocates! Planting &00&02S&10e&16e&15d&00&07s&16 of &00&01H&09a&16t&15e&16! I've &00&05L&13o&16v&15e&16d, Turned to &00&01H&09a&16t&15e&16! Trapped Far Beyond My &00&06F&14a&16t&15e&16!
wait 5 sec
say &16I Give! You Take! This Life That I &10F&00&06o&10r&00&06s&10a&00&06k&10e&16! Been Cheated of My Youth! You Turned this Lie to Truth!
wait 5 sec
say &00&01A&09n&16g&15e&00&07r &00&01M&09i&16s&15e&09r&00&01y&16 You'll Suffer unto me! &00&01H&09a&16r&15v&00&07e&15s&16t&09e&00&01r&16 o&15f&00&01 S&09o&16r&15r&09o&00&01w&16! &00&06L&14a&16n&15g&00&07u&16a&00&06g&14e&16 of the &00&06M&14a&15d &00&01H&09a&16r&15v&00&07e&15s&16t&09e&00&01r&16 o&15f&00&01 S&09o&16r&15r&09o&00&01w&16!
wait 5 sec
say &16Pure B&15l&00&07a&15c&16k, Looking Clear! My Work Is Done Soon Here! Try Getting Back to Me! Get Back Which Used to Be!
wait 5 sec
say &00&03D&11r&16i&15n&00&07k&16 up! Shoot in! Let the Beatings Begin! Distributor of &00&01P&09a&16i&15n&16, Your Loss Becomes My Gain!!
wait 5 sec
say &00&01A&09n&16g&15e&00&07r &00&01M&09i&16s&15e&09r&00&01y&16 You'll Suffer unto me! &00&01H&09a&16r&15v&00&07e&15s&16t&09e&00&01r&16 o&15f&00&01 S&09o&16r&15r&09o&00&01w&16! &00&06L&14a&16n&15g&00&07u&16a&00&06g&14e&16 of the &00&06M&14a&15d &00&01H&09a&16r&15v&00&07e&15s&16t&09e&00&01r&16 o&15f&00&01 S&09o&16r&15r&09o&00&01w&16!
wait 5 sec
mecho Kirk steps foward and comes out with a truly bad ass solo.
wait 5 sec
say &16All Have Said Their Prayers! Invade Their &00&01N&09i&16g&15h&00&07t&16m&15a&16r&09e&00&01s&16! See into My Eyes! You'll Find Where &00&01M&09u&16r&15d&09e&00&01r &16Lies! &00&06I&14n&16f&15a&00&07n&16t&00&07i&15c&16i&14d&00&06e&16!!!
wait 5 sec
say &00&01H&09a&16r&15v&00&07e&15s&16t&09e&00&01r&16 o&15f&00&01 S&09o&16r&15r&09o&00&01w&16! &00&06L&14a&16n&15g&00&07u&16a&00&06g&14e&16 of the &00&06M&14a&15d &00&01H&09a&16r&15v&00 &07e&15s&16t&09e&00&01r&16 o&15f&00&01 S&09o&16r&15r&09o&00&01w&16!
~
#5042
Battery~
0 d 100
Battery~
Nothing.
wait 5 sec
mecho The band starts playing again.
wait 5 sec
say &16Lashing out the action, returning the reaction, Weak are ripped and torn away! &00&04H&12y&16p&15n&00&07o&16t&00&07i&15z&16i&12n&00&04g&16 power, crushing all that cower! &00&04B&12a&16t&15t&00&07e&12r&00&04y&16 is here to stay!!
wait 5 sec
say &16Smashing through the boundaries, lunacy has found me! Cannot stop the &00&04B&12a&16t&15t&00&07e&12r&00&04y&16!!
wait 5 sec
say &00&04P&12o&16u&15n&00&07d&16i&12n&00&04g&16 out aggression turns into &00&04o&12b&16s&15e&00&07s&15s&16i&12o&00&04n&16! Cannot kill the &00&04B&12a&16t&15t&00&07e&12r&00&04y&16!! &16Cannot kill the family &00&04B&12a&16t&15t&00&07e&12r&00&04y&16 is found in me &00&04B&12a&16t&15t &00&07e&12r&00&04y&16!!!
wait 5 sec
say &16Crushing all &00&04d&12e&16c&15e&00&07i&15v&16e&12r&00&04s&16, mashing non-believers! Never ending &00&04p&12o&16t&15e&00&07n&12c&00&04y&16! Hungry violence seeker, feeding off the weaker, breeding on &00&04i&12n&16s&15a&00&07n&16i&12t&00&04y&16!!
wait 5 sec
say &16Smashing through the boundaries, lunacy has found me! Cannot stop the &00&04B&12a&16t&15t&00&07e&12r&00&04y&16!!
wait 5 sec
say &00&04P&12o&16u&15n&00&07d&16i&12n&00&04g&16 out aggression turns into &00&04o&12b&16s&15e&00&07s&15s&16i&12o&00&04n&16! Cannot kill the &00&04B&12a&16t&15t&00&07e&12r&00&04y&16!!
wait 5 sec
mecho Larz stands up and begins a solo which last several minutes.
wait 5 sec
say &16Circle of &00&04D&12e&16s&15t&00&07r&16u&00&07c&15t&16i&12o&00&04n&16, Hammer comes crushing! &00&03P&11o&16w&15e&00&07r&16h&15o&16u&11s&00&03e&16 of &00&06e&14n&16e&15r&14g&00&06y&16! Whipping up a fury, Dominating &00&04f&12l&16u&15r&12r&00&04y&16 we create the &00&04B&12a&16t&15t&00&07e&12r&00&04y&16!
wait 5 sec
say &16Smashing through the boundaries, lunacy has found me! Cannot stop the &00&04B&12a&16t&15t&00&07e&12r&00&04y&16!!
wait 5 sec
say &00&04P&12o&16u&15n&00&07d&16i&12n&00&04g&16 out aggression turns into &00&04o&12b&16s&15e&00&07s&15s&16i&12o&00&04n&16! Cannot kill the &00&04B&12a&16t&15t&00&07e&12r&00&04y&16!!
~
#5044
Metallica Greet~
0 g 100
~
wait 4 sec
say Hey looks like youv gotta pass... Well the songs were doing today are written on your pass... So just say what ya wanna hear!
~
$~
