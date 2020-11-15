#include "TableEntry.h"
#include <unordered_map>
#include <map>
#include <iostream>
#include <algorithm>
#include <vector>
#include <getopt.h>

using namespace std;

class SillyQL {


    struct Table
    {
        vector<vector<TableEntry>> entries;
        vector<EntryType> types;
        unordered_map<string, size_t> cols;
        unordered_map<TableEntry, vector<size_t>> hash;
        map<TableEntry, vector<size_t>> bst;
        string index = "", name;
    };
    bool quiet = false;

    unordered_map<string, Table> tables;

    class VecLess {
    public:
        VecLess(size_t t, const TableEntry &x)
            :col(t), p(x) {}
        bool operator() (const vector<TableEntry>& temp) const
        {
            return temp[col] < p;
        }
        bool operator() (TableEntry x) const
        {
            return x < p;
        }

    private:
        size_t col;
        TableEntry p;
    };

    class VecEqual {
    public:
        VecEqual(size_t t, const TableEntry& x)
            :col(t), p(x) {}
        bool operator() (const vector<TableEntry>& temp) const
        {
            return temp[col] == p;
        }
        bool operator() (TableEntry x) const
        {
            return x == p;
        }

    private:
        size_t col;
        TableEntry p;
    };

    class VecGreater {
    public:
        VecGreater(size_t t, const TableEntry& x)
            :col(t), p(x) {}
        bool operator() (const vector<TableEntry>& temp) const
        {
            return temp[col] > p;
        }
        bool operator() (TableEntry x) const
        {
            return x > p;
        }

    private:
        size_t col;
        TableEntry p;
    };

public:
    void getOptions(int argc, char** argv)
    {
        int option_index = 0, option = 0;

        struct option longOpts[] = { {"quiet", no_argument, nullptr, 'q' },
                                    {"help", no_argument, nullptr, 'h'},
                                    { nullptr, 0, nullptr, '\0' } };

        while ((option = getopt_long(argc, argv, "qh", longOpts, &option_index)) != -1) {
            switch (option) {
            case 'q':
                quiet = true;
                break;

            case 'h':
                cout << "Command line options: -q or -h";
                exit(0);

            default:
                exit(1);
            }
        }
    }

    void readCommands()
    {
        size_t count = 0;
        string trash, cmd;
        do
        {
            ++count;
            cout << "% ";
            cin >> cmd;
            switch (cmd[0])
            {
            case '#':
                getline(cin, trash);
                break;

            case 'Q':
                quit();
                return;

            case 'C':
                create();
                break;

            case 'I':
                insert();
                break;

            case 'R':
                remove();
                break;

            case 'P':
                print();
                break;

            case 'D':
                deleteRows();
                break;

            case 'J':
                join();
                break;

            case 'G':
                generate();
                break;

            default:
                cout << "Error: unrecognized command\n";
                getline(cin, trash);
            }
        } while (cmd != "QUIT");
    }

private:

    void create()
    {
        string name, temp, type;
        size_t num;
        cin >> name;
        if (tables.find(name) != tables.end())
        {
            cout << "Error: Cannot create already existing table " << name << "\n";
            getline(cin, name);
            return;
        }
        cin >> num;
        Table* table = &tables[name];
        table->name = name;
        table->types.reserve(num);
        //Adds column types
        for (size_t i = 0; i < num; ++i)
        {
            cin >> type;
            switch (type[0])
            {
            case 's':
                table->types.push_back(EntryType::String);
                break;

            case 'b':
                table->types.push_back(EntryType::Bool);
                break;

            case 'i':
                table->types.push_back(EntryType::Int);
                break;

            case 'd':
                table->types.push_back(EntryType::Double);
                break;
            }
        }
        cout << "New table " << name << " with column(s) ";
        //Adds column names
        for (size_t i = 0; i < num; ++i)
        {
            cin >> temp;
            table->cols[temp] = i;
            cout << temp << " ";
        }
        cout << "created\n";
        return;
    }

