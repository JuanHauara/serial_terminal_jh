/*
 ----------------------------------------------------------------------------

	Author: Juan Hauara
	Date: 05/03/2022
	Contact: neural_sensors@proton.me

 ----------------------------------------------------------------------------
*/


#include <wx/wx.h>

#include <iostream>
#include <string>
#include <list>
#include <chrono>

#include "persistent_data.h"	// Persistent variables stored in a disk file.


using namespace std;
using namespace chrono;


//==================================================================
// Settings
//==================================================================

#define DEBUG_LOG			0
#define LINUX_PLATFORM		0
#define WINDOWS_PLATFORM	1

#if LINUX_PLATFORM
#elif WINDOWS_PLATFORM
	#include "serial_port_windows.h"
#endif

const wxString software_version("v0.1.3");
const wxString window_title("Serial Terminal");
const wxString email_contact("juanhauara@gmail.com");
const wxString web_contact("");

const wxSize window_size = wxSize(550, 550);
const wxColour windows_color = wxColour(233, 233, 233);

#define DEFAULT_BAUD_RATE	9600
#define DEFAULT_DATA_BITS	8			/* 5, 6, 7, 8 */
#define DEFAULT_PARITY		NOPARITY	/* EVENPARITY, ODDPARITY */
#define DEFAULT_STOP_BITS	ONESTOPBIT	/* ONE5STOPBITS, TWOSTOPBITS */
#define DEFAULT_USE_DTR		false
#define DEFAULT_USE_RTS		false
#define DEFAULT_ADD_NL		false		/* Add newline character '\n' (0x0A in hexadecimal) at the end of the message before sending it. */
#define DEFAULT_ADD_CR		false		/* Add carriage return character '\r' (0x0D in hexadecimal) at the end of the message before sending it. */

//==================================================================


// Defines
#define WXT_NO_PARITY		wxT("No parity")
#define WXT_EVEN_PARITY		wxT("Even parity")
#define WXT_ODD_PARITY		wxT("Odd parity")

#define WXT_1_STOP_BIT		wxT("1 stop bit")
#define WXT_15_STOP_BITS	wxT("1.5 stop bits")
#define WXT_2_STOP_BITS		wxT("2 stop bits")


// Global variables.
bool menu_settings_up = false;


//==================================================================
// Main frame class.
//==================================================================
class frame_menu_settings : public wxFrame
{
	public:
		frame_menu_settings(wxWindow *parent);

	private:
		// Widgets
		wxComboBox *combo_box_data_bits;
		wxComboBox *combo_box_parity;
		wxComboBox *combo_box_stop_bits;
		wxCheckBox *check_box_use_dtr;
		wxCheckBox *check_box_use_rts;
		wxCheckBox *check_box_add_nl;
		wxCheckBox *check_box_add_cr;
		wxButton *button_default;
		wxButton *button_save;

		// Variables.
		int32_t data_bits;
		int32_t parity;
		int32_t stop_bits;
		bool use_dtr;
		bool use_rts;
		bool add_nl;
		bool add_cr;

		persistent_data *persistent_store = new persistent_data("persistent_data.dat");

		// Methods.
		void set_values_on_widgets(void);

		// Event handlers
		void on_close(wxCloseEvent &event);
		void on_combo_box_data_bits(wxCommandEvent &event);
		void on_combo_box_parity(wxCommandEvent &event);
		void on_combo_box_stop_bits(wxCommandEvent &event);
		void on_check_box_use_dtr(wxCommandEvent &event);
		void on_check_box_use_rts(wxCommandEvent &event);
		void on_check_box_add_nl(wxCommandEvent &event);
		void on_check_box_add_cr(wxCommandEvent &event);
		void on_click_default(wxCommandEvent &event);
		void on_click_save(wxCommandEvent &event);
};

