/*
	Clase para guardar y leer variables desde la memoria
	no volatil de la computadora, de forma transparente.
*/


#ifndef __Persistent_data_h__
#define __Persistent_data_h__


#include <iostream>
#include <string>
#include <vector>
#include <fstream>


using namespace std;


class Persistent_data
{
	public:
		Persistent_data(string dataFileName);
		
		bool loadData(void);
		
		int getInt(string _varName);
		float getFloat(string _varName);
		string getString(string _varName);
		bool getBool(string _varName);
		
		void setInt(string _varName, int _varValue);
		void setFloat(string _varName, float _varValue);
		void setString(string _varName, string _varValue);
		void setBool(string _varName, bool _varValue);
		
	private:
		string dataFileName = "";
		vector<string> varName;
		vector<string> varValue;
		int varCount = 0;  // Indica la cantidad total de variables persistentes.
		
		void parseLine(string line, int index);
		int seekVarIndex(string _varName);
};


#endif
