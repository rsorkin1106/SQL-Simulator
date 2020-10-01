//Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
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
        string index = "";
    };
    bool quiet = false;

    unordered_map<string, Table> tables;

    class OpLess {
    public:
        OpLess(const TableEntry &val_in)
        : val(val_in) {}
        bool operator() (TableEntry x)
        {
            return x < val;
        }
    private:
        TableEntry val;
    };


    class OpEqual {
    public:
        OpEqual (const TableEntry& val_in)
            : val(val_in) {}
        bool operator() (TableEntry x)
        {
            return x == val;
        }
    private:
        TableEntry val;
    };

    class OpGreater {
    public:
        OpGreater(const TableEntry& val_in)
            : val(val_in) {}
        bool operator() (TableEntry x)
        {
            return x > val;
        }
    private:
        TableEntry val;
    };

    class VecLess {
    public:
        VecLess(size_t t, const TableEntry &x)
            :col(t), p(x) {}
        bool operator() (const vector<TableEntry>& temp)
        {
            return temp[col] < p;
        }

    private:
        size_t col;
        TableEntry p;
    };

    class VecEqual {
    public:
        VecEqual(size_t t, const TableEntry& x)
            :col(t), p(x) {}
        bool operator() (const vector<TableEntry>& temp)
        {
            return temp[col] == p;
        }

    private:
        size_t col;
        TableEntry p;
    };

    class VecGreater {
    public:
        VecGreater(size_t t, const TableEntry& x)
            :col(t), p(x) {}
        bool operator() (const vector<TableEntry>& temp)
        {
            return temp[col] > p;
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
        string trash, cmd;
        do
        {
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
        tables[name];
        tables[name].types.reserve(num);
        //Adds column types
        for (size_t i = 0; i < num; ++i)
        {
            cin >> type;
            if (type == "string")
                tables[name].types.push_back(EntryType::String);
            else if (type == "bool")
                tables[name].types.push_back(EntryType::Bool);
            else if (type == "int")
                tables[name].types.push_back(EntryType::Int);
            else
                tables[name].types.push_back(EntryType::Double);
        }
        cout << "New table " << name << " with column(s) ";
        //Adds column names
        for (size_t i = 0; i < num; ++i)
        {
            cin >> temp;
            tables[name].cols[temp] = i;
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
        cin >> trash >> name >> num >> trash;
        if (tables.find(name) == tables.end())
        {
            cout << "Error: " << name << " does not name a table in the database\n";
            getline(cin, trash);
            for (size_t i = 0; i < num; ++i)
                getline(cin, trash);
            return;
        }
        start = tables[name].entries.size();
        tables[name].entries.reserve(start + num);
        numCols = tables[name].cols.size();
        for (size_t i = start; i < start + num; ++i)
        {
            vector<TableEntry> temp;
            temp.reserve(numCols);
            for (size_t j = 0; j < numCols; ++j)
            {
                switch (tables[name].types[j])
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
            tables[name].entries.push_back(temp);
        }
        cout << "Added " << num << " rows to " << name << " from position " << start << " to " << start + num - 1 << "\n";
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
        for (size_t i = 0; i < tables[name].cols.size(); ++i)
        {
            cin >> colName;
            auto it = tables[name].cols.find(colName);
            if (it == tables[name].cols.end())
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
                printAll(indexes, colNames, name);

            cout << "Printed " << tables[name].entries.size() << " matching rows from " << name << "\n";
        }
        else
            printWhere(indexes, colNames, name);
    }

    void printAll(const vector<size_t>& indexes, const vector<string> &colNames, const string& name)
    {
        //Prints colNames
        for (size_t i = 0; i < colNames.size(); ++i)
        {
            cout << colNames[i] << " ";
        }
        cout << "\n";

        //Prints rows
        for (size_t i = 0; i < tables[name].entries.size(); ++i)
        {
            for (size_t j = 0; j < indexes.size(); ++j)
            {
                cout << tables[name].entries[i][indexes[j]] << " ";
            }
            cout << "\n";
        }
    }

    void printWhere(const vector<size_t>& indexes, const vector<string> &colNames, const string& name)
    {
        string col;
        cin >> col;
        if (tables[name].cols.find(col) == tables[name].cols.end())
        {
            cout << "Error: " << col << " does not name a column in " << name << "\n";
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

        calcRows(indexes, col, name, true);
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

        cin >> trash >> col;
        if (tables[name].cols.find(col) == tables[name].cols.end())
        {
            cout << "Error: " << col << " does not name a column in " << name << "\n";
            getline(cin, col);
            return;
        }

        calcRows(col, name, false);
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
        if (tables[name1].cols.find(col1) == tables[name1].cols.end())
        {
            cout << "Error: " << col1 << " does not name a column in " << name1 << "\n";
            getline(cin, trash);
        }

        cin >> trash >> col2;
        if (tables[name2].cols.find(col2) == tables[name2].cols.end())
        {
            cout << "Error: " << col2 << " does not name a column in " << name2 << "\n";
            getline(cin, trash);
        }

        cin >> trash >> trash >> num;
        cols.reserve(num);

        //Checks cols to make sure they exist
        for (size_t i = 0; i < num; ++i)
        {
            cin >> printCol >> printNum;
            if (checkCol(cols, printCol, name1, name2, printNum))
                return;
        }

        joinAlgo(cols, name1, name2, col1, col2);
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

        cin >> type >> trash >> trash >> col;
        if (tables[name].cols.find(col) == tables[name].cols.end())
        {
            cout << "Error: " << col << " does not name a column in " << name << "\n";
            getline(cin, trash);
        }

        if (type == "hash")
        {
            if (!tables[name].bst.empty())
            {
                tables[name].bst.clear();
                Hash(name, col);
            }
            else if (!tables[name].hash.empty())
            {
                if (tables[name].index != col)
                {
                    tables[name].hash.clear();
                    Hash(name, col);
                }
            }
            else
                Hash(name, col);
        }
        else
        {
            if (!tables[name].hash.empty())
            {
                tables[name].hash.clear();
                BST(name, col);
            }
            else if (!tables[name].bst.empty())
            {
                if (tables[name].index != col)
                {
                    tables[name].bst.clear();
                    BST(name, col);
                }
            }
            else
                BST(name, col);
        }
        cout << "Created " << type << " index for table " << name << " on column " << col << "\n";
    }

    void quit()
    {
        cout << "Thanks for being silly!\n";
    }

    void calcRows(const vector<size_t>& indexes, const string& col, const string& name, bool print)
    {
        string sVal;
        double dVal;
        int iVal;
        bool bVal;
        char op;
        cin >> op;
        switch (tables[name].types[tables[name].cols[col]])
        {
        case EntryType::Bool:
            cin >> bVal;
            split3(indexes, TableEntry(bVal), col, name, print, op);
            break;
        case EntryType::Double:
            cin >> dVal;
            split3(indexes, TableEntry(dVal), col, name, print, op);
            break;
        case EntryType::Int:
            cin >> iVal;
            split3(indexes, TableEntry(iVal), col, name, print, op);
            break;
        case EntryType::String:
            cin >> sVal;
            split3(indexes, TableEntry(sVal), col, name, print, op);
            break;
        }
    }

    //Remove does not need a vector
    void calcRows(const string& col, const string& name, bool print)
    {
        vector<size_t> temp;
        calcRows(temp, col, name, print);
    }

    //Splits for bool
    void split3(const vector<size_t>& indexes, const TableEntry& temp, const string& col, const string& name, bool print, char op)
    {

        switch (op)
        {
        case '<':
        {
            if (print)
                calcRowHelp(indexes, col, name, OpLess(temp));
            else
                removeRow(name, VecLess(tables[name].cols[col], temp));
            break;
        }

        case '=':
        {
            if (print)
                calcRowHelp(indexes, col, name, OpEqual(temp));
            else
                removeRow(name, VecEqual(tables[name].cols[col], temp));
            break;
        }

        case '>':
            if (print)
                calcRowHelp(indexes, col, name, OpGreater(temp));
            else
                removeRow(name, VecGreater(tables[name].cols[col], temp));
            break;
        }
    }


    template<typename Pred>
    void calcRowHelp(const vector<size_t>& indexes, const string& col, const string& name, Pred predicate)
    {
        size_t count = 0;
        size_t idx = tables[name].cols[col];
        for (size_t i = 0; i < tables[name].entries.size(); ++i)
        {
            if (predicate(tables[name].entries[i][idx]))
            {
                ++count;
                if (!quiet)
                {
                    for (size_t j = 0; j < indexes.size(); ++j)
                    {
                        cout << tables[name].entries[i][indexes[j]] << " ";
                    }
                    cout << "\n";
                }
            }
        }
        cout << "Printed " << count << " matching rows from " << name << "\n";
    }

    template<typename Pred>
    void removeRow(const string& name, Pred predicate)
    {
        size_t initial = tables[name].entries.size();
        auto start = tables[name].entries.begin();

        auto end = remove_if(tables[name].entries.begin(), tables[name].entries.end(), predicate);

        tables[name].entries.resize(end - start);

        cout << "Deleted " << initial - tables[name].entries.size() << " rows from " << name << "\n";
    }

    bool checkCol(vector<pair<string, string>> &cols, const string& printCol, const string &name1, const string &name2, size_t printNum)
    {
        if (printNum == 1)
        {
            if (tables[name1].cols.find(printCol) == tables[name1].cols.end())
            {
                string trash;
                cout << "Error: " << printCol << " does not name a column in " << name1 << "\n";
                getline(cin, trash);
                return true;
            }
            cols.push_back({ name1, printCol });
        }
        else
        {
            if (tables[name2].cols.find(printCol) == tables[name2].cols.end())
            {
                string trash;
                cout << "Error: " << printCol << " does not name a column in " << name2 << "\n";
                getline(cin, trash);
                return true;
            }
            cols.push_back({ name2, printCol });
        }
        return false;
    }

    //Pair = {table, printCol}
    void joinAlgo(const vector<pair<string, string>>& cols, const string& name1, const string& name2, const string &col1, const string &col2)
    {
        size_t idx1 = tables[name1].cols[col1], idx2 = tables[name2].cols[col2], count = 0;

        //Prints colNames
        if (!quiet)
        {
            for (size_t i = 0; i < cols.size(); ++i)
            {
                cout << cols[i].second << " ";
            }
            cout << "\n";
        }


        for (size_t i = 0; i < tables[name1].entries.size(); ++i)
        {
            for (size_t j = 0; j < tables[name2].entries.size(); ++j)
            {
                if (tables[name1].entries[i][idx1] == tables[name2].entries[j][idx2])
                {
                    count++;
                    if (!quiet)
                    {
                        for (size_t k = 0; k < cols.size(); ++k)
                        {
                            size_t printIdx = tables[cols[k].first].cols[cols[k].second];
                            if (cols[k].first == name1)
                                cout << tables[name1].entries[i][printIdx] << " ";
                            else
                                cout << tables[name2].entries[j][printIdx] << " ";
                        }
                        cout << "\n";
                    }
                }
            }
        }

        cout << "Printed " << count << " rows from joining " << name1 << " to " << name2 << "\n";
    }

    void Hash(const string& name, const string& col)
    {
        tables[name].index = col;
        size_t idx = tables[name].cols[col];
        for (size_t i = 0; i < tables[name].entries.size(); ++i)
        {
            tables[name].hash[tables[name].entries[i][idx]].push_back(i);
        }
    }

    void BST(const string& name, const string& col)
    {
        tables[name].index = col;
        size_t idx = tables[name].cols[col];
        for (size_t i = 0; i < tables[name].entries.size(); ++i)
        {
            tables[name].bst[tables[name].entries[i][idx]].push_back(i);
        }
    }
};