// Hospital.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include<fstream>
#include<string>
#include<conio.h>
using namespace std;

class Patient
{
    string First_Name, Last_Name;
    string Gender;
    int Age;
    long Phone_Number;
    string Address;
    string Problem;
    string Doctor;
public:
    Patient();
    string Get_F_N();
};
Patient::Patient()
{ 
    fstream temp;
    fstream names;

    cout << "\nEnter first name: ";
    cin >> First_Name;
    cout << "\nEnter last name: ";
    cin >> Last_Name;
    cout << "\nEnter the Gender: ";
    cin >> Gender;
    cout << "\nEnter the age: ";
    cin >> Age;
    cout << "\nEnter the phone number: ";
    cin >> Phone_Number;
    cout << "\nEnter address: ";
    cin >> Address;
    cout << "\nEnter the problem: ";
    cin >> Problem;
    cout << "\nEnter the doctor name: ";
    cin >> Doctor; 

    //---saving the information in a file---
    string name = First_Name;
    name += ".txt";
    temp.open(name, ios::out);
    temp << First_Name << "\n";
    temp << Last_Name << "\n";
    temp <<  Gender << "\n";
    temp << Age << "\n";
    temp << Phone_Number << "\n";
    temp << Address << "\n";
    temp << Problem << "\n";
    temp << Doctor << "\n";
    temp.close();
    //___________________________________//
    names.open("Patients.txt", ios::app);
    names<< "\n" << First_Name << "\n";
    names.close();
}
string Patient::Get_F_N()
{
    return this->First_Name;
}
class Operations
{
    string password;
 public:
    Operations();
    void Add_Patient();
    void Edit();
    void Print_P_L();
    void Search();
    void Delete();
};
Operations::Operations()
{
    string p;
    password = "Hospital";
l:
    cout << "\n Password: "; 
    cin >> p;
    if (p == this->password)
    {
        cout <<" You are welcome ";
        system("pause");
    }
    else 
    {
        cout<<" Wrong password try again.\n ";
        goto l;
    }
}
void Operations::Add_Patient()
{
    Patient a;
}
void Operations::Print_P_L()
{
    fstream Patients;
    fstream Patient1;
    string temp;
    Patients.open("Patients.txt",ios::in);
    if (Patients.fail())
        cout << "\nThere is no patients ";
    else
    {
        cout <<"\n---------------------------------------------"
             <<"------------------------------------------------"
             <<"\nNames         Gender       Age       Phone number"
             <<"       Address       Problem       Doctor "
             <<"\n----------------------------------------------"
             <<"-----------------------------------------------\n";
        do 
        {
            getline(Patients,temp);
            if (temp == "")
                continue;
            temp += ".txt";
            Patient1.open(temp, ios::in);
            getline(Patient1, temp);
            cout << temp << " ";
            getline(Patient1, temp);
            cout << temp << "      ";
            getline(Patient1, temp);
            cout << temp << "          ";
            getline(Patient1, temp);
            cout << temp << "         ";
            getline(Patient1, temp);
            cout << temp << "        ";
            getline(Patient1, temp);
            cout << temp << "       ";
            getline(Patient1, temp);
            cout << temp << "       ";
            getline(Patient1, temp);
            cout << temp << "    ";
            Patient1.close();
            cout << endl;
        } while (!Patients.eof());
        cout << "\n---------------------------------------------"
            << "------------------------------------------------";
    }

}
void Operations::Delete()
{
    fstream Patient1,temp_file;
    string name,temp;
    cout << "\nEnter the patient name to delete: ";
    cin >> name;
    string name1 = name;
    name+=".txt";
    Patient1.open(name,ios::in);
    if (Patient1.fail())
    {
         cout << endl << name1 << " doesn't exist .";
    }
    else
    {
        Patient1.close();
        if (remove(name.c_str()) == 0)
        { 
            cout << endl << name1 << " deleted successfully .";
            Patient1.open("Patients.txt", ios::in);
            temp_file.open("tempfile.txt", ios::out);
            do
            {
                getline(Patient1, temp);
                if (temp != name1)
                {
                    temp_file << temp << endl;
                }
            } while (!Patient1.eof());
            Patient1.close();
            temp_file.close();
            remove("Patients.txt");
            rename("tempfile.txt","Patients.txt");
        } 
        else
        {
            cout << "\nFail to delete " << name1;
        }

    }
}
void Operations::Search() 
{
    string name,name1,temp;
    fstream Patient1;
    cout << "\nEnter the patient name to search about: ";
    cin >> name;
    name1 = name;
    name += ".txt";
    Patient1.open( name, ios::in);
    if (Patient1.fail())
        cout <<endl<<name1<<" doesn't exist ";
    else
    {
        cout<< "\n---------------------------------------------"
            << "------------------------------------------------"
            << "\nNames         Gender       Age       Phone number"
            << "       Address       Problem       Doctor "
            << "\n----------------------------------------------"
            << "-----------------------------------------------\n";
            getline(Patient1, temp);
            cout << temp << " ";
            getline(Patient1, temp);
            cout << temp << "      ";
            getline(Patient1, temp);
            cout << temp << "          ";
            getline(Patient1, temp);
            cout << temp << "         ";
            getline(Patient1, temp);
            cout << temp << "        ";
            getline(Patient1, temp);
            cout << temp << "       ";
            getline(Patient1, temp);
            cout << temp << "       ";
            getline(Patient1, temp);
            cout << temp << "    ";
            Patient1.close();
            cout << endl;
    }
}
void Operations::Edit()
{
    fstream Patient1, Patients, temp_file;
    string name, temp;
    Patients.open("Patients.txt", ios::in);
    if (Patients.fail())
    {
        cout << "\nThere is no patients "<<endl;
    }
    else
    {
        cout << "\nEnter the patient name to edit: ";
        cin >> name;
        string name1 = name;
        name += ".txt";
        Patient1.open(name, ios::in);
        if (Patient1.fail())
        {
            cout << endl << name1 << " doesn't exist .";
        }
        else
        {
            Patient1.close();
            remove(name.c_str());//delete the file
            //delete from the patients file
            Patient1.open("Patients.txt", ios::in);
            temp_file.open("tempfile.txt", ios::out);
            do
            {
                getline(Patient1, temp);
                if (temp != name1)
                {
                    temp_file << temp << endl;
                }
            } while (!Patient1.eof());
            Patient1.close();
            temp_file.close();
            remove("Patients.txt");
            rename("tempfile.txt", "Patients.txt");
            //adding the new info
            Add_Patient();
        }
    }

}
void print() 
{
    cout << "\n\n\t\t\t\t######################################\n\n"
        << "\t\t\t\t#            WELCOME TO              #\n\t\t\t\t#\t\t\t\t     #\n"
        << "\t\t\t\t#  CORONA HOSPITAL MANAGEMENT SYSTEM #\n\n"
        << "\t\t\t\t######################################\n";
}
void info()
{
    cout << "\n\n\n\t\t\t\t\t\tTAIZ UNIVERSITY ." << endl;
    cout << "\t\t\t\t\t   COLLEGE OF ENGINEERING . " << endl;
    cout << "\t\t\t\t\tSECTHION :INFORMATION TECHNOLOGY ." << endl;
    cout << "\t\t\t\t\t\tCOURSE :OOP ." << endl;
    cout << "\t\t\t\t COURS LECTURER : ENG.EZZADDIN AHMED." << endl ;
    cout << "\t\tSTUDENTS NAMES : OMAR YAHYA SAEED  , HAZIM SAEED ALQADY" 
         <<" and SHEHAB MOHAMMED AL-WARD" << endl << endl << endl;
    system("pause");
    system("cls");
}
int Choice()
{
    int cho;
    cout << endl << endl << "\t"
        << "-------------------------------------------------------------\n"
        << "           \t\t\t CORONA HOSPITAL \n        "
        << "-------------------------------------------------------------\n";

    cout << endl << endl << endl << "\t\t\t";
    cout << "1. Add a new patient" << endl << "\t\t\t";
    cout << "2. Modify patient information" << endl << "\t\t\t";
    cout << "3. Delete patient information" << endl << "\t\t\t";
    cout << "4. search about patient" << endl << "\t\t\t";
    cout << "5. The list of patients" << endl << "\t\t\t";
    cout << "0. Exit" << endl << "\t\t\t";
    cout << endl << endl;
    cout << "\t Enter your choice: ";
    cin >> cho;
    system("cls");
    return cho;
}
int main()
{
    info();
    print();
    Operations a1; 
    system("cls");
l:
    system("cls");
    int choice = Choice();
    switch (choice)
    {
    case 0:
    {
        exit(0);
    }
    case 1:
    {
        a1.Add_Patient();
        break;
    }
    case 2:
    {
        a1.Edit();
        cout << endl << endl << endl;
        system("pause");
        break;
    }
    case 3:
    {
        a1.Delete();
        cout << endl << endl << endl;
        system("pause");
        break;
    }
    case 4:
    {
        a1.Search();
        cout << endl << endl << endl;
        system("pause");
        break;
    }
    case 5:
    {
        a1.Print_P_L();
        cout << endl << endl << endl;
        system("pause");
        break;
    }
    default:
    {
        cout << "\nEnter a number between 0 and 5";
    }
    }
    goto l;
    system("pause");
    return 0;
}

