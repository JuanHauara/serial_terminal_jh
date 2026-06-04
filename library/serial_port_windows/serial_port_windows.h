#ifndef SERIAL_PORT_WINDOWS_H
#define SERIAL_PORT_WINDOWS_H

#include <wx/wx.h>
#include <wx/timer.h>

#include <windows.h>
#include <fileapi.h>
#include <winbase.h>
#include <iostream>
#include <string>

using namespace std;

#define DEBUG_TERMINAL_SERIAL_PORT_WINDOWS	false
#define RX_BUFFER_LEN						262144  /* Buffer Rx de 256KB */

/*
 * Tiempo en milisegundos.
 * El tiempo de polleo puede ser grande ya que el kernel del sistema
 * operativo guarda los Bytes recibidos automaticamente en un buffer.
 */ 
#define RECEIVED_TEXT_CONTROL_REFRESH_TIME 10


class serial_port : public wxTimer
{
	public:
		serial_port() : wxTimer() {};
		
		// Setters
		void set_serial_port(string serial_port_name_arg);
		void set_baud_rate(uint32_t baud_rate);
		void set_data_bits(int32_t n);
		void set_parity(int32_t n);
		void set_stop_bits(int32_t n);
		void use_dtr(bool flag);
		void use_rts(bool flag);
		void set_text_control(wxTextCtrl *text_control_received);
		void set_inter_byte_timeout(uint32_t ms);	// 10ms por defecto.
		
		// Getters
		string get_serial_port(void);
		uint32_t get_baud_rate(void);
		
		// Conexión
		bool connect(void);
		bool disconnect(void);
		
		// Enviar y recibir
		void tx_string(string buff);
		string poll_serial_port(void);
		void start_serial_rx(void);
		void stop_serial_rx(void);
		
		string get_last_error(void);
		
	private:
		string serial_port_name;
		uint32_t baud_rate = 0;
		int32_t data_bits = 8;
		int32_t parity = NOPARITY;
		int32_t stop_bits = ONESTOPBIT;
		bool flag_use_dtr;
		bool flag_use_rts;
		uint32_t inter_byte_timeout = 10;
		wxTextCtrl *text_control_received;
		HANDLE serial_handle;
		char rx_buffer[RX_BUFFER_LEN];
		string last_error;
		
		bool set_timeouts(void);
		bool set_parameters(void);
		
		void Notify(void);
};

#endif