    void insert()
    {
        string name, trash, sVal;
        int iVal;
        double dVal;
        bool bVal;
        size_t start, num, numCols;
        cin >> trash >> name >> num;
        if (tables.find(name) == tables.end())
        {
            cout << "Error: " << name << " does not name a table in the database\n";
            getline(cin, trash);
            for (size_t i = 0; i < num; ++i)
                getline(cin, trash);
            return;
        }
        cin >> trash;
        Table* table = &tables[name];
        start = table->entries.size();
        table->entries.reserve(start + num);
        numCols = table->cols.size();
        for (size_t i = start; i < start + num; ++i)
        {
            vector<TableEntry> temp;
            temp.reserve(numCols);
            for (size_t j = 0; j < numCols; ++j)
            {
                switch (table->types[j])
                {
                case EntryType::Bool:
                    cin >> bVal;
                    temp.emplace_back(bVal);
                    break;

                case EntryType::Double:
                    cin >> dVal;
                    temp.emplace_back(dVal);
                    break;

                case EntryType::Int:
                    cin >> iVal;
                    temp.emplace_back(iVal);
                    break;

                case EntryType::String:
                    cin >> sVal;
                    temp.emplace_back(sVal);
                    break;
                }
            }
            if (!table->hash.empty())
                table->hash[temp[table->cols[table->index]]].push_back(i);
            else if (!table->bst.empty())
                table->bst[temp[table->cols[table->index]]].push_back(i);

            table->entries.push_back(temp);
        }
        cout << "Added " << num << " rows to " << name << " from position " << start << " to " << start + num - 1 << "\n";
        assert(table->hash.empty() || table->bst.empty());
    }

    void remove()
    {
        string name;
        cin >> name;
        if (tables.find(name) == tables.end())
        {
            cout << "Error: " << name << " does not name a table in the database\n";
            return;
        }
        tables.erase(name);
        cout << "Table " << name << " deleted\n";
    }

    void print()
    {
        string name, colName, command;
        size_t num;
        
        cin >> name >> name; //from <tablename>
        if (tables.find(name) == tables.end())
        {
            cout << "Error: " << name << " does not name a table in the database\n";
            getline(cin, name);
            return;
        }
        cin >> num;
        vector<size_t> indexes;
        vector<string> colNames;
        indexes.reserve(num);
        colNames.reserve(num);
        Table* table = &tables[name];
        for (size_t i = 0; i < table->cols.size(); ++i)
        {
            cin >> colName;
            auto it = table->cols.find(colName);
            if (it == table->cols.end())
            {
                cout << "Error: " << colName << " does not name a column in " << name << "\n";
                getline(cin, name);
                return;
            }
            colNames.push_back(colName);
            indexes.push_back(it->second);
            if (colNames.size() == num)
                break;
        }
        cin >> command;
        if (command == "ALL")
        {
            if (!quiet)
                printAll(indexes, colNames, table);

            cout << "Printed " << table->entries.size() << " matching rows from " << name << "\n";
        }
        else
            printWhere(indexes, colNames, table);
    }

    void printAll(const vector<size_t>& indexes, const vector<string> &colNames, Table* table)
    {
        //Prints colNames
        for (size_t i = 0; i < colNames.size(); ++i)
        {
            cout << colNames[i] << " ";
        }
        cout << "\n";

        //Prints rows
        for (size_t i = 0; i < table->entries.size(); ++i)
        {
            for (size_t j = 0; j < indexes.size(); ++j)
            {
                cout << table->entries[i][indexes[j]] << " ";
            }
            cout << "\n";
        }
    }

    void printWhere(const vector<size_t>& indexes, const vector<string> &colNames,Table* table)
    {
        string col;
        cin >> col;
        if (table->cols.find(col) == table->cols.end())
        {
            cout << "Error: " << col << " does not name a column in " << table->name << "\n";
            getline(cin, col);
            return;
        }

        if (!quiet)
        {
            //Prints colNames
            for (size_t i = 0; i < colNames.size(); ++i)
            {
                cout << colNames[i] << " ";
            }
            cout << "\n";
        }

        calcRows(table, indexes, col, true);
    }

