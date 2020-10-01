//Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include "SillyQL.cpp"

int main(int argc, char** argv)
{
	ios_base::sync_with_stdio(false);
	cin >> boolalpha;
	cout << boolalpha;
	SillyQL silly;
	silly.getOptions(argc, argv);
	silly.readCommands();
	return 0;
}