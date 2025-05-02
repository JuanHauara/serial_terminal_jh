#ifndef __SerialPortWindows_h__
#define __SerialPortWindows_h__

#include <wx/wx.h>
#include <wx/timer.h>

#include <windows.h>
#include <fileapi.h>
#include <winbase.h>
#include <iostream>
#include <string>

using namespace std;

#define DEBUG_TERMINAL_SerialPortWindows	false
#define RX_BUFFER_LEN						262144  /* Buffer Rx de 256KB */

/*
 * Tiempo en milisegundos.
 * El tiempo de polleo puede ser grande ya que el kernel del sistema
 * operativo guarda los Bytes recibidos automaticamente en un buffer.
 */ 
#define RECEIVED_TEXT_CONTROL_REFRESH_TIME 10


class SerialPort : public wxTimer
{
	public:
		SerialPort() : wxTimer() {};
		
		// Setters
		void setSerialPort(string _serialPort);
		void setBaudRate(uint32_t baudRate);
		void setDataBits(int32_t n);
		void setParity(int32_t n);
		void setStopBits(int32_t n);
		void useDtr(bool flag);
		void useRts(bool flag);
		void setTextControl(wxTextCtrl *textControl_received);
		void setInterByteTimeOut(uint32_t ms);	// 10ms por defecto.
		
		// Getters
		string getSerialPort(void);
		uint32_t getBaudRate(void);
		
		// Conexión
		bool connect(void);
		bool disconnect(void);
		
		// Enviar y recibir
		void txString(string buff);
		string pollSerialPort(void);
		void startSerialRx(void);
		void stopSerialRx(void);
		
		string getLastError(void);
		
	private:
		char serialPort[10];
		uint32_t baudRate = 0;
		int32_t dataBits = 8;
		int32_t parity = NOPARITY;
		int32_t stopBits = ONESTOPBIT;
		bool flag_useDtr;
		bool flag_useRts;
		uint32_t interByteTimeOut = 10;
		wxTextCtrl *textControl_received;
		HANDLE serialHandle;
		char rxBuffer[RX_BUFFER_LEN];
		string lastError;
		
		bool setTimeOuts(void);
		bool setParameters(void);
		
		void Notify(void);
};

#endif