    void deleteRows()
    {
        string trash, col, name;

        cin >> trash >> name;
        if (tables.find(name) == tables.end())
        {
            cout << "Error: " << name << " does not name a table in the database\n";
            getline(cin, name);
            return;
        }

        Table* table = &tables[name];
        cin >> trash >> col;
        if (table->cols.find(col) == table->cols.end())
        {
            cout << "Error: " << col << " does not name a column in " << name << "\n";
            getline(cin, col);
            return;
        }

        calcRows(table, col, false);
    }

    void join()
    {
        //Pair = {table, printCol}
        vector<pair<string, string>> cols;
        string name1, name2, col1, col2, trash, printCol;
        size_t num, printNum;

        cin >> name1;
        if (tables.find(name1) == tables.end())
        {
            cout << "Error: " << name1 << " does not name a table in the database\n";
            getline(cin, trash);
            return;
        }

        cin >> trash >> name2;
        if (tables.find(name2) == tables.end())
        {
            cout << "Error: " << name2 << " does not name a table in the database\n";
            getline(cin, trash);
            return;
        }

        cin >> trash >> col1;
        Table* table1 = &tables[name1];
        Table* table2 = &tables[name2];
        if (table1->cols.find(col1) == table1->cols.end())
        {
            cout << "Error: " << col1 << " does not name a column in " << name1 << "\n";
            getline(cin, trash);
            return;
        }

        cin >> trash >> col2;
        if (table2->cols.find(col2) == table2->cols.end())
        {
            cout << "Error: " << col2 << " does not name a column in " << name2 << "\n";
            getline(cin, trash);
            return;
        }

        cin >> trash >> trash >> num;
        cols.reserve(num);

        //Checks cols to make sure they exist
        for (size_t i = 0; i < num; ++i)
        {
            cin >> printCol >> printNum;
            if (checkCol(table1, table2, cols, printCol, printNum))
                return;
        }

        joinAlgo(table1, table2, cols, col1, col2);
    }

    void generate()
    {
        string trash, name, type, col;

        cin >> trash >> name;
        if (tables.find(name) == tables.end())
        {
            cout << "Error: " << name << " does not name a table in the database\n";
            getline(cin, trash);
            return;
        }

        Table* table = &tables[name];
        cin >> type >> trash >> trash >> col;
        if (table->cols.find(col) == table->cols.end())
        {
            cout << "Error: " << col << " does not name a column in " << name << "\n";
            getline(cin, trash);
            return;
        }

        if (type == "hash")
        {
            if (!table->bst.empty())
            {
                table->bst.clear();
                Hash(table, col);
            }
            else if (!table->hash.empty())
            {
                //If a hash for that column already exists do nothing
                if (table->index != col)
                {
                    Hash(table, col);
                }
            }
            //Neither exist
            else
                Hash(table, col);
        }
        else
        {
            if (!table->hash.empty())
            {
                table->hash.clear();
                BST(table, col);
            }
            else if (!table->bst.empty())
            {
                //If a bst for that column already exists do nothing
                if (table->index != col)
                {
                    BST(table, col);
                }
            }
            //Neither exist
            else
                BST(table, col);
        }
        cout << "Created " << type << " index for table " << name << " on column " << col << "\n";
        assert(table->bst.empty() || table->hash.empty());
    }

    void quit()
    {
        cout << "Thanks for being silly!\n";
    }

    void calcRows(Table* table, const vector<size_t>& indexes, const string& col, bool print)
    {
        string sVal;
        double dVal;
        int iVal;
        bool bVal;
        char op;
        cin >> op;
        switch (table->types[table->cols[col]])
        {
        case EntryType::Bool:
            cin >> bVal;
            split3(table, indexes, TableEntry(bVal), col, print, op);
            break;
        case EntryType::Double:
            cin >> dVal;
            split3(table, indexes, TableEntry(dVal), col, print, op);
            break;
        case EntryType::Int:
            cin >> iVal;
            split3(table, indexes, TableEntry(iVal), col, print, op);
            break;
        case EntryType::String:
            cin >> sVal;
            split3(table, indexes, TableEntry(sVal), col, print, op);
            break;
        }
    }

