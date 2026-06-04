#include "serial_port_windows.h"


void serial_port::set_serial_port(string serial_port_name_arg)
{
	serial_port_name = serial_port_name_arg;
	cout << "Class serial_port_windows: serial_port_name_arg = " << serial_port_name_arg << endl;
	cout << "Class serial_port_windows: serial_port_name = " << serial_port_name << endl;
	cout << "Class serial_port_windows: serial_port_name.length() = " << serial_port_name.length() << endl;
}

void serial_port::set_baud_rate(uint32_t baud_rate)
{
	this->baud_rate = baud_rate;
}

void serial_port::set_data_bits(int32_t n)
{
	data_bits = n;
}

void serial_port::set_parity(int32_t n)
{
	parity = n;
}

void serial_port::set_stop_bits(int32_t n)
{
	stop_bits = n;
}

void serial_port::use_dtr(bool flag)
{
	flag_use_dtr = flag;
}

void serial_port::use_rts(bool flag)
{
	flag_use_rts = flag;
}

string serial_port::get_serial_port(void)
{
	return serial_port_name;
}

uint32_t serial_port::get_baud_rate(void)
{
	return baud_rate;
}

void serial_port::set_inter_byte_timeout(uint32_t ms)
{
	inter_byte_timeout = ms;
}

void serial_port::set_text_control(wxTextCtrl *text_control_received)
{
	this->text_control_received = text_control_received;
}

bool serial_port::set_timeouts(void)
{
	COMMTIMEOUTS timeouts = {0};

	// https://docs.microsoft.com/en-us/windows/win32/api/winbase/ns-winbase-commtimeouts
	timeouts.ReadIntervalTimeout = MAXDWORD;	// La interfaz gráfica de traba si se pone otro valor que no sea este.
	timeouts.ReadTotalTimeoutConstant = 0;		// La interfaz gráfica de traba si se pone otro valor que no sea este.
	timeouts.ReadTotalTimeoutMultiplier = 0;	// La interfaz gráfica de traba si se pone otro valor que no sea este.
	timeouts.WriteTotalTimeoutConstant = 120;
	timeouts.WriteTotalTimeoutMultiplier = 120;

	if(SetCommTimeouts(serial_handle, &timeouts) == 0) 
	{
		last_error = "Error trying to set time outs.";
		#if DEBUG_TERMINAL_SERIAL_PORT_WINDOWS 
		cout << "Error trying to set time outs." << endl;
		#endif
		
		return false;
	}

	return true;
}

bool serial_port::set_parameters(void)
{
	// Obtiene los parámetros del puerto serie
	DCB serial_params = {0};
	
	serial_params.DCBlength = sizeof(serial_params);	// Esto es necesario por algún extraño capricho de Windows
	
	if (GetCommState(serial_handle, &serial_params) == 0)	// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getcommstate
	{
		last_error = "Error trying to get serial port configuration.";
		#if DEBUG_TERMINAL_SERIAL_PORT_WINDOWS 
		cout << "Error trying to get serial port configuration." << endl;
		#endif
		disconnect();
		
		return false;
	}
	
	// Parámetros de la estructura DCB: https://docs.microsoft.com/en-us/windows/win32/api/winbase/ns-winbase-dcb
	serial_params.BaudRate = baud_rate;
	serial_params.ByteSize = data_bits;
	serial_params.Parity = parity;
	serial_params.StopBits = stop_bits;
	
	if (flag_use_dtr) 
	{
		serial_params.fDtrControl = 1;
	}
	else 
	{
		serial_params.fDtrControl = 0;
	}
	
	if (flag_use_rts)
	{
		serial_params.fRtsControl = 1;
	}
	else 
	{
		serial_params.fRtsControl = 0;
	}

	// Setea los parámetros en el puerto serie
	if (SetCommState(serial_handle, &serial_params) == 0)	// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setcommstate
	{
		last_error = "Error trying to set configuration in the serial port.";
		#if DEBUG_TERMINAL_SERIAL_PORT_WINDOWS 
		cout << "Error trying to set configuration in the serial port." << endl;
		#endif
		disconnect();
		
		return false;
	}
	
	return true;
}

bool serial_port::connect(void)
{
	// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
	string serial_port_path = "\\\\.\\" + serial_port_name;
	serial_handle = CreateFileA(serial_port_path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (serial_handle == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			last_error = "Serial port not found.";
			
			#if DEBUG_TERMINAL_SERIAL_PORT_WINDOWS 
			cout << "Serial port missing." << endl;
			#endif
			
			return false;
		}
		
		last_error = "The serial port is probably being used by another application.";

		#if DEBUG_TERMINAL_SERIAL_PORT_WINDOWS 
		cout << "The serial port is probably being used by another application." << endl;
		#endif
		
		return false;
	}
	
	if (!set_timeouts())
	{
		return false;
	}
	
	if (!set_parameters())
	{
		return false;
	}
	
	return true;
}

bool serial_port::disconnect(void)
{
	if (CloseHandle(serial_handle) == 0)	// https://docs.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
	{
		last_error = "Error trying to disconnect.";
		
		return false;
	}
	else
	{
		return true;
	}
}

void serial_port::tx_string(string buff)
{
	// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-writefile
	WriteFile(serial_handle, buff.c_str(), strlen(buff.c_str()), NULL, NULL);
}

string serial_port::poll_serial_port(void)
{
	DWORD bytes_read = 0;
	
	/*
	 * https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile
	 * 
	 * El tiempo de polleo puede ser grande ya que el kernel del sistema
	 * operativo guarda los Bytes recibidos en un buffer automaticamente.
	 */ 
	if ( !ReadFile(serial_handle, rx_buffer, RX_BUFFER_LEN, &bytes_read, NULL) )
	{
		last_error = "Error trying to read serial port.";
		#if DEBUG_TERMINAL_SERIAL_PORT_WINDOWS 
		cout << "Error trying to read serial port." << endl;
		#endif
		
		return "";
	}
	
	if (bytes_read >= sizeof(rx_buffer))
	{
		bytes_read = sizeof(rx_buffer) - 1;  // Saturate value
	}

	if (bytes_read > 0)	// Si recibió algún Byte
	{
		rx_buffer[bytes_read] = '\0';	// Agrega caracter de fin de cstring
		string str(rx_buffer);		// Convierte a string
		//str += "\n";				// Agrega nueva línea
		
		#if DEBUG_TERMINAL_SERIAL_PORT_WINDOWS 
		cout << "Rx " << bytes_read << " Bytes = " << str << endl;
		#endif
		
		return str;
	}
	else
	{
		return "";
	}
}

void serial_port::start_serial_rx(void)
{
	/*
	 * Método heredado de wxTimer.
	 * 
	 * El tiempo de polleo puede ser grande ya que el kernel del sistema
	 * operativo guarda los Bytes recibidos en un buffer automaticamente.
	 */ 
	Start(RECEIVED_TEXT_CONTROL_REFRESH_TIME, wxTIMER_CONTINUOUS);
}

void serial_port::stop_serial_rx(void)
{
	Stop();	// Método heredado de wxTimer.
}

void serial_port::Notify(void)
{
	/*
	 * El tiempo de polleo puede ser grande ya que el kernel del sistema
	 * operativo guarda los Bytes recibidos en un buffer automaticamente.
	 */ 
	string str = poll_serial_port();
	if (str != "") text_control_received->AppendText(str);
}

string serial_port::get_last_error(void)
{
	return last_error;
}