frame_menu_settings::frame_menu_settings(wxWindow *parent) : wxFrame(parent, wxID_ANY, wxT("Serial port settings"),
wxDefaultPosition, wxSize(240, 370), wxCAPTION | wxCLOSE_BOX | wxFRAME_FLOAT_ON_PARENT)
{
	CentreOnScreen();

	// Closing the window calls on_close().
	Bind(wxEVT_CLOSE_WINDOW, &frame_menu_settings::on_close, this);

	wxPanel *panel = new wxPanel(this, wxID_ANY);
	panel->SetBackgroundColour(windows_color);


	// Widget setup.
	// Static texts.
	wxStaticText *static_text_data_bits = new wxStaticText(panel, wxID_ANY, wxT("Data bits:"));
	wxStaticText *static_text_parity = new wxStaticText(panel, wxID_ANY, wxT("Parity:"));
	wxStaticText *static_text_stop_bits = new wxStaticText(panel, wxID_ANY, wxT("Stop bits:"));
	wxStaticText *static_text_flow_control = new wxStaticText(panel, wxID_ANY, wxT("Flow control:"));
	wxStaticText *static_text_add_to_message = new wxStaticText(panel, wxID_ANY, wxT("Add to message:"));


	// comboBox data bits
	wxArrayString data_bits_choices;
	data_bits_choices.Add(wxT("5"));	// Do not change this value.
	data_bits_choices.Add(wxT("6"));	// Do not change this value.
	data_bits_choices.Add(wxT("7"));	// Do not change this value.
	data_bits_choices.Add(wxT("8"));	// Do not change this value.
	combo_box_data_bits = new wxComboBox(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(95, -1), data_bits_choices, wxCB_READONLY);
	combo_box_data_bits->Bind(wxEVT_COMBOBOX, &frame_menu_settings::on_combo_box_data_bits, this);

	// Parity combo box.
	wxArrayString parity_choices;
	parity_choices.Add(WXT_NO_PARITY);
	parity_choices.Add(WXT_EVEN_PARITY);
	parity_choices.Add(WXT_ODD_PARITY);
	combo_box_parity = new wxComboBox(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(95, -1), parity_choices, wxCB_READONLY);
	combo_box_parity->Bind(wxEVT_COMBOBOX, &frame_menu_settings::on_combo_box_parity, this);

	// comboBox stop bits
	wxArrayString stop_bits_choices;
	stop_bits_choices.Add(WXT_1_STOP_BIT);
	stop_bits_choices.Add(WXT_15_STOP_BITS);
	stop_bits_choices.Add(WXT_2_STOP_BITS);
	combo_box_stop_bits = new wxComboBox(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(95, -1), stop_bits_choices, wxCB_READONLY);
	combo_box_stop_bits->Bind(wxEVT_COMBOBOX, &frame_menu_settings::on_combo_box_stop_bits, this);


	// DTR check box.
	check_box_use_dtr = new wxCheckBox(panel, wxID_ANY, wxT("enable DTR"), wxDefaultPosition, wxDefaultSize);
	check_box_use_dtr->Bind(wxEVT_CHECKBOX, &frame_menu_settings::on_check_box_use_dtr, this);

	// RTS check box.
	check_box_use_rts = new wxCheckBox(panel, wxID_ANY, wxT("enable RTS"), wxDefaultPosition, wxDefaultSize);
	check_box_use_rts->Bind(wxEVT_CHECKBOX, &frame_menu_settings::on_check_box_use_rts, this);


	// Check box add new line
	check_box_add_nl = new wxCheckBox(panel, wxID_ANY, wxT("add new line"), wxDefaultPosition, wxDefaultSize);
	check_box_add_nl->Bind(wxEVT_CHECKBOX, &frame_menu_settings::on_check_box_add_nl, this);

	// Check box add carriage return.
	check_box_add_cr = new wxCheckBox(panel, wxID_ANY, wxT("add carriage return"), wxDefaultPosition, wxDefaultSize);
	check_box_add_cr->Bind(wxEVT_CHECKBOX, &frame_menu_settings::on_check_box_add_cr, this);


	// Default button.
	button_default = new wxButton(panel, wxID_ANY, wxT("Default"), wxDefaultPosition, wxDefaultSize);
	button_default->Bind(wxEVT_BUTTON, &frame_menu_settings::on_click_default, this);

	// Save button.
	button_save = new wxButton(panel, wxID_ANY, wxT("Save"), wxDefaultPosition, wxDefaultSize);
	button_save->Bind(wxEVT_BUTTON, &frame_menu_settings::on_click_save, this);


	// GUI layout creation.
	///////////////////////////////////////////////////////////////////
	// Sizers
	wxBoxSizer *v_box_sizer_main = new wxBoxSizer(wxVERTICAL);			// Main vertical sizer.
	wxFlexGridSizer *flex_grid_sizer = new wxFlexGridSizer(2, 10, 10);	// Text and combo box sizer.
	wxBoxSizer *h_box_sizer_buttons = new wxBoxSizer(wxHORIZONTAL);		// Button horizontal sizer.

	flex_grid_sizer->Add(static_text_data_bits, 0);
	flex_grid_sizer->Add(combo_box_data_bits, 0);
	flex_grid_sizer->Add(static_text_parity, 0);
	flex_grid_sizer->Add(combo_box_parity, 0);
	flex_grid_sizer->Add(static_text_stop_bits, 0);
	flex_grid_sizer->Add(combo_box_stop_bits, 0);
	v_box_sizer_main->Add(flex_grid_sizer, 0, wxALL, 23);

	v_box_sizer_main->Add(static_text_flow_control, 0, wxLEFT, 23);
	v_box_sizer_main->AddSpacer(5);
	v_box_sizer_main->Add(check_box_use_dtr, 0, wxLEFT, 50);
	v_box_sizer_main->AddSpacer(5);
	v_box_sizer_main->Add(check_box_use_rts, 0, wxLEFT, 50);

	v_box_sizer_main->AddSpacer(23);
	v_box_sizer_main->Add(static_text_add_to_message, 0, wxLEFT, 23);
	v_box_sizer_main->AddSpacer(5);
	v_box_sizer_main->Add(check_box_add_nl, 0, wxLEFT, 50);
	v_box_sizer_main->AddSpacer(5);
	v_box_sizer_main->Add(check_box_add_cr, 0, wxLEFT, 50);

	v_box_sizer_main->AddSpacer(27);
	h_box_sizer_buttons->Add(button_default, 0, wxLEFT, 13);
	h_box_sizer_buttons->AddSpacer(56);			// 50 pixels between buttons.
	h_box_sizer_buttons->Add(button_save, 0);
	v_box_sizer_main->Add(h_box_sizer_buttons, 0);

	panel->SetSizer(v_box_sizer_main, wxEXPAND);	// Main vertical sizer in the panel.

	// Update the GUI with persistent values from disk.
	///////////////////////////////////////////////////////////////////
	if (persistent_store->load_data())
	{
		data_bits = persistent_store->get_int("dataBits");
		parity = persistent_store->get_int("parity");
		stop_bits = persistent_store->get_int("stopBits");
		use_dtr = persistent_store->get_bool("useDtr");
		use_rts = persistent_store->get_bool("useRts");
		add_nl = persistent_store->get_bool("add_nl");
		add_cr = persistent_store->get_bool("add_cr");
	}
	else
	{
		/*
		If this is the first program start, or if persistent_data.dat was
		not found, set the default values and try to create the file.
		*/
		data_bits = DEFAULT_DATA_BITS;
		parity = DEFAULT_PARITY;
		stop_bits = DEFAULT_STOP_BITS;
		use_dtr = DEFAULT_USE_DTR;
		use_rts = DEFAULT_USE_RTS;
		add_nl = DEFAULT_ADD_NL;
		add_cr = DEFAULT_ADD_CR;

		// Save values to disk.
		persistent_store->set_int("dataBits", data_bits);
		persistent_store->set_int("parity", parity);
		persistent_store->set_int("stopBits", stop_bits);
		persistent_store->set_bool("useDtr", use_dtr);
		persistent_store->set_bool("useRts", use_rts);
		persistent_store->set_bool("add_nl", add_nl);
		persistent_store->set_bool("add_cr", add_cr);
	}

	set_values_on_widgets();

	#if DEBUG_LOG
	cout << "\nData bits = " << data_bits << endl;
	cout << "Parity = " << parity << endl;
	cout << "Stop bits = " << stop_bits << endl;
	cout << "Use DTR = " << use_dtr << endl;
	cout << "Use RTS = " << use_rts << endl;
	cout << "Add new line = " << add_nl << endl;
	cout << "Add carriage return = " << add_cr << endl;
	#endif
}

