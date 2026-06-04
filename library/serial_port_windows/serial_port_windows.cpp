#include "serial_port_windows.h"


void serial_port::set_serial_port(const string &serial_port_name_arg)
{
	serial_port_name = serial_port_name_arg;

	#if DEBUG_TERMINAL_SERIAL_PORT_WINDOWS
	cout << "Class serial_port_windows: serial_port_name_arg = " << serial_port_name_arg << endl;
	cout << "Class serial_port_windows: serial_port_name = " << serial_port_name << endl;
	cout << "Class serial_port_windows: serial_port_name.length() = " << serial_port_name.length() << endl;
	#endif
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

bool serial_port::serial_handle_is_open(void)
{

	return (serial_handle != INVALID_HANDLE_VALUE);
}

bool serial_port::serial_parameters_are_valid(void)
{
	if (baud_rate == 0)
	{
		last_error = "Invalid baud rate.";

		return false;
	}

	if ((data_bits < 5) || (data_bits > 8))
	{
		last_error = "Invalid data bit count.";

		return false;
	}

	if ((parity != NOPARITY) && (parity != EVENPARITY) && (parity != ODDPARITY))
	{
		last_error = "Invalid parity setting.";

		return false;
	}

	if ((stop_bits != ONESTOPBIT) && (stop_bits != ONE5STOPBITS) && (stop_bits != TWOSTOPBITS))
	{
		last_error = "Invalid stop bit setting.";

		return false;
	}

	return true;
}

bool serial_port::set_timeouts(void)
{
	if (!serial_handle_is_open())
	{
		last_error = "Serial port is not open.";

		return false;
	}

	COMMTIMEOUTS timeouts = {0};

	// Required by the non-blocking polling model used by the GUI timer.
	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 120;
	timeouts.WriteTotalTimeoutMultiplier = 120;

	if (SetCommTimeouts(serial_handle, &timeouts) == 0)
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
	if (!serial_handle_is_open())
	{
		last_error = "Serial port is not open.";

		return false;
	}

	if (!serial_parameters_are_valid())
	{

		return false;
	}

	DCB serial_params = {0};

	serial_params.DCBlength = sizeof(serial_params);

	if (GetCommState(serial_handle, &serial_params) == 0)
	{
		last_error = "Error trying to get serial port configuration.";
		#if DEBUG_TERMINAL_SERIAL_PORT_WINDOWS
		cout << "Error trying to get serial port configuration." << endl;
		#endif

		return false;
	}

	serial_params.BaudRate = baud_rate;
	serial_params.ByteSize = data_bits;
	serial_params.Parity = parity;
	serial_params.StopBits = stop_bits;

	if (flag_use_dtr)
	{
		serial_params.fDtrControl = DTR_CONTROL_ENABLE;
	}
	else
	{
		serial_params.fDtrControl = DTR_CONTROL_DISABLE;
	}

	if (flag_use_rts)
	{
		serial_params.fRtsControl = RTS_CONTROL_ENABLE;
	}
	else
	{
		serial_params.fRtsControl = RTS_CONTROL_DISABLE;
	}

	if (SetCommState(serial_handle, &serial_params) == 0)
	{
		last_error = "Error trying to set configuration in the serial port.";
		#if DEBUG_TERMINAL_SERIAL_PORT_WINDOWS
		cout << "Error trying to set configuration in the serial port." << endl;
		#endif

		return false;
	}

	return true;
}

bool serial_port::connect(void)
{
	if (serial_handle_is_open())
	{
		last_error = "Serial port is already open.";

		return false;
	}

	if (serial_port_name.empty())
	{
		last_error = "Serial port name is empty.";

		return false;
	}

	last_error.clear();

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
		disconnect();

		return false;
	}

	if (!set_parameters())
	{
		disconnect();

		return false;
	}

	return true;
}

bool serial_port::disconnect(void)
{
	stop_serial_rx();

	if (!serial_handle_is_open())
	{

		return true;
	}

	if (CloseHandle(serial_handle) == 0)
	{
		last_error = "Error trying to disconnect.";

		return false;
	}

	serial_handle = INVALID_HANDLE_VALUE;

	return true;
}

bool serial_port::tx_string(const string &buff)
{
	if (!serial_handle_is_open())
	{
		last_error = "Serial port is not open.";

		return false;
	}

	if (buff.empty())
	{

		return true;
	}

	if (buff.size() > MAXDWORD)
	{
		last_error = "Transmit buffer is too large.";

		return false;
	}

	DWORD bytes_to_write = static_cast<DWORD>(buff.size());
	DWORD bytes_written = 0;

	if (WriteFile(serial_handle, buff.data(), bytes_to_write, &bytes_written, NULL) == 0)
	{
		last_error = "Error trying to write serial port.";

		return false;
	}

	if (bytes_written != bytes_to_write)
	{
		last_error = "Incomplete serial port write.";

		return false;
	}

	return true;
}

string serial_port::poll_serial_port(void)
{
	DWORD bytes_read = 0;

	if (!serial_handle_is_open())
	{
		last_error = "Serial port is not open.";

		return "";
	}

	if (ReadFile(serial_handle, rx_buffer, RX_BUFFER_LEN, &bytes_read, NULL) == 0)
	{
		last_error = "Error trying to read serial port.";
		#if DEBUG_TERMINAL_SERIAL_PORT_WINDOWS
		cout << "Error trying to read serial port." << endl;
		#endif

		return "";
	}

	if (bytes_read >= sizeof(rx_buffer))
	{
		bytes_read = sizeof(rx_buffer);
	}

	if (bytes_read > 0)
	{
		string str(rx_buffer, bytes_read);

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
	if (!serial_handle_is_open())
	{
		last_error = "Serial port is not open.";

		return;
	}

	Start(RECEIVED_TEXT_CONTROL_REFRESH_TIME, wxTIMER_CONTINUOUS);
}

void serial_port::stop_serial_rx(void)
{
	if (IsRunning())
	{
		Stop();
	}
}

void serial_port::Notify(void)
{
	if (text_control_received == NULL)
	{
		last_error = "Received text control is not set.";
		stop_serial_rx();

		return;
	}

	string str = poll_serial_port();

	if (str != "")
	{
		text_control_received->AppendText(str);
	}
}

string serial_port::get_last_error(void)
{

	return last_error;
}
