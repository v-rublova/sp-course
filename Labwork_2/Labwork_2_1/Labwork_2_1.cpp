#include "pch.h"
#include <iostream>
#include "windows.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdio.h> 
#include <stdlib.h>

using namespace std;

struct record {
	unsigned char index;
	FILETIME creationTime;
	char content[80];
	unsigned char accessCount;
};

//String date from SYSTEMTIME
string SDate(unsigned short day, unsigned short month, unsigned short year) {
	string rs = std::to_string(day) + "th of "s;
	switch (month)
	{
	case 1:
		rs += "January";
		break;
	case 2:
		rs += "February";
		break;
	case 3:
		rs += "March";
		break;
	case 4:
		rs += "April";
		break;
	case 5:
		rs += "May";
		break;
	case 6:
		rs += "June";
		break;
	case 7:
		rs += "July";
		break;
	case 8:
		rs += "August";
		break;
	case 9:
		rs += "September";
		break;
	case 10:
		rs += "October";
		break;
	case 11:
		rs += "November";
		break;
	case 12:
		rs += "December";
		break;
	default:
		rs += "???";
		break;
	}
	rs += " "s + std::to_string(year);
	return rs;
}
//String time from SYSTEMTIME
string STime(unsigned short hour, unsigned short minute, unsigned short second) {
	return std::string(std::to_string(hour) + ":"s + std::to_string(minute) + ":"s + std::to_string(second));
}
int main()
{
#pragma region variables
	unsigned char a;
	bool tr;
	unsigned short FileSize;
	unsigned char FRecordCount;
	SYSTEMTIME st;
	SYSTEMTIME lt;
	FILETIME ft;

#pragma endregion variables
	tr = false;
	do {
		cout << "\nChoose work mode:\n\n1 - create new file;\n2 - change previous one;\n0 - close program.\n";
		cin >> a;
		if (a == '1') {
			FILE *file = fopen("RecordList", "w");
			cout << "Enter number of records: ";
			scanf("%hhu", &FRecordCount);
			//number of non-empty records
			unsigned char b = '0';
			fwrite(&b, sizeof(char), 1, file);
			//placeholder for file size
			fwrite(&FileSize, sizeof(short), 1, file);
			//records' placeholders
			for (unsigned char i = 0; i < (int)FRecordCount; i++)
			{
				GetSystemTime(&st);
				SystemTimeToFileTime(&st, &ft);
				struct record rec = { 1, ft,"",0 };
				rec.index = i;
				fwrite(&rec, sizeof(struct record), 1, file);
			}
			//actual file size
			fseek(file, 0, SEEK_END);
			FileSize = ftell(file);
			fseek(file, sizeof(char), SEEK_SET);
			fwrite(&FileSize, sizeof(short), 1, file);
			fclose(file);
			cout << "A file was created.";
		}
		else if (a == '2') {

			FILE *file = fopen("RecordList", "r+");
			SYSTEMTIME lt;
			record inp_rec;
			unsigned char ind;
			bool er_tr, trf;
			er_tr = false;
			trf = false;
			cout << "Enter record's Index to access it: ";
			scanf("%hhu", &ind);
			fseek(file, sizeof(char) + sizeof(short), SEEK_SET);
			while (!trf && fread(&inp_rec, sizeof(struct record), 1, file))
			{
				if (inp_rec.index == ind) {
					er_tr = true;
					trf = true;
					cout << "Requested record was found.\n";
					if ((string)inp_rec.content == "") {
						cout << "No content was found\n";
					}
					else {
						cout << "Content: " << inp_rec.content << "\n";
					}

					FileTimeToSystemTime(&inp_rec.creationTime, &lt);
					cout << "Last modified: " << SDate(lt.wDay, lt.wMonth, lt.wYear) << " " << STime(lt.wHour, lt.wMinute, lt.wSecond) << endl;
					cout << "Number of modifications: " << (int)inp_rec.accessCount << endl;
				}
			}
			if (er_tr) {
				char b;
				cout << "Choose operation: 1 - delete record; 2 - change it's content; 0 - cancel operation;\n";
				cin >> b;
				if (b == '1') {
					if ((string)inp_rec.content != "") {
						//change number of non empty records
						unsigned char b = '0';
						fseek(file, 0, SEEK_SET);
						fread(&b, sizeof(char), 1, file);
						b -= 1;
						fseek(file, 0, SEEK_SET);
						fwrite(&b, sizeof(char), 1, file);
					}
					//as a 'deletion' of a record it was chosen to place new empty record on it's place 
					fseek(file, sizeof(char) + sizeof(short) + (int)ind * sizeof(struct record), SEEK_SET);
					GetSystemTime(&st);
					SystemTimeToFileTime(&st, &ft);
					struct record empty = { ind, ft,"",0 };
					fwrite(&empty, sizeof(struct record), 1, file);
					cout << "Record was deleted.\n";
					fclose(file);
				}
				else if (b == '2') {
					char cont[80];
					cout << "Input new content: ";
					cin.getline(cont, sizeof(cont));
					cin.getline(cont, sizeof(cont));
					cin.clear();
					cin.ignore(INT_MAX, '\n');
					if ((string)cont != "") {
						//change number of non empty records
						unsigned char b = '0';
						fseek(file, 0, SEEK_SET);
						fread(&b, sizeof(char), 1, file);
						b += 1;
						fseek(file, 0, SEEK_SET);
						fwrite(&b, sizeof(char), 1, file);
					}
					strncpy(inp_rec.content, cont, sizeof(inp_rec.content));
					inp_rec.accessCount++;
					fseek(file, sizeof(char) + sizeof(short) + (int)ind * sizeof(struct record), SEEK_SET);
					fwrite(&inp_rec, sizeof(struct record), 1, file);
					fclose(file);
				}
				else if (b == '0') {}
				else {
					cout << "An error has occurred.\n";
				}
			}
			else {
				cout << "A record with given Index doesn't exist.\n";
			}
		}
		else if (a == '0') {
			tr = true;
		}
		else {
			cout << "An error has occurred. Try again.\n";
		}
	} while (!tr);
}
