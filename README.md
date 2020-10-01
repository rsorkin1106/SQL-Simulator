# SQL-Simulator

This program uses maps, hashes, and BSTs to simulate a basic SQL query


## Usage

Using "make" from the makefile will compile puzzle

$ ./silly [--quiet] [--help]

--quiet - if picked, any print statements will not print the data accessed, but rather
only how much data was accessed.

--help - prints possible command line arguments

## Objective

The terminal will present you with a "%" which is the program's command line. Entering the
following commands with their respective parameters:

%CREATE \<tablename\> \<N\> \<coltype1\> \<coltype2\> ... \<coltypeN\> \<colname1\> \<colname2\> ... \<colnameN\>

Creates a new table with \<N\> columns. Each column contains data of type \<coltype\> and is accessed
with the name \<colname\>. Valid data types for coltype are {double, int, bool, string}. This table is initially empty.


%INSERT INTO \<tablename\> \<N\> ROWS

\<value11\> \<value12\> ... \<value1M\>

\<value21\> \<value22\> ... \<value2M\>

...

\<valueN1\> \<valueN2\> ... \<valueNM\>

Inserts N new rows (where N is greater than 0) into the table specified by \<tablename\>. Prints
the amount of rows added to which table and which row numbers.


%DELETE FROM \<tablename\> WHERE \<colname\> \<OP\> \<value\>

Deletes all rows from the table specified by \<tablename\> where the value of the entry in \<colname\>
satisfies the operation \<OP\> with the given value \<value\>. \<OP\> is strictly limited to the set { \<, \> , = }.
Prints the number of rows deleted from the table.


%GENERATE FOR \<tablename\> \<indextype\> INDEX ON \<colname\>

Directs the program to create an index of the type <indextype> on the column \<colname\> in the table
\<tablename\>, where \<indextype\> is strictly limited to the set {hash, bst}, denoting a hash table
index and a binary search tree index respectively. prints successful completion of index generation.


%PRINT FROM \<tablename\> \<N\> \<print_colname1\> \<print_colname2\> ... \<print_colnameN\>
[WHERE \<colname\> \<OP\> \<value\> | ALL ]

Directs the program to print the columns specified by \<print_colname1\>, \<print_colname2\>, ...
\<print_colnameN\> from some/all rows in \<tablename\>. If there is no condition (ALL), the matching columns
from all rows of the table are printed. If there is a condition (WHERE \<colname\> \<OP\> \<value\>), only rows,
whose \<colname\> value pass the condition, are printed.


%JOIN \<tablename1\> AND \<tablename2\> WHERE \<colname1\> = \<colname2\> AND PRINT \<N\>
\<print_colname1\> \<1|2\> \<print_colname2\> \<1|2\> ... \<print_colnameN\> \<1|2\>

Directs the program to print the the data in \<N\> columns, specified by <print_colname1>,
\<print_colname2\>, ... \<print_colnameN\>. The <print_colname>s will be the names of
columns in either the first table \<tablename1\> or the second table \<tablename2\>, as specified by the
\<1/2\> argument directly following each \<print_colnameN\>. Prints the names of the specified columns, followed by the values of each of the specified
columns in each row then a statement indicating how many rows were printed.


%REMOVE \<tablename\>

Removes the table specified by \<tablename\> and all associated data from the database, including any
created index.


%QUIT

Cleans up all internal data (i.e. no memory leaks) and exits the program.


## Example Input

(Checkpoint_2_input.txt)

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


## Example Output

% % New table cities with column(s) name state population area is_capital? created

% Added 7 rows to cities from position 0 to 6

% name state 

Ann_Arbor Michigan 

Lansing Michigan 

Miami Florida 

San_Francisco California 

Albany New_York 

Chicago Illinois 

St_Louis Missouri 

Printed 7 matching rows from cities

% Table cities deleted

% Thanks for being silly!
