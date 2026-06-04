#include "persistent_data.h"

persistent_data::persistent_data(const string &data_file_name)
{
	this->data_file_name = data_file_name;
}

bool persistent_data::load_data(void)
{
	var_name.clear();
	var_value.clear();
	var_count = 0;

	if (data_file_name.empty())
	{
		cerr << "Persistent data file name is empty." << endl;

		return false;
	}

	ifstream myfile;
	myfile.open(data_file_name);

	if (!myfile.is_open())
	{
		#if DEBUG_LOG
		cout << "Error trying to open persistent data file '" << data_file_name << "'" << endl;
		#endif

		return false;
	}

	string line;
	int line_number = 0;

	while (getline(myfile, line))
	{
		line_number++;

		if (line.empty())
		{
			continue;
		}

		if (!parse_line(line))
		{
			cerr << "Ignoring malformed persistent data line " << line_number << "." << endl;
		}

		#if DEBUG_LOG
		cout << line << endl;
		cout << "-------------" << endl;
		#endif
	}

	if (myfile.bad())
	{
		cerr << "Error reading persistent data file '" << data_file_name << "'" << endl;

		var_name.clear();
		var_value.clear();
		var_count = 0;

		return false;
	}

	myfile.close();
	var_count = static_cast<int>(var_name.size());

	#if DEBUG_LOG
	cout << "Persistent variable count = " << var_count << endl;
	#endif

	return true;
}

bool persistent_data::parse_line(const string &line)
{
	size_t separator_index = line.find('=');

	if ((separator_index == string::npos) || (separator_index == 0))
	{

		return false;
	}

	string parsed_var_name = line.substr(0, separator_index);
	string parsed_var_value = line.substr(separator_index + 1);

	for (size_t i = 0; i < var_name.size(); i++)
	{
		if (var_name[i] == parsed_var_name)
		{
			var_value[i] = parsed_var_value;

			return true;
		}
	}

	var_name.push_back(parsed_var_name);
	var_value.push_back(parsed_var_value);
	var_count = static_cast<int>(var_name.size());

	return true;
}

bool persistent_data::write_data(void)
{
	if (data_file_name.empty())
	{
		cerr << "Persistent data file name is empty." << endl;

		return false;
	}

	ofstream myfile;
	myfile.open(data_file_name, ios::trunc);

	if (!myfile.is_open())
	{
		cerr << "Error trying to open persistent data file '" << data_file_name << "' for writing." << endl;

		return false;
	}

	for (size_t i = 0; i < var_name.size(); i++)
	{
		myfile << var_name[i] << "=" << var_value[i] << endl;
	}

	if (!myfile.good())
	{
		cerr << "Error writing persistent data file '" << data_file_name << "'." << endl;

		return false;
	}

	myfile.close();

	if (myfile.fail())
	{
		cerr << "Error closing persistent data file '" << data_file_name << "'." << endl;

		return false;
	}

	return true;
}

int persistent_data::seek_var_index(const string &target_var_name)
{
	for (int i = 0; i < var_count; i++)
	{
		if (var_name[i] == target_var_name)
		{

			return i;
		}
	}

	#if DEBUG_LOG
	cout << "Persistent variable '" << target_var_name << "' does not exist." << endl;
	#endif

	return -1;
}

int persistent_data::get_int(const string &target_var_name)
{
	int index = seek_var_index(target_var_name);

	if (index < 0)
	{

		return 0;
	}

	int n = 0;
	size_t processed_chars = 0;

	try
	{
		n = stoi(var_value[index], &processed_chars);
	}
	catch (const exception &e)
	{
		cerr << "Error reading persistent int variable '" << target_var_name << "'." << endl;

		return 0;
	}

	if (processed_chars != var_value[index].length())
	{
		cerr << "Invalid persistent int variable '" << target_var_name << "'." << endl;

		return 0;
	}

	return n;
}

float persistent_data::get_float(const string &target_var_name)
{
	int index = seek_var_index(target_var_name);

	if (index < 0)
	{

		return 0.0;
	}

	float n = 0.0;
	size_t processed_chars = 0;

	try
	{
		n = stof(var_value[index], &processed_chars);
	}
	catch (const exception &e)
	{
		cerr << "Error reading persistent float variable '" << target_var_name << "'." << endl;

		return 0.0;
	}

	if (processed_chars != var_value[index].length())
	{
		cerr << "Invalid persistent float variable '" << target_var_name << "'." << endl;

		return 0.0;
	}

	return n;
}

string persistent_data::get_string(const string &target_var_name)
{
	int index = seek_var_index(target_var_name);

	if (index < 0)
	{

		return "";
	}

	return var_value[index];
}

bool persistent_data::get_bool(const string &target_var_name)
{
	int index = seek_var_index(target_var_name);

	if (index < 0)
	{

		return false;
	}

	if (var_value[index] == "true")
	{

		return true;
	}

	if (var_value[index] == "false")
	{

		return false;
	}

	cerr << "Invalid persistent bool variable '" << target_var_name << "'." << endl;

	return false;
}

bool persistent_data::set_string(const string &target_var_name, const string &target_var_value)
{
	if (target_var_name.empty())
	{
		cerr << "Persistent variable name is empty." << endl;

		return false;
	}

	int index = seek_var_index(target_var_name);

	if (index >= 0)
	{
		string previous_value = var_value[index];
		var_value[index] = target_var_value;

		if (!write_data())
		{
			var_value[index] = previous_value;

			return false;
		}

		return true;
	}

	var_name.push_back(target_var_name);
	var_value.push_back(target_var_value);
	var_count = static_cast<int>(var_name.size());

	if (!write_data())
	{
		var_name.pop_back();
		var_value.pop_back();
		var_count = static_cast<int>(var_name.size());

		return false;
	}

	return true;
}

bool persistent_data::set_int(const string &target_var_name, int target_var_value)
{

	return set_string(target_var_name, to_string(target_var_value));
}

bool persistent_data::set_float(const string &target_var_name, float target_var_value)
{

	return set_string(target_var_name, to_string(target_var_value));
}

bool persistent_data::set_bool(const string &target_var_name, bool target_var_value)
{
	if (target_var_value)
	{

		return set_string(target_var_name, "true");
	}

	return set_string(target_var_name, "false");
}
