#include <iostream>

using namespace std;

const char* codeMap[16] =
{ "0",
"1",
"2",
"3",
"4",
"5",
"6",
"7",
"8",
"9",
"A",
"B",
"C",
"D",
"E",
"F", };

unsigned int generateID(char* str)
{
    unsigned int ID = 0;
    int c;
    while (c = *str++)
    {
        ID = c + (ID << 6) + (ID << 16) - ID;
    }
    return ID;
}

string hexCodedAscii(unsigned int asd)
{
    string output = "";
    for (size_t i = sizeof(int) * 2; i > 0; i--)
    {
        int temp = ((asd & (0xF << (i - 1) * 4)) >> (i - 1) * 4);
        output.append(codeMap[temp]);
    }

    return output;

}

string hash(string str) {

    return hexCodedAscii(generateID((char*)str.c_str()));
}

/*
*       
*       Old debug main. TODO: sacarlo
* 
int main(int, char**)
{
    string penelope("000009EB");
    unsigned int hola = generateID((char*)penelope.c_str());
    cout << hola << endl;
    cout << hexCodedAscii(hola) << endl;


    return 0;
}
*/