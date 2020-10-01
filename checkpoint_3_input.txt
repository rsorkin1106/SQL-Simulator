# Checkpoint file 3: # (comment), CREATE, INSERT, PRINT ... ALL, REMOVE, QUIT
CREATE 281class 3 string string bool emotion person Y/N
CREATE cities 5 string string int double bool name state population area is_capital?
CREATE pets 3 string bool bool name likes_cats? likes_dogs?
PRINT FROM 281class 3 emotion person Y/N ALL
PRINT FROM pets 1 likes_cats? ALL
INSERT INTO 281class 3 ROWS
stressed Paoletti true
happy Darden true
sad Angstadt false
PRINT FROM cities 3 is_capital? name population ALL
PRINT FROM 281class 2 emotion person ALL
INSERT INTO cities 5 ROWS
San_Francisco California 864816 46.87 false
Ann_Arbor Michigan 120782 28.69 false
Miami Florida 453579 55.25 false
Albany New_York 98111 21.93 true
Chicago Illinois 2705000 234 false
PRINT FROM 281class 1 person ALL
INSERT INTO pets 3 ROWS
Paoletti true false
Darden false true
Angstadt true true
REMOVE 281class
PRINT FROM cities 3 population is_capital? name ALL
REMOVE cities
PRINT FROM pets 2 name likes_dogs? ALL
REMOVE pets
QUIT