    //Remove does not need a vector
    void calcRows(Table* table, const string& col, bool print)
    {
        vector<size_t> temp(0);
        calcRows(table, temp, col, print);
    }

    //Splits for bool
    void split3(Table* table, const vector<size_t>& indexes, const TableEntry& temp, const string& col, bool print, char op)
    {
        auto column = table->cols.find(col);

        switch (op)
        {
        case '<':
        {
            if (print)
            {
                calcRowHelp(table, indexes, col, VecLess(column->second, temp));
            }
            else
                removeRow(table, VecLess(column->second, temp));
            break;
        }

        case '=':
        {
            if (print)
            {
                calcRowHelp(table, indexes, col, VecEqual(column->second, temp));
            }
            else
                removeRow(table, VecEqual(column->second, temp));
            break;
        }

        case '>':
            if (print)
            {
                calcRowHelp(table, indexes, col, VecGreater(column->second, temp));
            }
            else
                removeRow(table, VecGreater(column->second, temp));
            break;
        }
    }


    template<typename Pred>
    void calcRowHelp(Table* table, const vector<size_t>& indexes, const string& col, Pred predicate)
    {
        if (!table->bst.empty() && table->index == col)
        {
            size_t count = 0;
            for (auto it = table->bst.begin(); it != table->bst.end(); ++it)
            {
                if (predicate(it->first))
                {
                    count += it->second.size();
                    if (!quiet)
                    {
                        for (size_t i = 0; i < it->second.size(); ++i)
                        {
                            for (size_t j = 0; j < indexes.size(); ++j)
                            {
                                cout << table->entries[it->second[i]][indexes[j]] << " ";
                            }
                            cout << "\n";
                        }
                    }
                }
            }
            cout << "Printed " << count << " matching rows from " << table->name << "\n";
            return;
        }
        if (quiet)
        {
            cout << "Printed " << count_if(table->entries.begin(), table->entries.end(), predicate) << " matching rows from " << table->name << "\n";
            return;
        }
        size_t count = 0;
        size_t idx = table->cols[col];
        for (size_t i = 0; i < table->entries.size(); ++i)
        {
            if (predicate(table->entries[i][idx]))
            {
                ++count;
                for (size_t j = 0; j < indexes.size(); ++j)
                {
                    cout << table->entries[i][indexes[j]] << " ";
                }
                cout << "\n";
            }
        }
        cout << "Printed " << count << " matching rows from " << table->name << "\n";
    }

    template<typename Pred>
    void removeRow(Table* table, Pred predicate)
    {
        size_t initial = table->entries.size();
        auto start = table->entries.begin();

        auto end = remove_if(table->entries.begin(), table->entries.end(), predicate);

        table->entries.resize(end - start);
        size_t size = initial - table->entries.size();

        cout << "Deleted " << size << " rows from " << table->name << "\n";
        if (!table->hash.empty())
        {
            //No need to clear and re-generate if no rows are deleted
            if (size != 0)
            {
                table->hash.clear();
                Hash(table, table->index);
            }
        }
        if (!table->bst.empty())
        {
            //No need to clear and re-generate if no rows are deleted
            if (size != 0)
            {
                table->bst.clear();
                BST(table, table->index);
            }
        }
        assert(table->bst.empty() || table->hash.empty());
    }

