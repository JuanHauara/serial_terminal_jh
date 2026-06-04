/*
	Clase para guardar y leer variables desde la memoria
	no volatil de la computadora, de forma transparente.
*/


#ifndef PERSISTENT_DATA_H
#define PERSISTENT_DATA_H


#include <iostream>
#include <string>
#include <vector>
#include <fstream>


using namespace std;


class persistent_data
{
	public:
		persistent_data(string data_file_name);
		
		bool load_data(void);
		
		int get_int(string target_var_name);
		float get_float(string target_var_name);
		string get_string(string target_var_name);
		bool get_bool(string target_var_name);
		
		void set_int(string target_var_name, int target_var_value);
		void set_float(string target_var_name, float target_var_value);
		void set_string(string target_var_name, string target_var_value);
		void set_bool(string target_var_name, bool target_var_value);
		
	private:
		string data_file_name = "";
		vector<string> var_name;
		vector<string> var_value;
		int var_count = 0;  // Indica la cantidad total de variables persistentes.
		
		void parse_line(string line, int index);
		int seek_var_index(string target_var_name);
};


#endif
