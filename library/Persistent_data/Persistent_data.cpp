#include "Persistent_data.h"

Persistent_data::Persistent_data(string dataFileName)
{
	this->dataFileName = dataFileName;
}

bool Persistent_data::loadData(void)
{
	ifstream myfile;
	myfile.open(dataFileName);  // Abre el archivo para lectura.
	string line;
	
	if (myfile.is_open())
	{
		varName.clear();
		varValue.clear();
		int index = 0;
		while (getline(myfile, line))
		{
			parseLine(line, index);
			index++;
			
			#if DEBUG_LOG
			cout << line << endl;
			cout << varName[index - 1] << endl;
			cout << varValue[index - 1] << endl;
			cout << "-------------" << endl;
			#endif
		}
		
		myfile.close();
		varCount = varName.size();  // Cantidad total de variables persistentes.
		#if DEBUG_LOG
		cout << "cantidad total de variables persistentes = " << varCount << endl;  // DEBUG
		#endif
		
		return true;
	}
	#if DEBUG_LOG
	else 
	{
		cout << "Error al intentar abrir archivo " << dataFileName << endl;
	}
	#endif
	
	return false;
}

void Persistent_data::parseLine(string line, int index)
{
	/*
	 * Primero busca el caracter "=" y luego divide la línea de texto en 
	 * "nombre de variable" y "valor de variable".
	 */
	  
	int ind = line.find('=');
	
	if (ind > 0)
	{
		string strVarName = line.substr(0, ind);
		string strVarValue = line.substr(ind + 1, line.length());
		
		varName.push_back(strVarName);
		varValue.push_back(strVarValue);
	}
}

int Persistent_data::seekVarIndex(string _varName)
{
	/*
	 * Devuelve el index de la variable persistente de nombre "_varName".
	 */
	
	for (int i = 0; i < varCount; i++)
	{
		if (varName[i] == _varName) return i;
	}
	
	#if DEBUG_LOG
	cout << "Error: No existe la variable persistente '" << _varName << "'" << endl;
	#endif
	
	return -1;
}

int Persistent_data::getInt(string _varName)
{
	// Devuelve 0 si no encuentra la variable en el archivo.
	
	int index = seekVarIndex(_varName);
	
	if (index >= 0)
	{
		int n = 0;
		try 
		{
			n = stoi(varValue[index]);
		}
		catch (const std::invalid_argument& e) 
		{
			cerr << "Error al intentar leer variable persistente tipo int '" << _varName << "'" << endl;
		}
  
		return n;
	}
	else 
	{
		return 0;
	}
}

float Persistent_data::getFloat(string _varName)
{
	// Devuelve 0.0 si no encuentra la variable en el archivo.
	
	int index = seekVarIndex(_varName);
	
	if (index >= 0)
	{
		float n = 0.0;
		try 
		{
			n = stof(varValue[index]);
		}
		catch (const std::invalid_argument& e) 
		{
			cerr << "Error al intentar leer variable persistente tipo float '" << _varName << "'" << endl;
		}
  
		return n;
	}
	else 
	{
		return 0.0;
	}
}

string Persistent_data::getString(string _varName)
{
	// Devuelve "" si no encuentra la variable en el archivo.
	
	int index = seekVarIndex(_varName);
	
	if (index >= 0)
	{
		return varValue[index];
	}
	else
	{
		return "";
	}
}

bool Persistent_data::getBool(string _varName)
{
	// Devuelve false si no encuentra la variable en el archivo.
	
	int index = seekVarIndex(_varName);
	
	if (index >= 0)
	{
		if (varValue[index] == "true")
		{
			return true;
		}
		else if (varValue[index] == "false")
		{
			return false;
		}
		else
		{
			cerr << "Error al intentar leer variable persistente tipo bool '" << _varName << "'" << endl;
		}
	}

	return false;
}

void Persistent_data::setString(string _varName, string _varValue)
{
	/*
		Primero se fija si la variable existe, si no existe la crea y la 
		inicializa. 
		Si la variable persistente ya existe, setea su valor a _varValue
		en RAM y en el archivo persistentData.dat.
	*/
	ofstream myfile;
	
	// Deuelve el index de la variable dentro del vector en memoria RAM
	// o -1 si no existe.
	int index = seekVarIndex(_varName);
	
	if (index >= 0)  // si ya existe
	{
		/*
			Actualiza el nuevo valor en el disco y en la RAM.
			Abre el archivo para escritura y si ya existe borra el contenido previo.
		*/
		myfile.open(dataFileName, ios::trunc);
		
		for (int i = 0; i < varCount; i++)
		{
			if (varName[i] == _varName)
			{
				// Actualiza el nuevo valor en el disco.
				myfile << varName[i] << "=" << _varValue << endl;
				
				// Y también lo actualiza en RAM.
				varValue[i] = _varValue;
			}
			else
			{
				myfile << varName[i] << "=" << varValue[i] << endl;
			}
		}
		
		myfile.close();
	}
	else
	{
		// Si no existe agrega la nueva variable al final del archivo.
		// Abre el archivo como escritura y para agregar al final.
		myfile.open(dataFileName, ios::app);
		myfile << _varName << "=" << _varValue << endl;
		myfile.close();
		
		// Y también la agrega en RAM.
		varName.push_back(_varName);
		varValue.push_back(_varValue);
		
		varCount++;  // Agregó una variable más.
	}
}

void Persistent_data::setInt(string _varName, int _varValue)
{
	setString(_varName, to_string(_varValue));
}

void Persistent_data::setFloat(string _varName, float _varValue)
{
	setString(_varName, to_string(_varValue));
}

void Persistent_data::setBool(string _varName, bool _varValue)
{
	if (_varValue)
	{
		setString(_varName, "true");
	}
	else
	{
		setString(_varName, "false");
	}
}


