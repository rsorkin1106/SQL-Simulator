# Checkpoint file 2: # (comment), CREATE, INSERT, PRINT ... ALL, REMOVE, QUIT
CREATE cities 5 string string int double bool name state population area is_capital?
INSERT INTO cities 7 ROWS
Ann_Arbor Michigan 120782 28.69 false
Lansing Michigan 116020 36.68 true
Miami Florida 453579 55.25 false
San_Francisco California 864816 46.87 false
Albany New_York 98111 21.93 true
Chicago Illinois 2705000 234 false
St_Louis Missouri 315685 65.98 false
PRINT FROM cities 2 name state ALL
REMOVE cities
QUIT
