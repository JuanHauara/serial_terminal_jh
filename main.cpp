/*
 ----------------------------------------------------------------------------

	Autor: Juan Hauara
	Fecha: 05/03/2022
	Contacto: neural_sensors@proton.me

 ----------------------------------------------------------------------------
 */


#include <wx/wx.h>

#include <iostream>
#include <string>
#include <list>
#include <chrono>

#include "Persistent_data.h"	// Variables persistentes desde un archivo en disco.


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
	#include "SerialPortWindows.h"
#endif

const wxString SOFTWARE_VERSION("v0.1.2");
const wxString WINDOW_TITLE("Serial Terminal");
const wxString EMAIL_CONTACT("juanhauara@gmail.com");
const wxString WEB_CONTACT("Juan Hauara");

const wxSize WINDOW_SIZE = wxSize(550, 550);
const wxColour WINDOWS_COLOR = wxColour(233, 233, 233);

#define DEFAULT_BAUD_RATE	9600
#define DEFAULT_DATA_BITS	8			/* 5, 6, 7, 8 */
#define DEFAULT_PARITY		NOPARITY	/* EVENPARITY, ODDPARITY */
#define DEFAULT_STOP_BITS	ONESTOPBIT	/* ONE5STOPBITS, TWOSTOPBITS */
#define DEFAULT_USE_DTR		false
#define DEFAULT_USE_RTS		false
#define DEFAULT_ADD_NL		false		/* Add new line caracter '\n' (0x0A in hexadecimal) at the end of the message before send it. */
#define DEFAULT_ADD_CR		false		/* Add new line caracter '\r' (0x0D in hexadecimal) at the end of the message before send it. */

//==================================================================


// Defines
#define WXT_NO_PARITY		wxT("No parity")
#define WXT_EVEN_PARITY		wxT("Even parity")
#define WXT_ODD_PARITY		wxT("Odd parity")

#define WXT_1_STOP_BIT		wxT("1 stop bit")
#define WXT_15_STOP_BITS	wxT("1.5 stop bits")
#define WXT_2_STOP_BITS		wxT("2 stop bits")


// Variables globales
bool menu_settings_up = false;


//==================================================================
// Clase Frame principal
//==================================================================
class Frame_menu_settings : public wxFrame
{
	public:
		Frame_menu_settings(wxWindow *parent);
	
	private:
		// Widgets
		wxComboBox *comboBox_dataBits;
		wxComboBox *comboBox_parity;
		wxComboBox *comboBox_stopBits;
		wxCheckBox *checkBox_useDtr;
		wxCheckBox *checkBox_useRts;
		wxCheckBox *checkBox_add_nl;
		wxCheckBox *checkBox_add_cr;
		wxButton *button_default;
		wxButton *button_save;
		
		// Variables
		int32_t dataBits;
		int32_t parity;
		int32_t stopBits;
		bool useDtr;
		bool useRts;
		bool add_nl;
		bool add_cr;
		
		Persistent_data *persistent_data = new Persistent_data("persistent_data.dat");
		
		// Métodos
		void setValuesOnWidgets(void);
		
		// Event handlers
		void OnClose(wxCloseEvent &event);
		void OnComboBox_dataBits(wxCommandEvent &event);
		void OnComboBox_parity(wxCommandEvent &event);
		void OnComboBox_stopBits(wxCommandEvent &event);
		void OnCheckBox_useDtr(wxCommandEvent &event);
		void OnCheckBox_useRts(wxCommandEvent &event);
		void OnCheckBox_add_nl(wxCommandEvent &event);
		void OnCheckBox_add_cr(wxCommandEvent &event);
		void OnClick_default(wxCommandEvent &event);
		void OnClick_save(wxCommandEvent &event);
};