void frame_menu_settings::set_values_on_widgets(void)
{
	if (data_bits == 5)
	{
		combo_box_data_bits->SetValue(wxT("5"));
	}
	else if (data_bits == 6)
	{
		combo_box_data_bits->SetValue(wxT("6"));
	}
	else if (data_bits == 7)
	{
		combo_box_data_bits->SetValue(wxT("7"));
	}
	else if (data_bits == 8)
	{
		combo_box_data_bits->SetValue(wxT("8"));
	}

	if (parity == NOPARITY)
	{
		combo_box_parity->SetValue(WXT_NO_PARITY);
	}
	else if (parity == EVENPARITY)
	{
		combo_box_parity->SetValue(WXT_EVEN_PARITY);
	}
	else if (parity == ODDPARITY)
	{
		combo_box_parity->SetValue(WXT_ODD_PARITY);
	}

	if (stop_bits == ONESTOPBIT)
	{
		combo_box_stop_bits->SetValue(WXT_1_STOP_BIT);
	}
	else if (stop_bits == ONE5STOPBITS)
	{
		combo_box_stop_bits->SetValue(WXT_15_STOP_BITS);
	}
	else if (stop_bits == TWOSTOPBITS)
	{
		combo_box_stop_bits->SetValue(WXT_2_STOP_BITS);
	}

	check_box_use_dtr->SetValue(use_dtr);
	check_box_use_rts->SetValue(use_rts);
	check_box_add_nl->SetValue(add_nl);
	check_box_add_cr->SetValue(add_cr);
}

void frame_menu_settings::on_close(wxCloseEvent &event)
{
	if (event.CanVeto())
	{
		menu_settings_up = false;
	}

	Destroy();
}

void frame_menu_settings::on_combo_box_data_bits(wxCommandEvent &event)
{
	wxString wxstr = combo_box_data_bits->GetStringSelection();
	data_bits = wxAtoi(wxstr);	// Convert from wxString to integer.

	#if DEBUG_LOG
	cout << "Data bits = " << data_bits << endl;
	#endif
}

void frame_menu_settings::on_combo_box_parity(wxCommandEvent &event)
{
	wxString wxstr = combo_box_parity->GetStringSelection();

	if (wxstr == WXT_NO_PARITY)
	{
		parity = NOPARITY;
	}
	else if (wxstr == WXT_EVEN_PARITY)
	{
		parity = EVENPARITY;
	}
	else if (wxstr == WXT_ODD_PARITY)
	{
		parity = ODDPARITY;
	}

	#if DEBUG_LOG
	cout << "Parity = " << parity << endl;
	#endif
}

void frame_menu_settings::on_combo_box_stop_bits(wxCommandEvent &event)
{
	wxString wxstr = combo_box_stop_bits->GetStringSelection();

	if (wxstr == WXT_1_STOP_BIT)
	{
		stop_bits = ONESTOPBIT;
	}
	else if (wxstr == WXT_15_STOP_BITS)
	{
		stop_bits = ONE5STOPBITS;
	}
	else if (wxstr == WXT_2_STOP_BITS)
	{
		stop_bits = TWOSTOPBITS;
	}

	#if DEBUG_LOG
	cout << "Stop bits = " << stop_bits << endl;
	#endif
}

