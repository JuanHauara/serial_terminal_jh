#include "persistent_data.h"

persistent_data::persistent_data(string data_file_name)
{
	this->data_file_name = data_file_name;
}

bool persistent_data::load_data(void)
{
	ifstream myfile;
	myfile.open(data_file_name);  // Abre el archivo para lectura.
	string line;
	
	if (myfile.is_open())
	{
		var_name.clear();
		var_value.clear();
		int index = 0;
		while (getline(myfile, line))
		{
			parse_line(line, index);
			index++;
			
			#if DEBUG_LOG
			cout << line << endl;
			cout << var_name[index - 1] << endl;
			cout << var_value[index - 1] << endl;
			cout << "-------------" << endl;
			#endif
		}
		
		myfile.close();
		var_count = var_name.size();  // Cantidad total de variables persistentes.
		#if DEBUG_LOG
		cout << "cantidad total de variables persistentes = " << var_count << endl;  // DEBUG
		#endif
		
		return true;
	}
	#if DEBUG_LOG
	else 
	{
		cout << "Error al intentar abrir archivo " << data_file_name << endl;
	}
	#endif
	
	return false;
}

void persistent_data::parse_line(string line, int index)
{
	/*
	 * Primero busca el caracter "=" y luego divide la línea de texto en 
	 * "nombre de variable" y "valor de variable".
	 */
	  
	int ind = line.find('=');
	
	if (ind > 0)
	{
		string str_var_name = line.substr(0, ind);
		string str_var_value = line.substr(ind + 1, line.length());
		
		var_name.push_back(str_var_name);
		var_value.push_back(str_var_value);
	}
}

int persistent_data::seek_var_index(string target_var_name)
{
	/*
	 * Devuelve el index de la variable persistente de nombre "target_var_name".
	 */
	
	for (int i = 0; i < var_count; i++)
	{
		if (var_name[i] == target_var_name) return i;
	}
	
	#if DEBUG_LOG
	cout << "Error: No existe la variable persistente '" << target_var_name << "'" << endl;
	#endif
	
	return -1;
}

int persistent_data::get_int(string target_var_name)
{
	// Devuelve 0 si no encuentra la variable en el archivo.
	
	int index = seek_var_index(target_var_name);
	
	if (index >= 0)
	{
		int n = 0;
		try 
		{
			n = stoi(var_value[index]);
		}
		catch (const std::invalid_argument& e) 
		{
			cerr << "Error al intentar leer variable persistente tipo int '" << target_var_name << "'" << endl;
		}
  
		return n;
	}
	else 
	{
		return 0;
	}
}

float persistent_data::get_float(string target_var_name)
{
	// Devuelve 0.0 si no encuentra la variable en el archivo.
	
	int index = seek_var_index(target_var_name);
	
	if (index >= 0)
	{
		float n = 0.0;
		try 
		{
			n = stof(var_value[index]);
		}
		catch (const std::invalid_argument& e) 
		{
			cerr << "Error al intentar leer variable persistente tipo float '" << target_var_name << "'" << endl;
		}
  
		return n;
	}
	else 
	{
		return 0.0;
	}
}

string persistent_data::get_string(string target_var_name)
{
	// Devuelve "" si no encuentra la variable en el archivo.
	
	int index = seek_var_index(target_var_name);
	
	if (index >= 0)
	{
		return var_value[index];
	}
	else
	{
		return "";
	}
}

bool persistent_data::get_bool(string target_var_name)
{
	// Devuelve false si no encuentra la variable en el archivo.
	
	int index = seek_var_index(target_var_name);
	
	if (index >= 0)
	{
		if (var_value[index] == "true")
		{
			return true;
		}
		else if (var_value[index] == "false")
		{
			return false;
		}
		else
		{
			cerr << "Error al intentar leer variable persistente tipo bool '" << target_var_name << "'" << endl;
		}
	}

	return false;
}

void persistent_data::set_string(string target_var_name, string target_var_value)
{
	/*
		Primero se fija si la variable existe, si no existe la crea y la 
		inicializa. 
		Si la variable persistente ya existe, setea su valor a target_var_value
		en RAM y en el archivo persistentData.dat.
	*/
	ofstream myfile;
	
	// Deuelve el index de la variable dentro del vector en memoria RAM
	// o -1 si no existe.
	int index = seek_var_index(target_var_name);
	
	if (index >= 0)  // si ya existe
	{
		/*
			Actualiza el nuevo valor en el disco y en la RAM.
			Abre el archivo para escritura y si ya existe borra el contenido previo.
		*/
		myfile.open(data_file_name, ios::trunc);
		
		for (int i = 0; i < var_count; i++)
		{
			if (var_name[i] == target_var_name)
			{
				// Actualiza el nuevo valor en el disco.
				myfile << var_name[i] << "=" << target_var_value << endl;
				
				// Y también lo actualiza en RAM.
				var_value[i] = target_var_value;
			}
			else
			{
				myfile << var_name[i] << "=" << var_value[i] << endl;
			}
		}
		
		myfile.close();
	}
	else
	{
		// Si no existe agrega la nueva variable al final del archivo.
		// Abre el archivo como escritura y para agregar al final.
		myfile.open(data_file_name, ios::app);
		myfile << target_var_name << "=" << target_var_value << endl;
		myfile.close();
		
		// Y también la agrega en RAM.
		var_name.push_back(target_var_name);
		var_value.push_back(target_var_value);
		
		var_count++;  // Agregó una variable más.
	}
}

void persistent_data::set_int(string target_var_name, int target_var_value)
{
	set_string(target_var_name, to_string(target_var_value));
}

void persistent_data::set_float(string target_var_name, float target_var_value)
{
	set_string(target_var_name, to_string(target_var_value));
}

void persistent_data::set_bool(string target_var_name, bool target_var_value)
{
	if (target_var_value)
	{
		set_string(target_var_name, "true");
	}
	else
	{
		set_string(target_var_name, "false");
	}
}