Frame_menu_settings::Frame_menu_settings(wxWindow *parent) : wxFrame(parent, wxID_ANY, wxT("Serial port settings"), 
wxDefaultPosition, wxSize(240, 370), wxCAPTION | wxCLOSE_BOX | wxFRAME_FLOAT_ON_PARENT)
{
	CentreOnScreen();
	
	// Cuando se da click al botón de cerrar ventana llama al método OnClose().
	Bind(wxEVT_CLOSE_WINDOW, &Frame_menu_settings::OnClose, this);
	
	wxPanel *panel = new wxPanel(this, wxID_ANY);
	panel->SetBackgroundColour(WINDOWS_COLOR);
	
	
	// Configuración de widgets
	// statics texts
	wxStaticText *staticText_dataBits = new wxStaticText(panel, wxID_ANY, wxT("Data bits:"));
	wxStaticText *staticText_parity = new wxStaticText(panel, wxID_ANY, wxT("Parity:"));
	wxStaticText *staticText_stopBits = new wxStaticText(panel, wxID_ANY, wxT("Stop bits:"));
	wxStaticText *staticText_flowControl = new wxStaticText(panel, wxID_ANY, wxT("Flow control:"));
	wxStaticText *staticText_add_to_message = new wxStaticText(panel, wxID_ANY, wxT("Add to message:"));
	
	
	// comboBox data bits
	wxArrayString dataBitsChoices;
	dataBitsChoices.Add(wxT("5"));	// Atención: No cambiar valor
	dataBitsChoices.Add(wxT("6"));	// Atención: No cambiar valor
	dataBitsChoices.Add(wxT("7"));	// Atención: No cambiar valor
	dataBitsChoices.Add(wxT("8"));	// Atención: No cambiar valor
	comboBox_dataBits = new wxComboBox(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(95, -1), dataBitsChoices, wxCB_READONLY);
	comboBox_dataBits->Bind(wxEVT_COMBOBOX, &Frame_menu_settings::OnComboBox_dataBits, this);
	
	// comboBox paridad
	wxArrayString parityChoices;
	parityChoices.Add(WXT_NO_PARITY);
	parityChoices.Add(WXT_EVEN_PARITY);
	parityChoices.Add(WXT_ODD_PARITY);
	comboBox_parity = new wxComboBox(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(95, -1), parityChoices, wxCB_READONLY);
	comboBox_parity->Bind(wxEVT_COMBOBOX, &Frame_menu_settings::OnComboBox_parity, this);
	
	// comboBox stop bits
	wxArrayString stopBitsChoices;
	stopBitsChoices.Add(WXT_1_STOP_BIT);
	stopBitsChoices.Add(WXT_15_STOP_BITS);
	stopBitsChoices.Add(WXT_2_STOP_BITS);
	comboBox_stopBits = new wxComboBox(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(95, -1), stopBitsChoices, wxCB_READONLY);
	comboBox_stopBits->Bind(wxEVT_COMBOBOX, &Frame_menu_settings::OnComboBox_stopBits, this);
	
	
	// Check box usar DTR
	checkBox_useDtr = new wxCheckBox(panel, wxID_ANY, wxT("enable DTR"), wxDefaultPosition, wxDefaultSize);
	checkBox_useDtr->Bind(wxEVT_CHECKBOX, &Frame_menu_settings::OnCheckBox_useDtr, this);
	
	// Check box usar RTS
	checkBox_useRts = new wxCheckBox(panel, wxID_ANY, wxT("enable RTS"), wxDefaultPosition, wxDefaultSize);
	checkBox_useRts->Bind(wxEVT_CHECKBOX, &Frame_menu_settings::OnCheckBox_useRts, this);
	
	
	// Check box add new line
	checkBox_add_nl = new wxCheckBox(panel, wxID_ANY, wxT("add new line"), wxDefaultPosition, wxDefaultSize);
	checkBox_add_nl->Bind(wxEVT_CHECKBOX, &Frame_menu_settings::OnCheckBox_add_nl, this);
	
	// Check box add carry return
	checkBox_add_cr = new wxCheckBox(panel, wxID_ANY, wxT("add carry return"), wxDefaultPosition, wxDefaultSize);
	checkBox_add_cr->Bind(wxEVT_CHECKBOX, &Frame_menu_settings::OnCheckBox_add_cr, this);
	
	
	// Botón default
	button_default = new wxButton(panel, wxID_ANY, wxT("Default"), wxDefaultPosition, wxDefaultSize);
	button_default->Bind(wxEVT_BUTTON, &Frame_menu_settings::OnClick_default, this);
	
	// Botón guardar
	button_save = new wxButton(panel, wxID_ANY, wxT("Save"), wxDefaultPosition, wxDefaultSize);
	button_save->Bind(wxEVT_BUTTON, &Frame_menu_settings::OnClick_save, this);
	
	
	// Creación del layout de la interfaz gráfica
	///////////////////////////////////////////////////////////////////
	// Sizers
	wxBoxSizer *vBoxSizer_main = new wxBoxSizer(wxVERTICAL);			// Sizer vertical principal.
	wxFlexGridSizer *flexGridSizer = new wxFlexGridSizer(2, 10, 10);	// Para textos y combo box.
	wxBoxSizer *hBoxSizer_buttons = new wxBoxSizer(wxHORIZONTAL);		// Sizer horizontal para botones.
	
	flexGridSizer->Add(staticText_dataBits, 0);
	flexGridSizer->Add(comboBox_dataBits, 0);
	flexGridSizer->Add(staticText_parity, 0);
	flexGridSizer->Add(comboBox_parity, 0);
	flexGridSizer->Add(staticText_stopBits, 0);
	flexGridSizer->Add(comboBox_stopBits, 0);
	vBoxSizer_main->Add(flexGridSizer, 0, wxALL, 23);
	
	vBoxSizer_main->Add(staticText_flowControl, 0, wxLEFT, 23);
	vBoxSizer_main->AddSpacer(5);
	vBoxSizer_main->Add(checkBox_useDtr, 0, wxLEFT, 50);
	vBoxSizer_main->AddSpacer(5);
	vBoxSizer_main->Add(checkBox_useRts, 0, wxLEFT, 50);
	
	vBoxSizer_main->AddSpacer(23);
	vBoxSizer_main->Add(staticText_add_to_message, 0, wxLEFT, 23);
	vBoxSizer_main->AddSpacer(5);
	vBoxSizer_main->Add(checkBox_add_nl, 0, wxLEFT, 50);
	vBoxSizer_main->AddSpacer(5);
	vBoxSizer_main->Add(checkBox_add_cr, 0, wxLEFT, 50);
	
	vBoxSizer_main->AddSpacer(27);
	hBoxSizer_buttons->Add(button_default, 0, wxLEFT, 13);
	hBoxSizer_buttons->AddSpacer(56);			// 50 píxeles entre los botones.
	hBoxSizer_buttons->Add(button_save, 0);
	vBoxSizer_main->Add(hBoxSizer_buttons, 0);
	
	panel->SetSizer(vBoxSizer_main, wxEXPAND);	// Sizer vertical principal en el panel.
	
	// Actualiza GUI con valores persistentes desde el disco
	///////////////////////////////////////////////////////////////////
	if (persistent_data->loadData())
	{
		dataBits = persistent_data->getInt("dataBits");
		parity = persistent_data->getInt("parity");
		stopBits = persistent_data->getInt("stopBits");
		useDtr = persistent_data->getBool("useDtr");
		useRts = persistent_data->getBool("useRts");
		add_nl = persistent_data->getBool("add_nl");
		add_cr = persistent_data->getBool("add_cr");
	}
	else
	{
		/*
		 * Si es la primera vez que inicia el programa o no encontró el 
		 * archivo persistentData.dat, setea los valores por defecto e 
		 * intenta crear el archivo.
		 */
		dataBits = DEFAULT_DATA_BITS;
		parity = DEFAULT_PARITY;
		stopBits = DEFAULT_STOP_BITS;
		useDtr = DEFAULT_USE_DTR;
		useRts = DEFAULT_USE_RTS;
		add_nl = DEFAULT_ADD_NL;
		add_cr = DEFAULT_ADD_CR;
		
		// Guarda valores en disco
		persistent_data->setInt("dataBits", dataBits);
		persistent_data->setInt("parity", parity);
		persistent_data->setInt("stopBits", stopBits);
		persistent_data->setBool("useDtr", useDtr);
		persistent_data->setBool("useRts", useRts);
		persistent_data->setBool("add_nl", add_nl);
		persistent_data->setBool("add_cr", add_cr);
	}
	
	setValuesOnWidgets();
	
	#if DEBUG_LOG
	cout << "\nData bits = " << dataBits << endl;
	cout << "Parity = " << parity << endl;
	cout << "Stop bits = " << stopBits << endl;
	cout << "Use DTR = " << useDtr << endl;
	cout << "Use RTS = " << useRts << endl;
	cout << "Add new line = " << add_nl << endl;
	cout << "Add carry return = " << add_cr << endl;
	#endif
}