    bool checkCol(Table* table1, Table* table2, vector<pair<string, string>> &cols, const string& printCol, size_t printNum)
    {
        if (printNum == 1)
        {
            if (table1->cols.find(printCol) == table1->cols.end())
            {
                string trash;
                cout << "Error: " << printCol << " does not name a column in " << table1->name << "\n";
                getline(cin, trash);
                return true;
            }
            cols.push_back({ table1->name, printCol });
        }
        else
        {
            if (table2->cols.find(printCol) == table2->cols.end())
            {
                string trash;
                cout << "Error: " << printCol << " does not name a column in " << table2->name << "\n";
                getline(cin, trash);
                return true;
            }
            cols.push_back({ table2->name, printCol });
        }
        return false;
    }

    //Pair = {table, printCol}
    void joinAlgo(Table* table1, Table* table2, const vector<pair<string, string>>& columns, const string &col1, const string &col2)
    {
        //Prints colNames
        if (!quiet)
        {
            for (size_t i = 0; i < columns.size(); ++i)
            {
                cout << columns[i].second << " ";
            }
            cout << "\n";
        }

        //Checks to see if either or both tables have an index
        if (table2->index == col2 && !table2->hash.empty())
        {
            joinBoth(table1, table2, table2->hash, columns, col1);
            return;
        }
        else if(table2->index != col2 || table2->hash.empty())
        {
            unordered_map<TableEntry, vector<size_t>> temp;
            tempHash(table2, col2, temp);
            joinBoth(table1, table2, temp, columns, col1);
            return;
        }
        
        size_t idx1 = table1->cols[col1], idx2 = table2->cols[col2], count = 0;
        for (size_t i = 0; i < table1->entries.size(); ++i)
        {
            for (size_t j = 0; j < table2->entries.size(); ++j)
            {
                if (table1->entries[i][idx1] == table2->entries[j][idx2])
                {
                    count++;
                    if (!quiet)
                    {
                        for (size_t k = 0; k < columns.size(); ++k)
                        {
                            if (columns[k].first == table1->name)
                                cout << table1->entries[i][table1->cols[columns[k].second]] << " ";
                            else
                                cout << table2->entries[j][table2->cols[columns[k].second]] << " ";
                        }
                        cout << "\n";
                    }
                }
            }
        }

        cout << "Printed " << count << " rows from joining " << table1->name << " to " << table2->name << "\n";
    }

    //Pair = {table, printCol}
    void joinBoth(Table* table1, Table* table2, const unordered_map<TableEntry, vector<size_t>>& map, const vector<pair<string, string>>& columns, const string &col1)
    {
        size_t count = 0, colIdx = table1->cols[col1];
        for (size_t i = 0; i < table1->entries.size(); ++i)
        {
            auto it = map.find(table1->entries[i][colIdx]);
            if (it != map.end())
            {
                count += it->second.size();
                if (!quiet)
                {
                    for (size_t j = 0; j < it->second.size(); ++j)
                    {
                        for (size_t k = 0; k < columns.size(); ++k)
                        {
                            if (columns[k].first == table1->name)
                                cout << table1->entries[i][table1->cols[columns[k].second]] << " ";
                            else
                                cout << table2->entries[it->second[j]][table2->cols[columns[k].second]] << " ";
                        }
                        cout << "\n";
                    }
                }
            }
        }
        cout << "Printed " << count << " rows from joining " << table1->name << " to " << table2->name << "\n";
    }

    void Hash(Table* table, const string& col)
    {
        table->hash.clear();
        table->index = col;
        size_t idx = table->cols[col];
        for (size_t i = 0; i < table->entries.size(); ++i)
        {
            table->hash[table->entries[i][idx]].push_back(i);
        }
    }

    void tempHash(Table* table, const string& col, unordered_map<TableEntry, vector<size_t>>& map)
    {
        size_t idx = table->cols[col];
        for (size_t i = 0; i < table->entries.size(); ++i)
            map[table->entries[i][idx]].push_back(i);
    }

    void BST(Table* table, const string& col)
    {
        table->bst.clear();
        table->index = col;
        size_t idx = table->cols[col];
        for (size_t i = 0; i < table->entries.size(); ++i)
        {
            table->bst[table->entries[i][idx]].push_back(i);
        }
    }
};