void frame_menu_settings::on_check_box_use_dtr(wxCommandEvent &event)
{
	use_dtr = check_box_use_dtr->GetValue();

	#if DEBUG_LOG
	cout << "Use DTR = " << use_dtr << endl;
	#endif
}

void frame_menu_settings::on_check_box_use_rts(wxCommandEvent &event)
{
	use_rts = check_box_use_rts->GetValue();

	#if DEBUG_LOG
	cout << "Use RTS = " << use_rts << endl;
	#endif
}

void frame_menu_settings::on_check_box_add_nl(wxCommandEvent &event)
{
	add_nl = check_box_add_nl->GetValue();

	#if DEBUG_LOG
	cout << "Add new line = " << add_nl << endl;
	#endif
}

void frame_menu_settings::on_check_box_add_cr(wxCommandEvent &event)
{
	add_cr = check_box_add_cr->GetValue();

	#if DEBUG_LOG
	cout << "Add carriage return = " << add_cr << endl;
	#endif
}

void frame_menu_settings::on_click_default(wxCommandEvent &event)
{
	data_bits = DEFAULT_DATA_BITS;
	parity = DEFAULT_PARITY;
	stop_bits = DEFAULT_STOP_BITS;
	use_dtr = DEFAULT_USE_DTR;
	use_rts = DEFAULT_USE_RTS;

	set_values_on_widgets();

	#if DEBUG_LOG
	cout << "\nData bits = " << data_bits << endl;
	cout << "Parity = " << parity << endl;
	cout << "Stop bits = " << stop_bits << endl;
	cout << "Use DTR = " << use_dtr << endl;
	cout << "Use RTS = " << use_rts << endl;
	cout << "Add new line = " << add_nl << endl;
	cout << "Add carriage return = " << add_cr << endl;
	#endif
}

void frame_menu_settings::on_click_save(wxCommandEvent &event)
{
	// Save values to disk.
	persistent_store->set_int("dataBits", data_bits);
	persistent_store->set_int("parity", parity);
	persistent_store->set_int("stopBits", stop_bits);
	persistent_store->set_bool("useDtr", use_dtr);
	persistent_store->set_bool("useRts", use_rts);
	persistent_store->set_bool("add_nl", add_nl);
	persistent_store->set_bool("add_cr", add_cr);

	menu_settings_up = false;

	#if DEBUG_LOG
	cout << "\nData bits = " << data_bits << endl;
	cout << "Parity = " << parity << endl;
	cout << "Stop bits = " << stop_bits << endl;
	cout << "Use DTR = " << use_dtr << endl;
	cout << "Use RTS = " << use_rts << endl;
	cout << "Add new line = " << add_nl << endl;
	cout << "Add carriage return = " << add_cr << endl;
	#endif

	Destroy();	// Close this window.
}

//==================================================================
// Main frame class.
//==================================================================
class frame_main : public wxFrame
{
	public:
		frame_main(const wxString &title, const wxSize size);

	private:
		// Widgets
		wxComboBox *combo_box_serial_port;
		wxComboBox *combo_box_baud_rate;
		wxButton *button_connect_disconnect;
		wxButton *button_clear_received;
		wxTextCtrl *text_ctrl_received;
		wxTextCtrl *text_ctrl_send;
		wxButton *button_send;
		//wxTimer *timer_serial_rx;
		// Menu.
		wxMenuBar *menu_bar;
		wxMenu *edit_menu;
		wxMenu *help_menu;

		// Variables.
		bool connected = false;

		bool add_nl;
		bool add_cr;

		wxArrayString serial_port_choices;
		wxArrayString baud_choices;

		serial_port *serial_port_driver = new serial_port();
		frame_menu_settings *frame_menu_settings_window;

		// Persistent variables saved to disk.
		//persistent_data *persistent_store = new persistent_data("persistent_data.dat");
		persistent_data persistent_store = persistent_data("persistent_data.dat");

		// Methods.
		void connect_disconnect(void);
		list<int> get_available_com_ports(void);
		void add_serial_ports_to_combo_box(list<int> com_ports);

		// Event handlers
		void on_close(wxCloseEvent &event);
		void on_combo_box_drop_down_serial_port(wxCommandEvent &event);
		void on_combo_box_close_up_serial_port(wxCommandEvent &event);
		void on_combo_box_baud_rate(wxCommandEvent &event);
		void on_click_connect_disconnect(wxCommandEvent &event);
		void on_click_clear_received(wxCommandEvent &event);
		void on_click_send(wxCommandEvent &event);
		// Menu.
		void on_menu_settings(wxCommandEvent &event);
		void on_menu_about(wxCommandEvent &event);
		//void on_timer_serial_rx(wxTimerEvent &event);
};

