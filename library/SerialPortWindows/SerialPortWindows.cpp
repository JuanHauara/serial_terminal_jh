#include "SerialPortWindows.h"


void SerialPort::setSerialPort(string _serialPort)
{
	strcpy(serialPort, _serialPort.c_str());
	cout << "Class SerialPortWindows: _serialPort = " << _serialPort << endl;
	cout << "Class SerialPortWindows: serialPort = " << serialPort << endl;
	cout << "Class SerialPortWindows: strlen(serialPort) = " << strlen(serialPort) << endl;
}

void SerialPort::setBaudRate(uint32_t baudRate)
{
	this->baudRate = baudRate;
}

void SerialPort::setDataBits(int32_t n)
{
	dataBits = n;
}

void SerialPort::setParity(int32_t n)
{
	parity = n;
}

void SerialPort::setStopBits(int32_t n)
{
	stopBits = n;
}

void SerialPort::useDtr(bool flag)
{
	flag_useDtr = flag;
}

void SerialPort::useRts(bool flag)
{
	flag_useRts = flag;
}

string SerialPort::getSerialPort(void)
{
	string _serialPort(serialPort);	// Pasa de cstring a string
	
	return _serialPort;
}

uint32_t SerialPort::getBaudRate(void)
{
	return baudRate;
}

void SerialPort::setInterByteTimeOut(uint32_t ms)
{
	interByteTimeOut = ms;
}

void SerialPort::setTextControl(wxTextCtrl *textControl_received)
{
	this->textControl_received = textControl_received;
}

bool SerialPort::setTimeOuts(void)
{
	COMMTIMEOUTS timeOuts = {0};

	// https://docs.microsoft.com/en-us/windows/win32/api/winbase/ns-winbase-commtimeouts
	timeOuts.ReadIntervalTimeout = MAXDWORD;	// La interfaz gráfica de traba si se pone otro valor que no sea este.
	timeOuts.ReadTotalTimeoutConstant = 0;		// La interfaz gráfica de traba si se pone otro valor que no sea este.
	timeOuts.ReadTotalTimeoutMultiplier = 0;	// La interfaz gráfica de traba si se pone otro valor que no sea este.
	timeOuts.WriteTotalTimeoutConstant = 120;
	timeOuts.WriteTotalTimeoutMultiplier = 120;

	if(SetCommTimeouts(serialHandle, &timeOuts) == 0) 
	{
		lastError = "Error trying to set time outs.";
		#if DEBUG_TERMINAL_SerialPortWindows 
		cout << "Error trying to set time outs." << endl;
		#endif
		
		return false;
	}

	return true;
}

bool SerialPort::setParameters(void)
{
	// Obtiene los parámetros del puerto serie
	DCB serialParams = {0};
	
	serialParams.DCBlength = sizeof(serialParams);	// Esto es necesario por algún extraño capricho de Windows
	
	if (GetCommState(serialHandle, &serialParams) == 0)	// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getcommstate
	{
		lastError = "Error trying to get serial port configuration.";
		#if DEBUG_TERMINAL_SerialPortWindows 
		cout << "Error trying to get serial port configuration." << endl;
		#endif
		disconnect();
		
		return false;
	}
	
	// Parámetros de la estructura DCB: https://docs.microsoft.com/en-us/windows/win32/api/winbase/ns-winbase-dcb
	serialParams.BaudRate = baudRate;
	serialParams.ByteSize = dataBits;
	serialParams.Parity = parity;
	serialParams.StopBits = stopBits;
	
	if (flag_useDtr) 
	{
		serialParams.fDtrControl = 1;
	}
	else 
	{
		serialParams.fDtrControl = 0;
	}
	
	if (flag_useRts)
	{
		serialParams.fRtsControl = 1;
	}
	else 
	{
		serialParams.fRtsControl = 0;
	}

	// Setea los parámetros en el puerto serie
	if (SetCommState(serialHandle, &serialParams) == 0)	// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setcommstate
	{
		lastError = "Error trying to set configuration in the serial port.";
		#if DEBUG_TERMINAL_SerialPortWindows 
		cout << "Error trying to set configuration in the serial port." << endl;
		#endif
		disconnect();
		
		return false;
	}
	
	return true;
}

bool SerialPort::connect(void)
{
	// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
	serialHandle = CreateFileA(serialPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (serialHandle == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			lastError = "Serial port not found.";
			
			#if DEBUG_TERMINAL_SerialPortWindows 
			cout << "Serial port missing." << endl;
			#endif
			
			return false;
		}
		
		lastError = "The serial port is probably being used by another application.";

		#if DEBUG_TERMINAL_SerialPortWindows 
		cout << "The serial port is probably being used by another application." << endl;
		#endif
		
		return false;
	}
	
	if (!setTimeOuts())
	{
		return false;
	}
	
	if (!setParameters())
	{
		return false;
	}
	
	return true;
}

bool SerialPort::disconnect(void)
{
	if (CloseHandle(serialHandle) == 0)	// https://docs.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
	{
		lastError = "Error trying to disconnect.";
		
		return false;
	}
	else
	{
		return true;
	}
}

void SerialPort::txString(string buff)
{
	// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-writefile
	WriteFile(serialHandle, buff.c_str(), strlen(buff.c_str()), NULL, NULL);
}

string SerialPort::pollSerialPort(void)
{
	long unsigned int bytesRead = 0;
	
	/*
	 * https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile
	 * 
	 * El tiempo de polleo puede ser grande ya que el kernel del sistema
	 * operativo guarda los Bytes recibidos en un buffer automaticamente.
	 */ 
	if ( !ReadFile(serialHandle, rxBuffer, RX_BUFFER_LEN, &bytesRead, NULL) )
	{
		lastError = "Error trying to read serial port.";
		#if DEBUG_TERMINAL_SerialPortWindows 
		cout << "Error trying to read serial port." << endl;
		#endif
		
		return "";
	}
	
	if (bytesRead > 0)	// Si recibió algún Byte
	{
		rxBuffer[bytesRead] = '\0';	// Agrega caracter de fin de cstring
		string str(rxBuffer);		// Convierte a string
		//str += "\n";				// Agrega nueva línea
		
		#if DEBUG_TERMINAL_SerialPortWindows 
		cout << "Rx " << bytesRead << " Bytes = " << str << endl;
		#endif
		
		return str;
	}
	else
	{
		return "";
	}
}

void SerialPort::startSerialRx(void)
{
	/*
	 * Método heredado de wxTimer.
	 * 
	 * El tiempo de polleo puede ser grande ya que el kernel del sistema
	 * operativo guarda los Bytes recibidos en un buffer automaticamente.
	 */ 
	Start(RECEIVED_TEXT_CONTROL_REFRESH_TIME, wxTIMER_CONTINUOUS);
}

void SerialPort::stopSerialRx(void)
{
	Stop();	// Método heredado de wxTimer.
}

void SerialPort::Notify(void)
{
	/*
	 * El tiempo de polleo puede ser grande ya que el kernel del sistema
	 * operativo guarda los Bytes recibidos en un buffer automaticamente.
	 */ 
	string str = pollSerialPort();
	if (str != "") textControl_received->AppendText(str);
}

string SerialPort::getLastError(void)
{
	return lastError;
}
