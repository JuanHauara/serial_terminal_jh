/*
	Class used to save and read variables from persistent computer storage.
*/


#ifndef PERSISTENT_DATA_H
#define PERSISTENT_DATA_H


#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <exception>


using namespace std;


class persistent_data
{
	public:
		persistent_data(const string &data_file_name);

		bool load_data(void);

		int get_int(const string &target_var_name);
		float get_float(const string &target_var_name);
		string get_string(const string &target_var_name);
		bool get_bool(const string &target_var_name);

		bool set_int(const string &target_var_name, int target_var_value);
		bool set_float(const string &target_var_name, float target_var_value);
		bool set_string(const string &target_var_name, const string &target_var_value);
		bool set_bool(const string &target_var_name, bool target_var_value);

	private:
		string data_file_name = "";
		vector<string> var_name;
		vector<string> var_value;
		int var_count = 0;  // Total persistent variable count.

		bool parse_line(const string &line);
		bool write_data(void);
		int seek_var_index(const string &target_var_name);
};


#endif