// Constructor.
frame_main::frame_main(const wxString &title, const wxSize size) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, size)
{
	CentreOnScreen();
	//Maximize();

	// Closing the window calls on_close().
	Bind(wxEVT_CLOSE_WINDOW, &frame_main::on_close, this);

	// Menu bar.
	//////////////////////////////////////////////////////////////////////////////
	menu_bar = new wxMenuBar();
	//-------------
	// Edit menu.
	edit_menu = new wxMenu();

	edit_menu->Append(wxID_PROPERTIES, wxT("&Settings"));
	Bind(wxEVT_MENU, &frame_main::on_menu_settings, this, wxID_PROPERTIES);

	menu_bar->Append(edit_menu, wxT("&Edit"));
	//-------------
	// Help menu.
	help_menu = new wxMenu();

	help_menu->Append(wxID_OPEN, wxT("&About"));
	Bind(wxEVT_MENU, &frame_main::on_menu_about, this, wxID_OPEN);

	menu_bar->Append(help_menu, wxT("&Help"));
	//-------------
	SetMenuBar(menu_bar);
	//////////////////////////////////////////////////////////////////////////////

	// Panels.
	//////////////////////////////////////////////////////////////////////////////
	// Main frame panel.
	wxPanel *panel_main = new wxPanel(this, wxID_ANY);
	panel_main->SetBackgroundColour(windows_color);
	//////////////////////////////////////////////////////////////////////////////

	// Fonts.
	// See the "wxFontInfo" class documentation for the different font options:
	// https://docs.wxwidgets.org/trunk/classwx_font_info.html#a74ff2d0449e75bcafc1dd4695f97ab66)
	//////////////////////////////////////////////////////////////////////////////
	wxFont font_received_text(wxFontInfo(11).FaceName("Courier New").Light().AntiAliased(true));	// Text control font.
	//////////////////////////////////////////////////////////////////////////////

	// Initialize widgets.
	//////////////////////////////////////////////////////////////////////////////
	// Serial port combo box text.
	wxStaticText *static_text_serial_port = new wxStaticText(panel_main, wxID_ANY, wxT("Serial port:"));
	//static_text_serial_port->SetFont(fontTitle);

	// Serial port combo box.
	combo_box_serial_port = new wxComboBox(panel_main, wxID_ANY, wxEmptyString, wxDefaultPosition,
									  wxSize(70, -1), serial_port_choices, wxCB_READONLY);
	//combo_box_serial_port->SetFont(fontCommon);
	combo_box_serial_port->Bind(wxEVT_COMBOBOX_DROPDOWN, &frame_main::on_combo_box_drop_down_serial_port, this); // Event generated when the combo box opens.
	combo_box_serial_port->Bind(wxEVT_COMBOBOX_CLOSEUP, &frame_main::on_combo_box_close_up_serial_port, this);   // Event generated when the combo box closes.

	// Baud rate combo box text.
	wxStaticText *static_text_baud_rate = new wxStaticText(panel_main, wxID_ANY, wxT("Baud:"));
	//static_text_baud_rate->SetFont(fontTitle);

	// comboBox baud rate
	baud_choices.Add(wxT("110"));
	baud_choices.Add(wxT("300"));
	baud_choices.Add(wxT("600"));
	baud_choices.Add(wxT("1200"));
	baud_choices.Add(wxT("2400"));
	baud_choices.Add(wxT("4800"));
	baud_choices.Add(wxT("9600"));
	baud_choices.Add(wxT("14400"));
	baud_choices.Add(wxT("19200"));
	baud_choices.Add(wxT("38400"));
	baud_choices.Add(wxT("57600"));
	baud_choices.Add(wxT("115200"));
	baud_choices.Add(wxT("128000"));
	baud_choices.Add(wxT("256000"));
	combo_box_baud_rate = new wxComboBox(panel_main, wxID_ANY, wxEmptyString, wxDefaultPosition,
									   wxSize(70, -1), baud_choices, wxCB_READONLY);
	//combo_box_baud_rate->SetFont(fontCommon);
	combo_box_baud_rate->Bind(wxEVT_COMBOBOX, &frame_main::on_combo_box_baud_rate, this);

	// Connect/disconnect button.
	button_connect_disconnect = new wxButton(panel_main, wxID_ANY, wxT("Connect"), wxDefaultPosition, wxDefaultSize);	// wxSize(90, 25)
	//button_connect_disconnect->SetFont(fontButton);
	button_connect_disconnect->Bind(wxEVT_BUTTON, &frame_main::on_click_connect_disconnect, this);

	// Clear received messages button.
	button_clear_received = new wxButton(panel_main, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize);	// wxSize(70, 25)
	//button_clear_received->SetFont(fontButton);
	button_clear_received->Bind(wxEVT_BUTTON, &frame_main::on_click_clear_received, this);

	// Received messages text control.
	text_ctrl_received = new wxTextCtrl(panel_main, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
	text_ctrl_received->SetFont(font_received_text);

	// Sent message text control.
	text_ctrl_send = new wxTextCtrl(panel_main, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	text_ctrl_send->SetFont(font_received_text);

	// Send button.
	button_send = new wxButton(panel_main, wxID_ANY, wxT("Send"), wxDefaultPosition, wxDefaultSize);	// wxSize(70, 25)
	//button_send->SetFont(fontButton);
	button_send->Bind(wxEVT_BUTTON, &frame_main::on_click_send, this);
	//////////////////////////////////////////////////////////////////////////////

	// GUI layout creation.
	//////////////////////////////////////////////////////////////////////////////
	// Sizers
	wxBoxSizer *v_box_main = new wxBoxSizer(wxVERTICAL);													// Main vertical sizer.
	//wxStaticBoxSizer *h_box_top = new wxStaticBoxSizer(wxHORIZONTAL, panel_main, wxString("Settings"));	// Top horizontal sizer.
	wxBoxSizer *h_box_top = new wxBoxSizer(wxHORIZONTAL);													// Top horizontal sizer.
	wxBoxSizer *h_box_bottom = new wxBoxSizer(wxHORIZONTAL);												// Bottom horizontal sizer.

	v_box_main->AddSpacer(7);	// 7 pixel margin between the menu and widgets.

	// Add widgets from the top vertical sizer.
	// --------------
	h_box_top->Add(static_text_serial_port, 0, wxALIGN_CENTRE);		// Vertically center the widget.
	h_box_top->AddSpacer(4);										// 4 pixels between the text and the combo box.
	h_box_top->Add(combo_box_serial_port, 0, wxALIGN_CENTRE);		// Vertically center the widget.
	h_box_top->AddSpacer(20);										// 20 pixels between the combo box and the baud rate text.

	h_box_top->Add(static_text_baud_rate, 0, wxALIGN_CENTRE);		// Vertically center the widget.
	h_box_top->AddSpacer(4);										// 4 pixels between the text and the combo box.
	h_box_top->Add(combo_box_baud_rate, 0, wxALIGN_CENTRE);			// Vertically center the widget.
	h_box_top->AddSpacer(20);										// 20 pixels between the combo box and the connect button.

	h_box_top->Add(button_connect_disconnect, 0, wxALIGN_CENTRE);	// Vertically center the widget.

	h_box_top->AddSpacer(90);										// 90 pixels between the connect and clear buttons.
	h_box_top->Add(button_clear_received, 0, wxALIGN_CENTRE);		// Vertically center the widget.
	// --------------
	v_box_main->Add(h_box_top, 0, wxALL, 4);	// Add the top horizontal sizer to the main vertical sizer with a 4 pixel margin.

	// Received message text.
	// --------------
	/*
	The 1 in the second parameter and the wxEXPAND flag in the third
	parameter expand the text control over the available window space.
	It has a 4 pixel margin on every side.
	*/
	v_box_main->Add(text_ctrl_received, 1, wxEXPAND | wxALL, 4);
	// --------------

	// Add widgets from the bottom vertical sizer.
	// --------------
	/*
	The 1 in the second parameter expands the control over the available
	window space. It is vertically centered.
	*/
	h_box_bottom->Add(text_ctrl_send, 1, wxALIGN_CENTRE);
	h_box_bottom->AddSpacer(5);
	h_box_bottom->Add(button_send, 0, wxALIGN_CENTRE);	// Vertically centered.
	// --------------
	v_box_main->Add(h_box_bottom, 0, wxEXPAND | wxALL, 4);	// Add the bottom horizontal sizer to the main vertical sizer.
	v_box_main->AddSpacer(2);

	panel_main->SetSizer(v_box_main, wxEXPAND);	// Main vertical sizer in the main panel.
	//////////////////////////////////////////////////////////////////////////////

	// Timer used to update the received message text control.
	//timer_serial_rx = new wxTimer(this, wxID_ANY);
	//timer_serial_rx = new wxTimer(this);
	//timer_serial_rx->Bind(wxEVT_TIMER, &frame_main::on_timer_serial_rx, this);	// Event generated when the timer expires.
	//timer_serial_rx->Bind(wxEVT_TIMER, &frame_main::on_timer_serial_rx, this, timer_serial_rx->GetId());

	// Update the GUI with persistent variables from disk.
	//////////////////////////////////////////////////////////////////////////////
	string selected_serial_port;
	int32_t baud_rate;

	if (persistent_store.load_data())	// If the file exists.
	{
		selected_serial_port = persistent_store.get_string("selected_serialPort");
		baud_rate = persistent_store.get_int("baudRate");
		add_nl = persistent_store.get_bool("add_nl");
		add_cr = persistent_store.get_bool("add_cr");
	}
	else
	{
		/*
		If this is the first program start, or if persistent_data.dat was
		not found, set the default values and try to create the file.
		*/
		selected_serial_port = "";
		baud_rate = DEFAULT_BAUD_RATE;
		add_nl = DEFAULT_ADD_NL;
		add_cr = DEFAULT_ADD_CR;

		// Save values to disk.
		persistent_store.set_string("selected_serialPort", selected_serial_port);
		persistent_store.set_int("baudRate", DEFAULT_BAUD_RATE);
		persistent_store.set_int("dataBits", DEFAULT_DATA_BITS);
		persistent_store.set_int("parity", DEFAULT_PARITY);
		persistent_store.set_int("stopBits", DEFAULT_STOP_BITS);
		persistent_store.set_bool("useDtr", DEFAULT_USE_DTR);
		persistent_store.set_bool("useRts", DEFAULT_USE_RTS);
		persistent_store.set_bool("add_nl", DEFAULT_ADD_NL);
		persistent_store.set_bool("add_cr", DEFAULT_ADD_CR);
	}

	#if DEBUG_LOG
	cout << "\nserial port = " << persistent_store.get_string("selected_serialPort") << endl;
	cout << "baud rate = " << persistent_store.get_int("baudRate") << " baud\n";
	cout << "Data bits = " << persistent_store.get_int("dataBits") << endl;
	cout << "Parity = " << persistent_store.get_int("parity") << endl;
	cout << "Stop bits = " << persistent_store.get_int("stopBits") << endl;
	cout << "Use DTR = " << persistent_store.get_bool("useDtr") << endl;
	cout << "Use RTS = " << persistent_store.get_bool("useRts") << endl;
	cout << "Add new line = " << persistent_store.get_bool("add_nl") << endl;
	cout << "Add carriage return = " << persistent_store.get_bool("add_cr") << endl;
	#endif

	list<int> com_ports = get_available_com_ports();	// Get the available COM port numbers.
	if (!com_ports.empty())
	{
		add_serial_ports_to_combo_box(com_ports);			// Add the available serial ports to the combo box.
	}

	wxString str;
	str << selected_serial_port;
	combo_box_serial_port->SetValue(str);

	str.clear();
	str << baud_rate;
	combo_box_baud_rate->SetValue(str);
	//////////////////////////////////////////////////////////////////////////////

	// Set the text control where received messages are displayed.
	serial_port_driver->set_text_control(text_ctrl_received);
}

/*
void frame_main::on_timer_serial_rx(wxTimerEvent &event)
{
	text_ctrl_received->AppendText(serial_port_driver->poll_serial_port());
	text_ctrl_received->AppendText("\n");
	cout << "Hello world.\n";
}
*/

void frame_main::on_close(wxCloseEvent &event)
{
	if (event.CanVeto())
	{
		if (connected)
		{
			connect_disconnect();
		}
	}

	Destroy();
}

void frame_main::on_menu_settings(wxCommandEvent &event)
{
	if (!menu_settings_up)
	{
		if (connected)
		{
			connect_disconnect();
		}

		menu_settings_up = true;

		frame_menu_settings_window = new frame_menu_settings(this);
		frame_menu_settings_window->Show(true);
	}
}

void frame_main::on_menu_about(wxCommandEvent &event)
{
	// Display contact information.

	wxString about_text;
	about_text += wxT("      ") + window_title + wxT("\n\n");
	about_text += wxT("      Version: ") + software_version + wxT("\n\n");
	about_text += wxT("      Author: Juan Hauara\n\n");
	about_text += wxT("      Email: ") + email_contact + wxT("\n\n");
	if (!web_contact.IsEmpty())
	{
		about_text += wxT("      Web: ") + web_contact + wxT("\n\n");
	}

	wxMessageDialog *dialog = new wxMessageDialog(NULL, about_text, wxT("About"), wxOK | wxICON_INFORMATION);
	dialog->ShowModal();
}

void frame_main::connect_disconnect(void)
{
	if (connected)
	{
		//timer_serial_rx->Stop();
		serial_port_driver->stop_serial_rx();

		if (serial_port_driver->disconnect())
		{
			connected = false;
			button_connect_disconnect->SetLabel(wxT("Connect"));

			#if DEBUG_LOG
			cout << "disconnected" << endl;
			#endif
		}
	}
	else
	{
		/*
		Parameters are configured and saved to disk.
		Always read from disk before using these variables to use the latest
		configured values.
		*/

		// Update persistent variables from disk.
		persistent_store.load_data();

		// Set the serial port parameters.
		serial_port_driver->set_serial_port(persistent_store.get_string("selected_serialPort"));
		serial_port_driver->set_baud_rate(persistent_store.get_int("baudRate"));
		serial_port_driver->set_data_bits(persistent_store.get_int("dataBits"));
		serial_port_driver->set_parity(persistent_store.get_int("parity"));
		serial_port_driver->set_stop_bits(persistent_store.get_int("stopBits"));
		serial_port_driver->use_dtr(persistent_store.get_bool("useDtr"));
		serial_port_driver->use_rts(persistent_store.get_bool("useRts"));

		add_nl = persistent_store.get_bool("add_nl");
		add_cr = persistent_store.get_bool("add_cr");

		#if DEBUG_LOG
		cout << "\nserial port = " << persistent_store.get_string("selected_serialPort") << endl;
		cout << "baud rate = " << persistent_store.get_int("baudRate") << " baud\n";
		cout << "Data bits = " << persistent_store.get_int("dataBits") << endl;
		cout << "Parity = " << persistent_store.get_int("parity") << endl;
		cout << "Stop bits = " << persistent_store.get_int("stopBits") << endl;
		cout << "Use DTR = " << persistent_store.get_bool("useDtr") << endl;
		cout << "Use RTS = " << persistent_store.get_bool("useRts") << endl;
		cout << "Add new line = " << persistent_store.get_bool("add_nl") << endl;
		cout << "Add carriage return = " << persistent_store.get_bool("add_cr") << endl;
		#endif

		if (serial_port_driver->get_serial_port() == "")
		{
			// Display a warning when no serial port is selected.
			wxMessageDialog *dialog = new wxMessageDialog(NULL, wxT("No serial port selected."), wxT(""), wxOK | wxICON_EXCLAMATION);
			dialog->ShowModal();

			return;
		}

		if (serial_port_driver->connect())
		{
			connected = true;
			button_connect_disconnect->SetLabel(wxT("Disconnect"));

			#if DEBUG_LOG
			cout << "connected" << endl;
			#endif

			//timer_serial_rx->Start(10, wxTIMER_CONTINUOUS);	// Start receiving bytes from the serial port.
			serial_port_driver->start_serial_rx();	// Start receiving bytes from the serial port.
		}
		else
		{
			// Display the connection error.
			string error = serial_port_driver->get_last_error();
			wxString error_text(error);

			wxMessageDialog *dialog = new wxMessageDialog(NULL, error_text, wxT("Could not connect"), wxOK | wxICON_EXCLAMATION);
			dialog->ShowModal();
		}
	}
}

list<int> frame_main::get_available_com_ports(void)
{
	/*
	Build a list with the available COM port numbers.
	*/

	wchar_t com_port_path[5000]; // Buffer used to store COM port paths.
	list<int> port_list;

	for (uint8_t i = 0; i < 255; i++) // Check ports from COM0 to COM254.
	{
		wstring str = L"COM" + to_wstring(i);
		DWORD res = QueryDosDevice(str.c_str(), com_port_path, 5000);

		if (res != 0)
		{
			port_list.push_back(i);

			/*#if DEBUG_LOG
			cout << str << ": " << com_port_path << endl;
			#endif*/
		}
	}

	return port_list;
}

void frame_main::add_serial_ports_to_combo_box(list<int> com_ports)
{
	// Add available COM ports to the combo box.

	list<int>::iterator it;
	for (it = com_ports.begin(); it != com_ports.end(); ++it)
	{
		wxString port = wxString("COM");
		port << *it;	// Append the port number to COM.

		#if DEBUG_LOG
		cout << port << endl;
		#endif

		combo_box_serial_port->Append(port);	// Method inherited from wxItemContainer.
	}
}

void frame_main::on_combo_box_drop_down_serial_port(wxCommandEvent &event)  // Event generated when the combo box opens.
{
	if (connected)
	{
		connect_disconnect();
	}

	combo_box_serial_port->Clear();	// Clear the combo box content. Method inherited from wxItemContainer.

	list<int> com_ports = get_available_com_ports();	// Return a list with the available COM port numbers.

	if (!com_ports.empty())
	{
		add_serial_ports_to_combo_box(com_ports);	// Add available serial ports to the combo box.
	}
	else
	{
		// Display a warning when no serial port is available.
		wxMessageDialog *dialog = new wxMessageDialog(NULL, wxT("No available serial port found."), wxT(""), wxOK | wxICON_EXCLAMATION);
		dialog->ShowModal();
	}
}

void frame_main::on_combo_box_close_up_serial_port(wxCommandEvent &event)  // Event generated when the combo box closes.
{
	wxString port = combo_box_serial_port->GetStringSelection();			// Read the selected serial port.
	persistent_store.set_string("selected_serialPort", port.ToStdString());	// Save the value in a persistent variable with the same name.

	#if DEBUG_LOG
	cout << "selected_serial_port = " << port.ToStdString() << endl;
	#endif
}

void frame_main::on_combo_box_baud_rate(wxCommandEvent &event)
{
	if (connected)
	{
		connect_disconnect();
	}

	wxString baud = combo_box_baud_rate->GetStringSelection();			// Read the selected baud rate.
	int32_t baud_rate_value = wxAtoi(baud);								// Convert from wxString to integer.
	persistent_store.set_int("baudRate", baud_rate_value);				// Save the value in a persistent variable with the same name.

	#if DEBUG_LOG
	cout << "baud_rate = " << baud_rate_value << endl;
	#endif
}

void frame_main::on_click_connect_disconnect(wxCommandEvent &event)
{
	connect_disconnect();
}

void frame_main::on_click_clear_received(wxCommandEvent &event)
{
	text_ctrl_received->Clear();
}

void frame_main::on_click_send(wxCommandEvent &event)
{
	if (connected)
	{
		wxString text = text_ctrl_send->GetValue();

		if (add_nl)
		{
			text += "\n";
		}

		if (add_cr)
		{
			text += "\r";
		}

		if (text != "")
		{
			if (serial_port_driver->tx_string(text.ToStdString()))
			{
				#if DEBUG_LOG
				cout << "Tx = " << text << endl;
				#endif
			}
			else
			{
				wxString error_text(serial_port_driver->get_last_error());
				wxMessageDialog *dialog = new wxMessageDialog(NULL, error_text, wxT("Could not send"), wxOK | wxICON_EXCLAMATION);
				dialog->ShowModal();
			}
		}
	}
	else
	{
		// Display a warning when the serial port is not connected.
		wxMessageDialog *dialog = new wxMessageDialog(NULL, wxT("Not connected"), wxT(""), wxOK | wxICON_EXCLAMATION);
		dialog->ShowModal();

		#if DEBUG_LOG
		cout << "not connected" << endl;
		#endif
	}
}
//==================================================================


//==================================================================
// App class.
//==================================================================
class app : public wxApp
{
	public:
		virtual bool OnInit();
};

IMPLEMENT_APP(app)

bool app::OnInit()
{
	frame_main *frame_main_window = new frame_main(window_title, window_size);
	frame_main_window->Show(true);

	return true;
}
//==================================================================