void Frame_menu_settings::setValuesOnWidgets(void)
{
	if (dataBits == 5) comboBox_dataBits->SetValue(wxT("5"));
	else if (dataBits == 6) comboBox_dataBits->SetValue(wxT("6"));
	else if (dataBits == 7) comboBox_dataBits->SetValue(wxT("7"));
	else if (dataBits == 8) comboBox_dataBits->SetValue(wxT("8"));
	
	if (parity == NOPARITY) comboBox_parity->SetValue(WXT_NO_PARITY);
	else if (parity == EVENPARITY) comboBox_parity->SetValue(WXT_EVEN_PARITY);
	else if (parity == ODDPARITY) comboBox_parity->SetValue(WXT_ODD_PARITY);
	
	if (stopBits == ONESTOPBIT) comboBox_stopBits->SetValue(WXT_1_STOP_BIT);
	else if (stopBits == ONE5STOPBITS) comboBox_stopBits->SetValue(WXT_15_STOP_BITS);
	else if (stopBits == TWOSTOPBITS) comboBox_stopBits->SetValue(WXT_2_STOP_BITS);
	
	checkBox_useDtr->SetValue(useDtr);
	checkBox_useRts->SetValue(useRts);
	checkBox_add_nl->SetValue(add_nl);
	checkBox_add_cr->SetValue(add_cr);
}

void Frame_menu_settings::OnClose(wxCloseEvent &event)
{
	if(event.CanVeto())
	{
		menu_settings_up = false;
	}

	Destroy();
}

void Frame_menu_settings::OnComboBox_dataBits(wxCommandEvent &event)
{
	wxString wxstr = comboBox_dataBits->GetStringSelection();
	dataBits = wxAtoi(wxstr);	// Pasa de wxString a entero.
	
	#if DEBUG_LOG
	cout << "Data bits = " << dataBits << endl;
	#endif
}

