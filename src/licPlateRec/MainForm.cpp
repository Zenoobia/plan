#include "MainForm.h"
using namespace System;
using namespace System::Windows::Forms;
[STAThreadAttribute]
void Main() {
	Application::EnableVisualStyles;
	Application::SetCompatibleTextRenderingDefault;
	licPlaterec::MainForm form;
	Application::Run(%form);
}