void Frame_menu_settings::OnComboBox_parity(wxCommandEvent &event)
{
	wxString wxstr = comboBox_parity->GetStringSelection();
	
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

void Frame_menu_settings::OnComboBox_stopBits(wxCommandEvent &event)
{
	wxString wxstr = comboBox_stopBits->GetStringSelection();
	
	if (wxstr == WXT_1_STOP_BIT)
	{
		stopBits = ONESTOPBIT;
	}
	else if (wxstr == WXT_15_STOP_BITS)
	{
		stopBits = ONE5STOPBITS;
	}
	else if (wxstr == WXT_2_STOP_BITS)
	{
		stopBits = TWOSTOPBITS;
	}
	
	#if DEBUG_LOG 
	cout << "Stop bits = " << stopBits << endl;
	#endif
}

void Frame_menu_settings::OnCheckBox_useDtr(wxCommandEvent &event)
{
	useDtr = checkBox_useDtr->GetValue();
	
	#if DEBUG_LOG 
	cout << "Use DTR = " << useDtr << endl;
	#endif
}

void Frame_menu_settings::OnCheckBox_useRts(wxCommandEvent &event)
{
	useRts = checkBox_useRts->GetValue();
	
	#if DEBUG_LOG 
	cout << "Use RTS = " << useRts << endl;
	#endif
}

void Frame_menu_settings::OnCheckBox_add_nl(wxCommandEvent &event)
{
	add_nl = checkBox_add_nl->GetValue();
	
	#if DEBUG_LOG 
	cout << "Add new line = " << add_nl << endl;
	#endif
}

void Frame_menu_settings::OnCheckBox_add_cr(wxCommandEvent &event)
{
	add_cr = checkBox_add_cr->GetValue();
	
	#if DEBUG_LOG 
	cout << "Add carry return = " << add_cr << endl;
	#endif
}

void Frame_menu_settings::OnClick_default(wxCommandEvent &event)
{
	dataBits = DEFAULT_DATA_BITS;
	parity = DEFAULT_PARITY;
	stopBits = DEFAULT_STOP_BITS;
	useDtr = DEFAULT_USE_DTR;
	useRts = DEFAULT_USE_RTS;
	
	setValuesOnWidgets();
	
	#if DEBUG_LOG
	cout << "\nData bits = " << dataBits << endl;
	cout << "Parity = " << parity << endl;
	cout << "Stop bits = " << stopBits << endl;
	cout << "Use DTR = " << useDtr << endl;
	cout << "Use RTS = " << useRts << endl;
	cout << "Add new line = " << add_nl << endl;
	cout << "Add carry return = " << add_cr << endl;
	#endif
}

void Frame_menu_settings::OnClick_save(wxCommandEvent &event)
{
	// Guarda valores en disco
	persistent_data->setInt("dataBits", dataBits);
	persistent_data->setInt("parity", parity);
	persistent_data->setInt("stopBits", stopBits);
	persistent_data->setBool("useDtr", useDtr);
	persistent_data->setBool("useRts", useRts);
	persistent_data->setBool("add_nl", add_nl);
	persistent_data->setBool("add_cr", add_cr);
	
	menu_settings_up = false;
	
	#if DEBUG_LOG 
	cout << "\nData bits = " << dataBits << endl;
	cout << "Parity = " << parity << endl;
	cout << "Stop bits = " << stopBits << endl;
	cout << "Use DTR = " << useDtr << endl;
	cout << "Use RTS = " << useRts << endl;
	cout << "Add new line = " << add_nl << endl;
	cout << "Add carry return = " << add_cr << endl;
	#endif
	
	Destroy();	// Cierra la esta ventana.
}

//==================================================================
// Clase Frame principal
//==================================================================
class Frame_main : public wxFrame
{
	public:
		Frame_main(const wxString &title, const wxSize size);

	private:
		// Widgets
		wxComboBox *comboBox_serialPort;
		wxComboBox *comboBox_baudRate;
		wxButton *button_connectDisconnect;
		wxButton *button_clearReceived;
		wxTextCtrl *textCtrl_received;
		wxTextCtrl *textCtrl_send;
		wxButton *button_send;
		//wxTimer *timer_serialRx;
		// Menú
		wxMenuBar *menuBar;
		wxMenu *editMenu;
		wxMenu *helpMenu;

		// Variables
		bool connected = false;
		
		bool add_nl;
		bool add_cr;
		
		wxArrayString serialPortChoices;
		wxArrayString baudChoices;
		
		SerialPort *serialPort = new SerialPort();
		Frame_menu_settings *frameMenuSettings;
		
		// Variables persistentes guardadas en disco
		//Persistent_data *persistent_data = new Persistent_data("persistent_data.dat");
		Persistent_data persistent_data = Persistent_data("persistent_data.dat");
		
		// Métodos
		void connectDisconnect(void);
		list<int> getAvailableComPorts(void);
		void addSerialPortsToComboBox(list<int> comPorts);

		// Event handlers
		void OnClose(wxCloseEvent &event);
		void OnComboBoxDropDown_serialPort(wxCommandEvent &event);
		void OnComboBoxCloseUp_serialPort(wxCommandEvent &event);
		void OnComboBox_baudRate(wxCommandEvent &event);
		void OnClick_connectDisconnect(wxCommandEvent &event);
		void OnClick_clearReceived(wxCommandEvent &event);
		void OnClick_send(wxCommandEvent &event);
		// Menú
		void OnMenu_settings(wxCommandEvent &event);
		void OnMenu_about(wxCommandEvent &event);
		//void OnTimer_serialRx(wxTimerEvent &event);
};

// Constructor
Frame_main::Frame_main(const wxString &title, const wxSize size) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, size)
{
	CentreOnScreen();
	//Maximize();

	// Cuando se da click al botón de cerrar ventana llama al método OnClose().
	Bind(wxEVT_CLOSE_WINDOW, &Frame_main::OnClose, this);

	// Barra de menú
	//////////////////////////////////////////////////////////////////////////////
	menuBar = new wxMenuBar();
	//-------------
	// Menú Editar
	editMenu = new wxMenu();
	
	editMenu->Append(wxID_PROPERTIES, wxT("&Settings"));
	Bind(wxEVT_MENU, &Frame_main::OnMenu_settings, this, wxID_PROPERTIES);
	
	menuBar->Append(editMenu, wxT("&Edit"));
	//-------------
	// Menú Ayuda
	helpMenu = new wxMenu();
	
	helpMenu->Append(wxID_OPEN, wxT("&About"));
	Bind(wxEVT_MENU, &Frame_main::OnMenu_about, this, wxID_OPEN);
	
	menuBar->Append(helpMenu, wxT("&Help"));
	//-------------
	SetMenuBar(menuBar);
	//////////////////////////////////////////////////////////////////////////////

	// Paneles
	//////////////////////////////////////////////////////////////////////////////
	// Panel principal del frame
	wxPanel *panelMain = new wxPanel(this, wxID_ANY);
	panelMain->SetBackgroundColour(WINDOWS_COLOR);
	//////////////////////////////////////////////////////////////////////////////

	// Fuentes
	// Ver documentación de clase "wxFontInfo" para ver las distintas opciones de fuentes:
	// https://docs.wxwidgets.org/trunk/classwx_font_info.html#a74ff2d0449e75bcafc1dd4695f97ab66)
	//////////////////////////////////////////////////////////////////////////////	
	wxFont font_receivedText(wxFontInfo(11).FaceName("Courier New").Light().AntiAliased(true));	// Fuente para los text control
	//////////////////////////////////////////////////////////////////////////////

	// Inicializa los widgets
	//////////////////////////////////////////////////////////////////////////////
	// Texto comboBox puerto serial
	wxStaticText *staticText_serialPort = new wxStaticText(panelMain, wxID_ANY, wxT("Serial port:"));
	//staticText_serialPort->SetFont(fontTitle);

	// comboBox puerto serial
	comboBox_serialPort = new wxComboBox(panelMain, wxID_ANY, wxEmptyString, wxDefaultPosition,
									  wxSize(70, -1), serialPortChoices, wxCB_READONLY);
	//comboBox_serialPort->SetFont(fontCommon);
	comboBox_serialPort->Bind(wxEVT_COMBOBOX_DROPDOWN, &Frame_main::OnComboBoxDropDown_serialPort, this); // Evento generado cuando el combo box se despliega.
	comboBox_serialPort->Bind(wxEVT_COMBOBOX_CLOSEUP, &Frame_main::OnComboBoxCloseUp_serialPort, this);   // Evento generado cuando el combo box se cierra.

	// Texto comboBox baud rate
	wxStaticText *staticText_baudRate = new wxStaticText(panelMain, wxID_ANY, wxT("Baud:"));
	//staticText_baudRate->SetFont(fontTitle);

	// comboBox baud rate
	baudChoices.Add(wxT("110"));
	baudChoices.Add(wxT("300"));
	baudChoices.Add(wxT("600"));
	baudChoices.Add(wxT("1200"));
	baudChoices.Add(wxT("2400"));
	baudChoices.Add(wxT("4800"));
	baudChoices.Add(wxT("9600"));
	baudChoices.Add(wxT("14400"));
	baudChoices.Add(wxT("19200"));
	baudChoices.Add(wxT("38400"));
	baudChoices.Add(wxT("57600"));
	baudChoices.Add(wxT("115200"));
	baudChoices.Add(wxT("128000"));
	baudChoices.Add(wxT("256000"));
	comboBox_baudRate = new wxComboBox(panelMain, wxID_ANY, wxEmptyString, wxDefaultPosition,
									   wxSize(70, -1), baudChoices, wxCB_READONLY);
	//comboBox_baudRate->SetFont(fontCommon);
	comboBox_baudRate->Bind(wxEVT_COMBOBOX, &Frame_main::OnComboBox_baudRate, this);

	// Botón conectar/desconectar
	button_connectDisconnect = new wxButton(panelMain, wxID_ANY, wxT("Connect"), wxDefaultPosition, wxDefaultSize);	// wxSize(90, 25)
	//button_connectDisconnect->SetFont(fontButton);
	button_connectDisconnect->Bind(wxEVT_BUTTON, &Frame_main::OnClick_connectDisconnect, this);
	
	// Botón borrar mensajes recibidos
	button_clearReceived = new wxButton(panelMain, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize);	// wxSize(70, 25)
	//button_clearReceived->SetFont(fontButton);
	button_clearReceived->Bind(wxEVT_BUTTON, &Frame_main::OnClick_clearReceived, this);

	// Text control mensajes recibidos
	textCtrl_received = new wxTextCtrl(panelMain, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
	textCtrl_received->SetFont(font_receivedText);

	// Text control mensaje enviado
	textCtrl_send = new wxTextCtrl(panelMain, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	textCtrl_send->SetFont(font_receivedText);

	// Botón enviar
	button_send = new wxButton(panelMain, wxID_ANY, wxT("Send"), wxDefaultPosition, wxDefaultSize);	// wxSize(70, 25)
	//button_send->SetFont(fontButton);
	button_send->Bind(wxEVT_BUTTON, &Frame_main::OnClick_send, this);
	//////////////////////////////////////////////////////////////////////////////

	// Creación del layout de la interfaz gráfica
	//////////////////////////////////////////////////////////////////////////////
	// Sizers
	wxBoxSizer *vboxMain = new wxBoxSizer(wxVERTICAL);													// Sizer vertical principal
	//wxStaticBoxSizer *hboxTop = new wxStaticBoxSizer(wxHORIZONTAL, panelMain, wxString("Settings"));	// Sizer horizontal superior
	wxBoxSizer *hboxTop = new wxBoxSizer(wxHORIZONTAL);													// Sizer horizontal superior
	wxBoxSizer *hboxBottom = new wxBoxSizer(wxHORIZONTAL);												// Sizer horizontal inferior

	vboxMain->AddSpacer(7);	// 7 píxeles de margen entre el menú y los widgets.

	// Agrega los widgets del sizer vertical superior.
	// --------------
	hboxTop->Add(staticText_serialPort, 0, wxALIGN_CENTRE);		// Centra el widget verticalmente.
	hboxTop->AddSpacer(4);										// 4 píxeles de espacio entre el texto y el combo box.
	hboxTop->Add(comboBox_serialPort, 0, wxALIGN_CENTRE);		// Centra el widget verticalmente.
	hboxTop->AddSpacer(20);										// 20 píxeles de espacio entre el combo box y el texto de baud rate.
	
	hboxTop->Add(staticText_baudRate, 0, wxALIGN_CENTRE);		// Centra el widget verticalmente.
	hboxTop->AddSpacer(4);										// 4 píxeles de espacio entre el texto y el combo box.
	hboxTop->Add(comboBox_baudRate, 0, wxALIGN_CENTRE);			// Centra el widget verticalmente.
	hboxTop->AddSpacer(20);										// 20 píxeles de espacio entre el combo box y el botón de conectar.
	
	hboxTop->Add(button_connectDisconnect, 0, wxALIGN_CENTRE);	// Centra el widget verticalmente.
	
	hboxTop->AddSpacer(90);										// 90 píxeles de espacio entre el botón de conectar y el botón de borrar.
	hboxTop->Add(button_clearReceived, 0, wxALIGN_CENTRE);		// Centra el widget verticalmente.
	// --------------
	vboxMain->Add(hboxTop, 0, wxALL, 4);	// Agrega el sizer horizontal superior dentro del sizer vertical principal con 4 píxeles de margen en todos lados.

	// Texto de mensajes recibidos
	// --------------
	/*
	 * El 1 en el segundo parámetro y el flag wxEXPAND en el tercer parámetro
	 * indica que se expandirá por todo el espacio disponible en la ventana. 
	 * Tendrá 4 píxeles de margen en todos los lados,
	 */
	vboxMain->Add(textCtrl_received, 1, wxEXPAND | wxALL, 4);
	// --------------
	
	// Agrega los widgets del sizer vertical inferior.
	// --------------
	/*
	 * El 1 en el segundo parámetro indica que se expandirá por todo el 
	 * espacio disponible en la ventana. 
	 * Estará centrado verticalmente.
	 */
	hboxBottom->Add(textCtrl_send, 1, wxALIGN_CENTRE);
	hboxBottom->AddSpacer(5);
	hboxBottom->Add(button_send, 0, wxALIGN_CENTRE);	// Centrado verticalmente.
	// --------------
	vboxMain->Add(hboxBottom, 0, wxEXPAND | wxALL, 4);	// Agrega el sizer horizontal inferior dentro del sizer vertical principal.
	vboxMain->AddSpacer(2);
	
	panelMain->SetSizer(vboxMain, wxEXPAND);	// Sizer vertical principal en el panel principal.
	//////////////////////////////////////////////////////////////////////////////
	
	// Timer para actualizar textCtrl de mensajes recibidos
	//timer_serialRx = new wxTimer(this, wxID_ANY);
	//timer_serialRx = new wxTimer(this);
	//timer_serialRx->Bind(wxEVT_TIMER, &Frame_main::OnTimer_serialRx, this);	// Evento generado cuando vence el Timer.
	//timer_serialRx->Bind(wxEVT_TIMER, &Frame_main::OnTimer_serialRx, this, timer_serialRx->GetId());
		
	// Actualiza GUI con las variables persistentes desde el disco.
	//////////////////////////////////////////////////////////////////////////////
	string selected_serialPort;
	int32_t baudRate;
		
	if (persistent_data.loadData())	// Si existe el archivo
	{
		selected_serialPort = persistent_data.getString("selected_serialPort");
		baudRate = persistent_data.getInt("baudRate");
		add_nl = persistent_data.getBool("add_nl");
		add_cr = persistent_data.getBool("add_cr");
	}
	else
	{
		/*
		 * Si es la primera vez que inicia el programa o no encontró el 
		 * archivo persistentData.dat, setea los valores por defecto e 
		 * intenta crear el archivo.
		 */
		selected_serialPort = "";
		baudRate = DEFAULT_BAUD_RATE;
		add_nl = DEFAULT_ADD_NL;
		add_cr = DEFAULT_ADD_CR;
		
		// Guarda valores en disco
		persistent_data.setString("selected_serialPort", selected_serialPort);
		persistent_data.setInt("baudRate", DEFAULT_BAUD_RATE);
		persistent_data.setInt("dataBits", DEFAULT_DATA_BITS);
		persistent_data.setInt("parity", DEFAULT_PARITY);
		persistent_data.setInt("stopBits", DEFAULT_STOP_BITS);
		persistent_data.setBool("useDtr", DEFAULT_USE_DTR);
		persistent_data.setBool("useRts", DEFAULT_USE_RTS);
		persistent_data.setBool("add_nl", DEFAULT_ADD_NL);
		persistent_data.setBool("add_cr", DEFAULT_ADD_CR);
	}
	
	#if DEBUG_LOG	
	cout << "\nserial port = " << persistent_data.getString("selected_serialPort") << endl;
	cout << "baud rate = " << persistent_data.getInt("baudRate") << " baudios\n";
	cout << "Data bits = " << persistent_data.getInt("dataBits") << endl;
	cout << "Parity = " << persistent_data.getInt("parity") << endl;
	cout << "Stop bits = " << persistent_data.getInt("stopBits") << endl;
	cout << "Use DTR = " << persistent_data.getBool("useDtr") << endl;
	cout << "Use RTS = " << persistent_data.getBool("useRts") << endl;
	cout << "Add new line = " << persistent_data.getBool("add_nl") << endl;
	cout << "Add carry return = " << persistent_data.getBool("add_cr") << endl;
	#endif

	list<int> comPorts = getAvailableComPorts();	// Devuelve una lista de enteros con los números de los puertos COM disponibles
	if (!comPorts.empty())
	{
		addSerialPortsToComboBox(comPorts);			// Agrega los puertos seriales disponibles en el combobox
	}
	
	wxString str;
	str << selected_serialPort;
	comboBox_serialPort->SetValue(str);
	
	str.clear();
	str << baudRate;
	comboBox_baudRate->SetValue(str);
	//////////////////////////////////////////////////////////////////////////////
	
	// Setea el text control donde desplegará los mensajes recibidos.
	serialPort->setTextControl(textCtrl_received);
}

/*void Frame_main::OnTimer_serialRx(wxTimerEvent &event)
{
	textCtrl_received->AppendText(serialPort->pollSerialPort());
	textCtrl_received->AppendText("\n");
	cout << "Hola mundo!\n";
}*/

void Frame_main::OnClose(wxCloseEvent &event)
{
	if(event.CanVeto())
	{
		if (connected) connectDisconnect();	// Si está conectado, desconecta el puerto utilizado
	}

	Destroy();
}

void Frame_main::OnMenu_settings(wxCommandEvent &event)
{
	if (!menu_settings_up)
	{
		if (connected) connectDisconnect();	// Si está conectado, desconecta el puerto utilizado
		
		menu_settings_up = true;
		
		frameMenuSettings = new Frame_menu_settings(this);
		frameMenuSettings->Show(true);
	}
}

void Frame_main::OnMenu_about(wxCommandEvent &event)
{
	// Muestra un diálogo con información de contacto
	
	wxString aboutText;
	aboutText += wxT("      ") + WINDOW_TITLE + wxT("\n\n");
	aboutText += wxT("      Version: ") + SOFTWARE_VERSION + wxT("\n\n");
	aboutText += wxT("      Author: Juan Hauara\n\n");
	aboutText += wxT("      Email: ") + EMAIL_CONTACT + wxT("\n\n");
	aboutText += wxT("      Web: ") + WEB_CONTACT + wxT("\n\n");
	
	wxMessageDialog *dialog = new wxMessageDialog(NULL, aboutText, wxT("About"), wxOK | wxICON_INFORMATION);
	dialog->ShowModal();
}

void Frame_main::connectDisconnect(void)
{
	if (connected)
	{
		//timer_serialRx->Stop();
		serialPort->stopSerialRx();
		
		if (serialPort->disconnect())
		{
			connected = false;
			button_connectDisconnect->SetLabel(wxT("Connect"));
			
			#if DEBUG_LOG 
			cout << "disconnected" << endl;
			#endif
		}
	}
	else
	{		
		/*
		 * Parámetros configurados y guardados en disco. 
		 * Siempre leer desde el disco antes de usar estas variables, para 
		 * segurarse que se estén usando los últimos valores configurados.
		 */
		
		// Actualiza variables persistentes desde el disco
		persistent_data.loadData();
		// Setea los parámetros
		serialPort->setSerialPort(persistent_data.getString("selected_serialPort"));
		serialPort->setBaudRate(persistent_data.getInt("baudRate"));
		serialPort->setDataBits(persistent_data.getInt("dataBits"));
		serialPort->setParity(persistent_data.getInt("parity"));
		serialPort->setStopBits(persistent_data.getInt("stopBits"));
		serialPort->useDtr(persistent_data.getBool("useDtr"));
		serialPort->useRts(persistent_data.getBool("useRts"));
		
		add_nl = persistent_data.getBool("add_nl");
		add_cr = persistent_data.getBool("add_cr");
		
		#if DEBUG_LOG
		cout << "\nserial port = " << persistent_data.getString("selected_serialPort") << endl;
		cout << "baud rate = " << persistent_data.getInt("baudRate") << " baudios\n";
		cout << "Data bits = " << persistent_data.getInt("dataBits") << endl;
		cout << "Parity = " << persistent_data.getInt("parity") << endl;
		cout << "Stop bits = " << persistent_data.getInt("stopBits") << endl;
		cout << "Use DTR = " << persistent_data.getBool("useDtr") << endl;
		cout << "Use RTS = " << persistent_data.getBool("useRts") << endl;
		cout << "Add new line = " << persistent_data.getBool("add_nl") << endl;
		cout << "Add carry return = " << persistent_data.getBool("add_cr") << endl;
		#endif
		
		if (serialPort->getSerialPort() == "")
		{
			// Muestra diálogo avisando que no ha seleccionado un puerto serial.
			wxMessageDialog *dialog = new wxMessageDialog(NULL, wxT("No serial port selected."), wxT(""), wxOK | wxICON_EXCLAMATION);
			dialog->ShowModal();
			
			return;
		}
		
		if (serialPort->connect())
		{
			connected = true;
			button_connectDisconnect->SetLabel(wxT("Disconnect"));

			#if DEBUG_LOG 
			cout << "connected" << endl;
			#endif
			
			//timer_serialRx->Start(10, wxTIMER_CONTINUOUS);	// Comienza a recibir Bytes desde el puerto serie.
			serialPort->startSerialRx();	// Comienza a recibir Bytes desde el puerto serie.
		}
		else
		{
			// Muestra diálogo avisando que hubo error al intentar conectar.
			string error = serialPort->getLastError();
			wxString _error(error);
			
			wxMessageDialog *dialog = new wxMessageDialog(NULL, _error, wxT("Could not connect"), wxOK | wxICON_EXCLAMATION);
			dialog->ShowModal();
		}
	}
}

list<int> Frame_main::getAvailableComPorts(void)
{
	/*
	 * Devuelve una lista de enteros con los números de los puertos COM disponibles.
	 */
	 
    wchar_t comPortPath[5000]; // buffer to store the path of the COM PORTS
    list<int> portList;

    for (uint8_t i = 0; i < 255; i++) // checking ports from COM0 to COM255
    {
        wstring str = L"COM" + to_wstring(i); // converting to COM0, COM1, COM2
        DWORD res = QueryDosDevice(str.c_str(), comPortPath, 5000);

        // Test the return value and error if any
        if (res != 0) //QueryDosDevice returns zero if it didn't find an object
        {
            portList.push_back(i);
            
            /*#if DEBUG_LOG 
            cout << str << ": " << comPortPath << endl;
            #endif*/
        }
    }
    
    return portList;
}

void Frame_main::addSerialPortsToComboBox(list<int> comPorts)
{
	// Agrega los puertos COM disponibles al combobox
	
	list<int>::iterator it;
	for (it = comPorts.begin(); it != comPorts.end(); ++it)
	{
		wxString port = wxString("COM");
		port << *it;	// Agrega el número a COM
		
		#if DEBUG_LOG 
		cout << port << endl;
		#endif
		
		comboBox_serialPort->Append(port);	// Método heredado de la clase wxItemContainer
	}
}

void Frame_main::OnComboBoxDropDown_serialPort(wxCommandEvent &event)  // Evento generado cuando el combo box se despliega.
{	
	if (connected) connectDisconnect();	// Si está conectado, desconecta el puerto serie
	
	comboBox_serialPort->Clear();	// Borra el contenido del combobox. Método heredado de la clase wxItemContainer
	
	list<int> comPorts = getAvailableComPorts();	// Devuelve una lista de enteros con los números de los puertos COM disponibles
	
	if (!comPorts.empty())
	{
		addSerialPortsToComboBox(comPorts);	// Agrega los puertos seriales disponibles en el combobox
	}
	else
	{
		// Muestra diálogo avisando que no hay ningún puerto serial disponible.
		wxMessageDialog *dialog = new wxMessageDialog(NULL, wxT("No available serial port found."), wxT(""), wxOK | wxICON_EXCLAMATION);
		dialog->ShowModal();
	}
}

void Frame_main::OnComboBoxCloseUp_serialPort(wxCommandEvent &event)  // Evento generado cuando el combo box se cierra.
{	
	wxString port = comboBox_serialPort->GetStringSelection();	// Lee el puerto serial seleccionado
	persistent_data.setString("selected_serialPort", port.ToStdString());			// Guarda el valor en una variable persistente del mismo nombre
	
	#if DEBUG_LOG 
	cout << "selected_serialPort = " << port.ToStdString() << endl;
	#endif
}

void Frame_main::OnComboBox_baudRate(wxCommandEvent &event)
{	
	if (connected) connectDisconnect();							// Si está conectado, desconecta el puerto serie
	
	wxString baud = comboBox_baudRate->GetStringSelection();	// Lee el baud rate seleccionado
	int32_t _baud = wxAtoi(baud);								// Pasa de wxString a entero
	persistent_data.setInt("baudRate", _baud);							// Guarda el valor en una variable persistente del mismo nombre
	
	#if DEBUG_LOG 
	cout << "baudRate = " << _baud << endl;
	#endif
}

void Frame_main::OnClick_connectDisconnect(wxCommandEvent &event)
{
	connectDisconnect();
}

void Frame_main::OnClick_clearReceived(wxCommandEvent &event)
{
	textCtrl_received->Clear();
}

void Frame_main::OnClick_send(wxCommandEvent &event)
{
	if (connected)
	{
		wxString text = textCtrl_send->GetValue();
		
		if (add_nl) text += "\n";
		
		if (add_cr) text += "\r";
		
		if (text != "")
		{
			serialPort->txString(text.ToStdString());
			
			#if DEBUG_LOG 
			cout << "Tx = " << text << endl;
			#endif
		}
	}
	else
	{
		// Muestra diálogo avisando que no está conectado.
		wxMessageDialog *dialog = new wxMessageDialog(NULL, wxT("Not connected!"), wxT(""), wxOK | wxICON_EXCLAMATION);
		dialog->ShowModal();
  
		#if DEBUG_LOG
		cout << "not connected!" << endl;
		#endif
	}
}
//==================================================================


//==================================================================
// Clase App
//==================================================================
class App : public wxApp
{
	public:
		virtual bool OnInit();
};

IMPLEMENT_APP(App)

bool App::OnInit()
{
    Frame_main *frameMain = new Frame_main(WINDOW_TITLE, WINDOW_SIZE);
    frameMain->Show(true);

    return true;
}
//==================================================================

