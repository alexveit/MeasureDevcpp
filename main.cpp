#include <Windows.h>
#include <stdio.h>
#include <vector>
#include <gl\glu.h>
#include <algorithm>
#include <fstream>
#include <shlobj.h>
#include <cmath>
using namespace std;

#ifndef _SHLOBJ_H_
typedef enum {
    SHGFP_TYPE_CURRENT  = 0,   // current value for user, verify it exists
    SHGFP_TYPE_DEFAULT  = 1,   // default value, may not exist
} SHGFP_TYPE;
#endif

//tell compiler that we will be using some openGL libraries
//#pragma comment( lib, "opengl32.lib" )
//#pragma comment( lib, "glu32.lib" )

/*
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
*/

class MeasureWin
{
#define FRAME_WIN_CLASS							"FRAME_WIN_CLASS"
#define CONTROLS_WIN_CLASS						"CONTROLS_WIN_CLASS"
#define GL_WIN_CLASS							"GL_WIN_CLASS"
#define MEASURMENT_DIPLAY_WIN_CLASS				"MEASURMENT_DIPLAY_WIN_CLASS"
#define MEASURMENT_DIPLAY_TOTALS_WIN_CLASS		"MEASURMENT_DIPLAY_TOTALS_WIN_CLASS"
#define TOP_INPUT_WIN_CLASS						"TOP_INPUT_WIN_CLASS"

	struct Measurment
	{
		int _w_f, _w_i, _l_f, _l_i;
		float _x, _y;
		bool _print_text, _is_clicked;
		Measurment() : _w_f(0), _w_i(0), _l_f(0), _l_i(0), _x(0.0f), _y(0.0f), _print_text(false),_is_clicked(false) {}
		Measurment(int w_f,int w_i,int l_f,int l_i) : 
			_w_f(w_f), _w_i(w_i), _l_f(l_f), _l_i(l_i), _x(0.0f), _y(0.0f), _print_text(false),_is_clicked(false) {}
		
		Measurment(const Measurment& m)
		{
			_w_f = m._w_f;
			_w_i = m._w_i;
			_l_f = m._l_f;
			_l_i = m._l_i;
			_x = m._x;
			_y = m._y;
			_print_text = m._print_text;
			_is_clicked = m._is_clicked;
		}

		Measurment& operator= (const Measurment& m)
		{
			_w_f = m._w_f;
			_w_i = m._w_i;
			_l_f = m._l_f;
			_l_i = m._l_i;
			_x = m._x;
			_y = m._y;
			_print_text = m._print_text;
			_is_clicked = m._is_clicked;
			return *this;
		}

		void add_length(const Measurment &m)
		{
			_l_f += m._l_f;
			_l_i += m._l_i;
			normalize_length();	
		}
		
		void add_width(const Measurment &m)
		{
			_w_f += m._w_f;
			_w_i += m._w_i;
			normalize_width();	
		}
		
		void draw(int name)
		{
			float w_fraction = width_fraction();
			float l_fraction = length_fraction();
			glLoadName(name);
			glPushMatrix();

				if(_is_clicked)
					glTranslatef(0,0,.01f);



				glColor3f(0.0f,0.0f,1.0f);
				glBegin(GL_QUADS);
					glVertex3f(_x, _y*-1, 0);
					glVertex3f(_x+w_fraction, _y*-1, 0);
					glVertex3f(_x+w_fraction, (l_fraction+_y)*-1, 0);
					glVertex3f(_x, (l_fraction+_y)*-1, 0);
				glEnd();

				glColor3f(0.0f,0.0f,0.0f);
				glBegin(GL_LINE_LOOP);
					glVertex3f(_x, _y*-1, 0);
					glVertex3f(_x+w_fraction, _y*-1, 0);
					glVertex3f(_x+w_fraction, (l_fraction+_y)*-1, 0);
					glVertex3f(_x, (l_fraction+_y)*-1, 0);
				glEnd();
			glPopMatrix();
		}

		float get_length_plus_y_fraction() { return _y + length_fraction(); }

		int get_square_footage() { return _w_f * _l_f; }

		void get_square_footage_string(char *buff) { sprintf(buff,"%d",_w_f*_l_f); }

		void get_square_yardage_string(char *buff)  { sprintf(buff,"%d",(_w_f*_l_f)/9); }

		void get_string(char *buff)
		{
			string str;
			if(_w_f < 10)
				str.append(" %d'");
			else
				str.append("%d'");

			if(_w_i < 10)
				str.append(" %d\"");
			else
				str.append("%d\"");

			str.append(" X ");

			if(_l_f < 10)
				str.append(" %d'");
			else
				str.append("%d'");

			if(_l_i < 10)
				str.append(" %d\"");
			else
				str.append("%d\"");
			sprintf(buff,str.c_str(),_w_f,_w_i,_l_f,_l_i);
		}
		
		float get_width_plus_x_fraction() { return _x + width_fraction(); }

		bool is_same_length(const Measurment &m)
		{
			return (_l_f == m._l_f) && (_l_i == m._l_i);
		}

		float length_fraction() const
		{
			float len = (float)_l_f;
			switch(_l_i)
			{
			case 1: len += 0.07f; break;
			case 2: len += 0.17f; break;
			case 3: len += 0.25f; break;
			case 4: len += 0.33f; break;
			case 5: len += 0.42f; break;
			case 6: len += 0.50f; break;
			case 7: len += 0.58f; break;
			case 8: len += 0.67f; break;
			case 9: len += 0.75f; break;
			case 10: len += 0.83f; break;
			case 11: len += 0.92f; break;
			}
			return len;
		}

		bool less_or_equal_then(const Measurment &m) const
		{
			int mywidth = (_w_f*12) + _w_i;
			int comparewidth = (m._w_f*12) + m._w_i;

			return mywidth <= comparewidth;
		}
		
		void normalize_length()
		{
			bool good = false;
			while(!good)
			{
				if(_l_i > 11)
				{
					_l_f++;
					_l_i -= 12;
				}
				else
					good = true;
			}
		}
		
		void normalize_width()
		{
			bool good = false;
			while(!good)
			{
				if(_w_i > 11)
				{
					_w_f++;
					_w_i -= 12;
				}
				else
					good = true;
			}
		}

		void subtract_width(const Measurment &m)
		{
			int myinchwidth = (_w_f*12) + _w_i;
			int compareinchwidth = (m._w_f*12) + m._w_i;

			myinchwidth -= compareinchwidth;

			_w_f = 0;

			bool good = false;

			while(!good)
			{
				if(myinchwidth > 11)
				{
					_w_f++;
					myinchwidth -= 12;
				}
				else
				{
					_w_i = myinchwidth;
					good = true;
				}
			}
		}

		float width_fraction() const
		{
			float wid = (float)_w_f;
			switch(_w_i)
			{
			case 1: wid += 0.07f; break;
			case 2: wid += 0.17f; break;
			case 3: wid += 0.25f; break;
			case 4: wid += 0.33f; break;
			case 5: wid += 0.42f; break;
			case 6: wid += 0.50f; break;
			case 7: wid += 0.58f; break;
			case 8: wid += 0.67f; break;
			case 9: wid += 0.75f; break;
			case 10: wid += 0.83f; break;
			case 11: wid += 0.92f; break;
			}
			return wid;
		}	
	};

	HINSTANCE _hInstance;

	HWND _frame_wnd;
	HWND _control_wnd;
	HWND _measure_display_wnd;
	HWND _measure_displayTotals_wnd;
	HWND _gl_wnd;
	HWND _topInput_wnd;

	HDC _hDC;
	HGLRC _hRC;

	GLuint	_base;	

	float x_dist;
	float y_dist;
	float z_dist;

	WORD last_xPos;
	WORD last_yPos;

	int motion_part;

	HWND _width_feet_edit;
	HWND _width_inch_edit;
	HWND _length_feet_edit;
	HWND _length_inch_edit;
	HWND _measurements_edit;
	HWND _standard_edit;
	HWND _needs_edit;
	HWND _standard_total_edit;
	HWND _calculated_needs_edit;
	HWND _ts_cn_edit;
	HWND _square_foot_edit;
	HWND _square_yard_edit;
	HWND _square_foot_waste_edit;
	HWND _square_yard_waste_edit;
	HWND _with_edit;
	HWND _total_cost_edit;
	HWND _steps_edit;

	HWND _cost_edit;
	HWND _padd_edit;
	HWND _install_edit;
	HWND _ripout_edit;

	HWND _add_button;
	HWND _invert_button;
	HWND _clear_button;
	HWND _export_button;
	HWND _padd_check;
	HWND _install_check;
	HWND _ripout_check;

	HWND last_updated_edit;


	vector<Measurment> _measurments;
	vector<Measurment> _standard;
	vector<Measurment> _needs;
	vector<Measurment> _accounted;

	Measurment _steps;

	bool _done_with_input;

	static bool sort_function(const Measurment &m1, const Measurment &m2)
	{
		return m1.length_fraction() > m2.length_fraction();
	}

	static LRESULT CALLBACK WndProcFrame (HWND frame_wnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		MeasureWin *mw = NULL;

		//set user data, in this case, a pointer to my CubeInterface class
		if(message == WM_CREATE)
		{
			//lParam contains a pointer to a CREATESTRUCT
			//(CREATESTRUCT*)lParam)->lpCreateParams contains a pointer to CubeInterface
			mw = (MeasureWin*)((CREATESTRUCT*)lParam)->lpCreateParams;

			//Actual line that stores my class in the window's user data
			SetWindowLongPtr(frame_wnd,GWLP_USERDATA,(LONG_PTR)mw);

			mw->create_child_windows(frame_wnd);

			return 0;
		}

		//Retrieve my class
		mw = (MeasureWin*)GetWindowLongPtr(frame_wnd,GWLP_USERDATA);

		switch (message)
		{
		case WM_CLOSE:
			PostQuitMessage (0);
			return 0;

		case WM_ERASEBKGND:
			return TRUE;

		case WM_SIZE:
			mw->size_frame_window();
			break;
		}

		return DefWindowProc(frame_wnd, message, wParam, lParam);
	}
	
	static LRESULT CALLBACK WndProcControls (HWND control_wnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		MeasureWin *mw = NULL;

		//set user data, in this case, a pointer to my CubeInterface class
		if(message == WM_CREATE)
		{
			//lParam contains a pointer to a CREATESTRUCT
			//(CREATESTRUCT*)lParam)->lpCreateParams contains a pointer to CubeInterface
			mw = (MeasureWin*)((CREATESTRUCT*)lParam)->lpCreateParams;

			mw->create_controls(control_wnd);

			//Actual line that stores my class in the window's user data
			SetWindowLongPtr(control_wnd,GWLP_USERDATA,(LONG_PTR)mw);
			return 0;
		}

		//Retrieve my class
		mw = (MeasureWin*)GetWindowLongPtr(control_wnd,GWLP_USERDATA);

		switch (message)
		{
		case WM_SIZE:
			mw->size_control_window();
			break;

		case WM_ERASEBKGND:
			return TRUE;

		case WM_COMMAND:
			switch(HIWORD(wParam))
			{
			case BN_CLICKED:
				mw->process_button_click(lParam);
				break;
			}
			break;
		}

		return DefWindowProc(control_wnd, message, wParam, lParam);
	}
	
	static LRESULT CALLBACK WndProcTopInput (HWND top_wnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		MeasureWin *mw = NULL;

		//set user data, in this case, a pointer to my CubeInterface class
		if(message == WM_CREATE)
		{
			//lParam contains a pointer to a CREATESTRUCT
			//(CREATESTRUCT*)lParam)->lpCreateParams contains a pointer to CubeInterface
			mw = (MeasureWin*)((CREATESTRUCT*)lParam)->lpCreateParams;

			mw->create_top_inputs(top_wnd);

			//Actual line that stores my class in the window's user data
			SetWindowLongPtr(top_wnd,GWLP_USERDATA,(LONG_PTR)mw);
			return 0;
		}

		//Retrieve my class
		mw = (MeasureWin*)GetWindowLongPtr(top_wnd,GWLP_USERDATA);

		switch (message)
		{
		case WM_SIZE:

			break;


		case WM_COMMAND:
			switch(HIWORD(wParam))
			{
			case BN_CLICKED:
				mw->process_button_click(lParam);
				break;

			case EN_UPDATE:
				mw->process_fee_filds_keystroke(lParam);
				break;
			}
			break;
		}

		return DefWindowProc(top_wnd, message, wParam, lParam);
	}

	static LRESULT CALLBACK WndProcMeasurmentDisplay (HWND meas_wnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		MeasureWin *mw = NULL;

		//set user data, in this case, a pointer to my CubeInterface class
		if(message == WM_CREATE)
		{
			//lParam contains a pointer to a CREATESTRUCT
			//(CREATESTRUCT*)lParam)->lpCreateParams contains a pointer to CubeInterface
			mw = (MeasureWin*)((CREATESTRUCT*)lParam)->lpCreateParams;

			mw->create_MeasurmentDisplay(meas_wnd);

			//Actual line that stores my class in the window's user data
			SetWindowLongPtr(meas_wnd,GWLP_USERDATA,(LONG_PTR)mw);
			return 0;
		}

		//Retrieve my class
		mw = (MeasureWin*)GetWindowLongPtr(meas_wnd,GWLP_USERDATA);

		switch (message)
		{
		case WM_SIZE:
			mw->size_MeasurmentDisplay_window();
			break;

		}

		return DefWindowProc(meas_wnd, message, wParam, lParam);
	}
	
	static LRESULT CALLBACK WndProcMeasurmentDisplayTotals (HWND meas_total_wnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		MeasureWin *mw = NULL;

		//set user data, in this case, a pointer to my CubeInterface class
		if(message == WM_CREATE)
		{
			//lParam contains a pointer to a CREATESTRUCT
			//(CREATESTRUCT*)lParam)->lpCreateParams contains a pointer to CubeInterface
			mw = (MeasureWin*)((CREATESTRUCT*)lParam)->lpCreateParams;

			mw->create_MeasurmentDisplayTotals(meas_total_wnd);

			//Actual line that stores my class in the window's user data
			SetWindowLongPtr(meas_total_wnd,GWLP_USERDATA,(LONG_PTR)mw);
			return 0;
		}

		//Retrieve my class
		mw = (MeasureWin*)GetWindowLongPtr(meas_total_wnd,GWLP_USERDATA);

		switch (message)
		{
		case WM_SIZE:
			//mw->size_MeasurmentDisplay_window();
			break;

		case WM_COMMAND:
			switch(HIWORD(wParam))
			{
			case BN_CLICKED:
				mw->process_button_click(lParam);
				break;
			}
			break;

		}

		return DefWindowProc(meas_total_wnd, message, wParam, lParam);
	}

	static LRESULT CALLBACK WndProcGL (HWND gl_wnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		MeasureWin *mw = NULL;

		//set user data, in this case, a pointer to my CubeInterface class
		if(message == WM_CREATE)
		{
			//lParam contains a pointer to a CREATESTRUCT
			//(CREATESTRUCT*)lParam)->lpCreateParams contains a pointer to CubeInterface
			mw = (MeasureWin*)((CREATESTRUCT*)lParam)->lpCreateParams;

			//Actual line that stores my class in the window's user data
			SetWindowLongPtr(gl_wnd,GWLP_USERDATA,(LONG_PTR)mw);
			return 0;
		}

		//Retrieve my class
		mw = (MeasureWin*)GetWindowLongPtr(gl_wnd,GWLP_USERDATA);

		switch (message)
		{
		case WM_SIZE:
			mw->size_gl_window();
			break;
		case WM_LBUTTONDOWN:
			SetFocus(mw->_gl_wnd);
			mw->gl_select(LOWORD(lParam),HIWORD(lParam),true);
			break;
		case WM_LBUTTONUP:
			mw->process_left_mouse_up();
			break;
		case WM_RBUTTONDOWN:
			SetFocus(mw->_gl_wnd);
			break;
		case WM_MOUSEMOVE:
			mw->process_mouse_motion(wParam,lParam);
			break;
		case WM_MOUSEWHEEL:
			mw->process_wheel(HIWORD(wParam));
			break;
		case WM_KEYDOWN:
			mw->process_keydown(wParam);
			break;

		case WM_PAINT:
			mw->draw_gl_scene();
			break;

		case WM_ERASEBKGND:
			return TRUE;
		}

		return DefWindowProc(gl_wnd, message, wParam, lParam);
	}

	void build_font()								// Build Our Bitmap Font
	{
		HFONT	font;										// Windows Font ID
		HFONT	oldfont;									// Used For Good House Keeping

		_base = glGenLists(96);								// Storage For 96 Characters

		font = CreateFont(	-20,							// Height Of Font
							0,								// Width Of Font
							0,								// Angle Of Escapement
							0,								// Orientation Angle
							FW_BOLD,						// Font Weight
							FALSE,							// Italic
							FALSE,							// Underline
							FALSE,							// Strikeout
							ANSI_CHARSET,					// Character Set Identifier
							OUT_TT_PRECIS,					// Output Precision
							CLIP_DEFAULT_PRECIS,			// Clipping Precision
							ANTIALIASED_QUALITY,			// Output Quality
							FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
							"Courier New");					// Font Name

		oldfont = (HFONT)SelectObject(_hDC, font);           // Selects The Font We Want
		wglUseFontBitmaps(_hDC, 32, 96, _base);				// Builds 96 Characters Starting At Character 32
		SelectObject(_hDC, oldfont);							// Selects The Font We Want
		DeleteObject(font);									// Delete The Font
	}

	void calculate_totals()
	{
		Measurment remaining;
		remaining._w_f = 12;

		vector<Measurment> ordered_needs(_needs);
		_accounted.clear();

		sort(ordered_needs.begin(), ordered_needs.end(),sort_function);

		bool good = false;

		while(!good)
		{
			for(unsigned i = 0; i < ordered_needs.size(); i++)
			{
				if(ordered_needs[i].less_or_equal_then(remaining))
				{
					ordered_needs[i]._x = remaining._x;
					ordered_needs[i]._y = remaining._y;

					remaining.subtract_width(ordered_needs[i]);

					remaining._x += ordered_needs[i].width_fraction();

					_accounted.push_back(ordered_needs[i]);

					ordered_needs.erase(ordered_needs.begin()+i);

					i--;
				}
			}

			if(ordered_needs.size() > 0)
			{
				remaining._w_f = 12;
				remaining._w_i = 0;
				remaining._x = 0;
				remaining._y = get_biggest_y_from_accounted();
			}
			else
				good = true;
		}
	}

	void clear_all()
	{
		_measurments.clear();
		_standard.clear();
		_needs.clear();
		_accounted.clear();
		clear_inputs();
		SetWindowText(_cost_edit,"");
		SetWindowText(_steps_edit,"");
		update_display();
		LRESULT res = SendMessage(_padd_check,BM_GETCHECK,(WPARAM)0,(LPARAM)0);
		if(res == BST_CHECKED)
			SendMessage(_padd_check,BM_CLICK,(WPARAM)0,(LPARAM)0);
		res = SendMessage(_install_check,BM_GETCHECK,(WPARAM)0,(LPARAM)0);
		if(res == BST_CHECKED)
			SendMessage(_install_check,BM_CLICK,(WPARAM)0,(LPARAM)0);
		res = SendMessage(_ripout_check,BM_GETCHECK,(WPARAM)0,(LPARAM)0);
		if(res == BST_CHECKED)
			SendMessage(_ripout_check,BM_CLICK,(WPARAM)0,(LPARAM)0);
		set_default_dists();
		_done_with_input = false;
	}

	void clear_inputs()
	{
		SetWindowText(_width_feet_edit,"");
		SetWindowText(_width_inch_edit,"");
		SetWindowText(_length_feet_edit,"");
		SetWindowText(_length_inch_edit,"");
	}

	void consolidate_needs()
	{
		for (unsigned i = 0; i < _needs.size(); i++)
		{
			for (unsigned j = i+1; j < _needs.size(); j++)
			{
				if(_needs[i].is_same_length(_needs[j]))
				{
					_needs[i].add_width(_needs[j]);
					_needs.erase(_needs.begin()+j);
				}
			}
		}
	}

	void consolidate_standards()
	{
		int val = 0;
		for(unsigned i = 0; i < _needs.size(); i++)
		{
			if(_needs[i]._w_f > 11)
			{
				Measurment *pm = &_needs[i];
				bool good = false;
				while(!good)
				{
					if(pm->_w_f > 11)
					{
						_standard.push_back(Measurment(12,0,pm->_l_f,pm->_l_i));
						pm->_w_f -= 12;
					}
					else if(pm->_w_f == 0)
					{
						if(pm->_w_i == 0)
						{
							_needs.erase(_needs.begin()+i);	
						}
						good = true;
					}
					else
					{
						good = true;
					}
				}
			}
		}
	}

	void create_child_windows(HWND frame_wnd)
	{
		_control_wnd = CreateWindow (CONTROLS_WIN_CLASS, NULL,
			WS_CHILD | WS_VISIBLE,0, 0, 390, 200,
			frame_wnd, NULL, _hInstance, this);

		_gl_wnd = CreateWindow (GL_WIN_CLASS, NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | 
			WS_CLIPCHILDREN | WS_BORDER,390, 0, 200, 200,
			frame_wnd, NULL, _hInstance, this);

		set_up_gl_context();
	}
	
	void create_controls(HWND control_wnd)
	{

		RECT c_rect;
		GetClientRect(control_wnd,&c_rect);

		_topInput_wnd = CreateWindow (TOP_INPUT_WIN_CLASS, NULL,
			WS_CHILD | WS_VISIBLE,0, 0, c_rect.right, 140,
			control_wnd, NULL, _hInstance, this);

		_measure_display_wnd = CreateWindow (MEASURMENT_DIPLAY_WIN_CLASS, NULL,
			WS_CHILD | WS_VISIBLE,0, 400, 50, 50,
			control_wnd, NULL, _hInstance, this);

	}

	//NEED WORK
	void create_html_document()
	{
		ofstream myfile;
		char buff[50];
		TCHAR appData[256];
		sprintf(appData,"example.html");
		if (SUCCEEDED(SHGetFolderPath(NULL,CSIDL_DESKTOPDIRECTORY | CSIDL_FLAG_CREATE, 
			NULL, SHGFP_TYPE_CURRENT, appData))) 
		{
			sprintf(appData,"%s\\example.html",appData);
		}

		myfile.open (appData);
		
		myfile << "<html><body>";

		int totalsqft = 0;
		int totalsqyd = 0;

		TCHAR ft[30], yd[30], tr[30];

		get_total_square_footage_string(ft);
		get_total_square_yardage_string(yd);
		get_total_roll_string(tr);

		myfile << "<ul><li><b>Measurements</b></li><ul>";
		for(unsigned i = 0; i < _measurments.size(); i++)
		{
			_measurments[i].get_string(buff);
			myfile << "<li>" << buff << "</li>";
		}
		myfile << "</ul></ul>";

		myfile << "<p><b>Total Roll:</b> " << tr << "</p>";

		myfile << "<table style='border-style:solid; border-width:1px;'><tr><td colspan='3'><b>Area</b></td></tr>";

		myfile << "<tr><td><br></td><td>Sq ft</td><td>Sq yd</td></tr>";

		myfile << "<tr><td><b>Total</b></td><td>" << ft << "</td><td>" << yd << "</td></tr>";

		myfile << "<tr><td><b>Waste</b></td><td>" << get_waste_footage_int() << "</td><td>" << get_waste_yardage_int() << "</td></tr>";

		myfile << "</table>";

		myfile << "</body></html>";

		myfile.close();
	}

	void create_MeasurmentDisplay(HWND meas_wnd)
	{
		CreateWindow("STATIC", "Measurments",WS_VISIBLE | WS_CHILD,
                      10, 0, 95, 18,meas_wnd,NULL, _hInstance, NULL);

		_measurements_edit = CreateWindow("EDIT", "",
                      WS_VISIBLE | WS_CHILD | ES_READONLY | ES_MULTILINE | 
					  WS_VSCROLL | ES_LEFT, 10, 20, 120, 265,
					  meas_wnd,NULL, _hInstance, NULL);

		CreateWindow("STATIC", "Standard",WS_VISIBLE | WS_CHILD,
                      135, 0, 65, 18, meas_wnd,NULL, _hInstance, NULL);

		_standard_edit = CreateWindow("EDIT", "",
                      WS_VISIBLE | WS_CHILD | ES_READONLY | ES_MULTILINE | 
					  WS_VSCROLL | ES_LEFT, 135, 20, 120, 265,
					  meas_wnd,NULL, _hInstance, NULL);

		CreateWindow("STATIC", "Needs",WS_VISIBLE | WS_CHILD,
                      260, 0, 50, 18,meas_wnd,NULL, _hInstance, NULL);

		_needs_edit = CreateWindow("EDIT", "",
                      WS_VISIBLE | WS_CHILD | ES_READONLY | ES_MULTILINE | 
					  WS_VSCROLL | ES_LEFT, 260, 20, 120, 265,
					  meas_wnd,NULL, _hInstance, NULL);

		_measure_displayTotals_wnd = CreateWindow (MEASURMENT_DIPLAY_TOTALS_WIN_CLASS, NULL,
			WS_CHILD | WS_VISIBLE,0, 100, 50, 50,
			meas_wnd, NULL, _hInstance, this);

		HFONT hFont=CreateFont(0,6,0,0,FW_HEAVY,0,0,0,0,0,0,0,0,TEXT("Courier New"));
		SendMessage(_measurements_edit,WM_SETFONT,(WPARAM)hFont,0);
		SendMessage(_standard_edit,WM_SETFONT,(WPARAM)hFont,0);
		SendMessage(_needs_edit,WM_SETFONT,(WPARAM)hFont,0);

	}
	
	void create_MeasurmentDisplayTotals(HWND meas_total_wnd)
	{

		_export_button = CreateWindow("BUTTON", "Export",WS_VISIBLE | WS_CHILD,
			10, 10, 90, 35,meas_total_wnd,NULL, _hInstance, NULL);

		CreateWindow("STATIC", "Total Standards",WS_VISIBLE | WS_CHILD,
                      135, 0, 100, 38, meas_total_wnd,NULL, _hInstance, NULL);

		CreateWindow("STATIC", "Calculated Needs",WS_VISIBLE | WS_CHILD,
                      260, 0, 100, 38,meas_total_wnd,NULL, _hInstance, NULL);

		_standard_total_edit = CreateWindow("EDIT", "",
                      WS_VISIBLE | WS_CHILD | ES_READONLY | ES_LEFT,
					  135, 38, 120, 20,meas_total_wnd,NULL, _hInstance, NULL);

		_calculated_needs_edit = CreateWindow("EDIT", "", //_needs_edit = 
                      WS_VISIBLE | WS_CHILD | ES_READONLY | ES_LEFT,
					  260, 38, 120, 20, meas_total_wnd,NULL, _hInstance, NULL);

		_clear_button = CreateWindow("BUTTON", "Clear All",WS_VISIBLE | WS_CHILD,
			272, 68, 90, 35,meas_total_wnd,NULL, _hInstance, NULL);


		CreateWindow("STATIC", "Total Roll",WS_VISIBLE | WS_CHILD,
                      135, 65, 80, 20, meas_total_wnd,NULL, _hInstance, NULL);

		_ts_cn_edit = CreateWindow("EDIT", "",
                      WS_VISIBLE | WS_CHILD | ES_READONLY | ES_LEFT,
					  135, 85, 120, 20, meas_total_wnd,NULL, _hInstance, NULL);


		CreateWindow ("BUTTON", "Area",WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
			5, 110, 195, 100,meas_total_wnd, NULL, _hInstance, NULL);


		CreateWindow("STATIC", "Sq ft",WS_VISIBLE | WS_CHILD,
                      65, 130, 50, 20, meas_total_wnd,NULL, _hInstance, NULL);

		CreateWindow("STATIC", "Sq yd",WS_VISIBLE | WS_CHILD,
                      130, 130, 50, 20, meas_total_wnd,NULL, _hInstance, NULL);

		CreateWindow("STATIC", "Total",WS_VISIBLE | WS_CHILD,
                      15, 150, 50, 20, meas_total_wnd,NULL, _hInstance, NULL);

		_square_foot_edit = CreateWindow("EDIT", "",
                      WS_VISIBLE | WS_CHILD | ES_READONLY | ES_LEFT,
					  65, 150, 60, 20, meas_total_wnd,NULL, _hInstance, NULL);

		_square_yard_edit = CreateWindow("EDIT", "",
                      WS_VISIBLE | WS_CHILD | ES_READONLY | ES_LEFT,
					  130, 150, 60, 20, meas_total_wnd,NULL, _hInstance, NULL);

		int buff = 30;

		CreateWindow("STATIC", "Waste",WS_VISIBLE | WS_CHILD,
                      15, 150+buff, 50, 20, meas_total_wnd,NULL, _hInstance, NULL);

		_square_foot_waste_edit = CreateWindow("EDIT", "",
                      WS_VISIBLE | WS_CHILD | ES_READONLY | ES_LEFT,
					  65, 150+buff, 60, 20, meas_total_wnd,NULL, _hInstance, NULL);

		_square_yard_waste_edit = CreateWindow("EDIT", "",
                      WS_VISIBLE | WS_CHILD | ES_READONLY | ES_LEFT,
					  130, 150+buff, 60, 20, meas_total_wnd,NULL, _hInstance, NULL);

		CreateWindow ("BUTTON", "Estimate",WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
			210, 110, 170, 70,meas_total_wnd, NULL, _hInstance, NULL);


		CreateWindow("STATIC", "Cost:",WS_VISIBLE | WS_CHILD,
            218, 130, 40, 20, meas_total_wnd,NULL, _hInstance, NULL);

		_total_cost_edit = CreateWindow("EDIT", "",
            WS_VISIBLE | WS_CHILD | ES_READONLY | ES_LEFT,
			258, 130, 117, 20, meas_total_wnd,NULL, _hInstance, NULL);

		CreateWindow("STATIC", "With:",WS_VISIBLE | WS_CHILD,
            218, 155, 40, 20, meas_total_wnd,NULL, _hInstance, NULL);

		_with_edit = CreateWindow("EDIT", "",
            WS_VISIBLE | WS_CHILD | ES_READONLY | ES_LEFT,
			258, 155, 117, 20, meas_total_wnd,NULL, _hInstance, NULL);


		HFONT hFont=CreateFont(0,6,0,0,FW_HEAVY,0,0,0,0,0,0,0,0,TEXT("Courier New"));

		SendMessage(_with_edit,WM_SETFONT,(WPARAM)hFont,0);
		SendMessage(_total_cost_edit,WM_SETFONT,(WPARAM)hFont,0);
		SendMessage(_square_foot_edit,WM_SETFONT,(WPARAM)hFont,0);
		SendMessage(_square_yard_edit,WM_SETFONT,(WPARAM)hFont,0);
		SendMessage(_square_foot_waste_edit,WM_SETFONT,(WPARAM)hFont,0);
		SendMessage(_square_yard_waste_edit,WM_SETFONT,(WPARAM)hFont,0);
		SendMessage(_standard_total_edit,WM_SETFONT,(WPARAM)hFont,0);
		SendMessage(_calculated_needs_edit,WM_SETFONT,(WPARAM)hFont,0);
		SendMessage(_ts_cn_edit,WM_SETFONT,(WPARAM)hFont,0);
	}

	void create_top_inputs(HWND top_wnd)
	{

		CreateWindow ("BUTTON", "Measurement Input",
			WS_CHILD | WS_VISIBLE | BS_GROUPBOX,10, 10, 265, 50,
			top_wnd, NULL, _hInstance, NULL);

		_width_feet_edit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
                      WS_VISIBLE | WS_CHILD | ES_NUMBER | ES_LEFT | WS_TABSTOP,
                      17, 30, 35, 20,top_wnd,NULL, _hInstance, NULL);

		CreateWindow("STATIC", "'",WS_VISIBLE | WS_CHILD,
                      54, 30, 5, 20,top_wnd,NULL, _hInstance, NULL);

		_width_inch_edit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
                      WS_VISIBLE | WS_CHILD | ES_NUMBER | ES_LEFT | WS_TABSTOP,
                      61, 30, 30, 20,top_wnd,NULL, _hInstance, NULL);

		CreateWindow("STATIC", "\"  X",WS_VISIBLE | WS_CHILD,
                      93, 30, 30, 20,top_wnd,NULL, _hInstance, NULL);

		int buff = 110;

		_length_feet_edit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
                      WS_VISIBLE | WS_CHILD | ES_NUMBER | ES_LEFT | WS_TABSTOP,
                      17+buff, 30, 35, 20,top_wnd,NULL, _hInstance, NULL);

		CreateWindow("STATIC", "'",WS_VISIBLE | WS_CHILD,
                      54+buff, 30, 5, 20,top_wnd,NULL, _hInstance, NULL);

		_length_inch_edit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
                      WS_VISIBLE | WS_CHILD | ES_NUMBER | ES_LEFT | WS_TABSTOP,
                      61+buff, 30, 30, 20,top_wnd,NULL, _hInstance, NULL);

		CreateWindow("STATIC", "\"",WS_VISIBLE | WS_CHILD,
                      93+buff, 30, 8, 20,top_wnd,NULL, _hInstance, NULL);

		_add_button = CreateWindow("BUTTON", "Add",WS_VISIBLE | WS_CHILD | WS_TABSTOP,
                      105+buff, 30, 50, 20,top_wnd,NULL, _hInstance, NULL);

		_invert_button = CreateWindow("BUTTON", "Invert",WS_VISIBLE | WS_CHILD,
                      180+buff, 21, 83, 35,top_wnd,NULL, _hInstance, NULL);

		CreateWindow ("BUTTON", "Fees",
			WS_CHILD | WS_VISIBLE | BS_GROUPBOX,10, 60, 370, 70,
			top_wnd, NULL, _hInstance, NULL);

		CreateWindow("STATIC", "Cost",WS_VISIBLE | WS_CHILD,
                      17, 80, 40, 20,top_wnd,NULL, _hInstance, NULL);

		_cost_edit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
                      WS_VISIBLE | WS_CHILD | ES_LEFT | WS_TABSTOP,
                      17, 100, 50, 20,top_wnd,NULL, _hInstance, NULL);

		CreateWindow("STATIC", "Steps",WS_VISIBLE | WS_CHILD ,
                      85, 80, 40, 20,top_wnd,NULL, _hInstance, NULL);

		_steps_edit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
            WS_VISIBLE | WS_CHILD | ES_NUMBER | ES_LEFT | WS_TABSTOP,
            85, 100, 50, 20,top_wnd,NULL, _hInstance, NULL);


		_padd_check = CreateWindow ("BUTTON", "Pad",
			WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			160, 80, 75, 20,top_wnd, NULL, _hInstance, NULL);

		_padd_edit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
            WS_VISIBLE | WS_DISABLED | WS_CHILD | ES_LEFT | WS_TABSTOP,
            160, 100, 50, 20,top_wnd,NULL, _hInstance, NULL);

		_install_check = CreateWindow ("BUTTON", "Install",
			WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			235, 80, 60, 20,top_wnd, NULL, _hInstance, NULL);

		_install_edit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
            WS_VISIBLE | WS_DISABLED | WS_CHILD | ES_LEFT | WS_TABSTOP,
            235, 100, 50, 20,top_wnd,NULL, _hInstance, NULL);

		_ripout_check = CreateWindow ("BUTTON", "Ripout",
			WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			310, 80, 65, 20,top_wnd, NULL, _hInstance, NULL);

		_ripout_edit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
            WS_VISIBLE | WS_DISABLED | WS_CHILD | ES_LEFT,
            310, 100, 50, 20,top_wnd,NULL, _hInstance, NULL);
	}
	
	void draw_gl_scene()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
		glLoadIdentity();									// Reset The Current Modelview Matrix
		
		if(!(_accounted.size() > 0))
		{
			SwapBuffers(_hDC);
			return;
		}

		glPushMatrix();

		glTranslatef(x_dist,y_dist,z_dist);

		draw_red_backgroud();

		char buff[20];
		for(unsigned i = 0; i < _accounted.size(); i++)
		{
			_accounted[i].draw(i+1);

			if(_accounted[i]._print_text)
			{
				//print dimentions
				glPushMatrix();
					glColor3f(0.0f,0.0f,0.0f);
					_accounted[i].get_string(buff);
					glRasterPos3f(_accounted[i]._x, (_accounted[i]._y*-1)-1.5f,0.2f);
 					gl_print(buff);
				glPopMatrix();
			}
			
		}

		glPopMatrix();

		SwapBuffers(_hDC);
	}

	void draw_red_backgroud()
	{
		float y = get_biggest_y_from_accounted()*-1;
		float myzdist = z_dist;
		myzdist -= 0.1f;

		glPushMatrix();
			

			glTranslatef(0,0,-0.01f);
			
			glColor3f(1.0f,0.0f,0.0f);
			glBegin(GL_QUADS);
				glVertex3f(0, 0, 0);
				glVertex3f(12, 0, 0);
				glVertex3f(12, y, 0);
				glVertex3f(0, y, 0);
			glEnd();

			float temp = y*-1;

			float myvar = 0;

			glColor3f(0.0f,0.0f,0.0f);

			while(true)
			{
				glBegin(GL_LINES);
					glVertex3f(0, myvar, 0);
					glVertex3f(12, myvar, 0);
				glEnd();
				temp -= 1;
				myvar -= 1;
				if(temp < 0)
					break;
			}


			myvar = 0;
			while(true)
			{
				glBegin(GL_LINES);
					glVertex3f(myvar, 0, 0);
					glVertex3f(myvar, y, 0);
				glEnd();
				myvar += 1;
				if(myvar > 12)
					break;
			}

		glPopMatrix();
	}
	
	void get_additions_check(bool *pad,bool *install,bool *rip)
	{
		*pad = SendMessage(_padd_check,BM_GETCHECK,(WPARAM)0,(LPARAM)0) == BST_CHECKED;
		*install = SendMessage(_install_check,BM_GETCHECK,(WPARAM)0,(LPARAM)0) == BST_CHECKED;
		*rip = SendMessage(_ripout_check,BM_GETCHECK,(WPARAM)0,(LPARAM)0) == BST_CHECKED;
	}

	float get_biggest_y_from_accounted()
	{
		float len = 0;
		float temp = 0;

		for(unsigned i = 0; i < _accounted.size(); i++)
		{
			temp = _accounted[i].get_length_plus_y_fraction();
			if(temp > len)
				len = temp;
		}

		return len;
	}
	
	Measurment get_calculated_needs_length()
	{
		Measurment m;

		float len = get_biggest_y_from_accounted();

		float fractpart, intpart;

		fractpart = modf (len , &intpart);

		m._l_f = (int)intpart;
	
		if(fractpart >= .91f)		m._l_i = 11;
		else if(fractpart >= .82f)	m._l_i = 10;
		else if(fractpart >= .74f)	m._l_i = 9;
		else if(fractpart >= .66f)	m._l_i = 8;
		else if(fractpart >= .57f)	m._l_i = 7;
		else if(fractpart >= .49f)	m._l_i = 6;
		else if(fractpart >= .41f)	m._l_i = 5;
		else if(fractpart >= .32f)	m._l_i = 4;
		else if(fractpart >= .24f)	m._l_i = 3;
		else if(fractpart >= .16f)	m._l_i = 2;
		else if(fractpart >= .06f)	m._l_i = 1;
		else						m._l_i = 0;

		return m;
	}

	float get_motion_dist()
	{
		RECT gl_rect;
		GetClientRect(_gl_wnd, &gl_rect);
		if(gl_rect.right > gl_rect.bottom)
			return (z_dist*-1)/gl_rect.right;
		
		return (z_dist*-1)/gl_rect.bottom;

	}

	int get_needs_sqft()
	{
		int sqft = 0;
		for(unsigned i = 0; i < _needs.size(); i++)
		{
			sqft += _needs[i].get_square_footage();
		}
		return sqft;
	}

	int get_standard_sqft()
	{
		int sqft = 0;
		for(unsigned i = 0; i < _standard.size(); i++)
		{
			sqft += _standard[i].get_square_footage();
		}
		sqft += _steps.get_square_footage();
		return sqft;
	}
	
	void get_total_roll_string(TCHAR *buff)
	{
		if(_standard.size() > 0 || _needs.size() > 0)
		{
			Measurment m = get_total_standard_length();
			m.add_length(get_calculated_needs_length());
			m._w_f = 12;
			m.get_string(buff);
		}
	}

	void get_total_square_footage_string(TCHAR *buff)
	{
		if(_standard.size() > 0 || _needs.size() > 0)
		{
			Measurment m = get_total_standard_length();
			m.add_length(get_calculated_needs_length());
			m.add_length(_steps);
			m._w_f = 12;
		
			m.get_square_footage_string(buff);
		}
	}

	void get_total_square_yardage_string(TCHAR *buff)
	{
		if(_standard.size() > 0 || _needs.size() > 0)
		{
			Measurment m = get_total_standard_length();
			m.add_length(get_calculated_needs_length());
			m.add_length(_steps);
			m._w_f = 12;

			m.get_square_yardage_string(buff);
		}
	}

	Measurment get_total_standard_length()
	{
		Measurment m;
		for(unsigned i = 0; i < _standard.size(); i++)
		{
			m.add_length(_standard[i]);
		}
		return m;
	}
	
	int get_waste_footage_int()
	{
		if(_standard.size() > 0 || _needs.size() > 0)
		{
			Measurment ms = get_total_standard_length();
			ms.add_length(_steps);
			Measurment mn = get_calculated_needs_length();

			int used_sqft = get_standard_sqft();
			int needs_sqft = get_needs_sqft();

			Measurment total;

			total._w_f = 12;

			total.add_length(ms);
			total.add_length(mn);

			return total.get_square_footage() - (used_sqft + needs_sqft);
		}

		return 0;
	}
	
	int get_waste_yardage_int()
	{
		if(_standard.size() > 0 || _needs.size() > 0)
		{
			Measurment ms = get_total_standard_length();
			ms.add_length(_steps);
			Measurment mn = get_calculated_needs_length();

			int used_sqft = get_standard_sqft();
			int needs_sqft = get_needs_sqft();

			Measurment total;

			total._w_f = 12;

			total.add_length(ms);
			total.add_length(mn);

			return (total.get_square_footage() - (used_sqft + needs_sqft))/9;
		}

		return 0;
	}

	void gl_print(const char *fmt, ...)					// Custom GL "Print" Routine
	{
		char		text[256];								// Holds Our String
		va_list		ap;										// Pointer To List Of Arguments

		if (fmt == NULL)									// If There's No Text
			return;											// Do Nothing

		va_start(ap, fmt);									// Parses The String For Variables
			vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
		va_end(ap);											// Results Are Stored In Text

		glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
		glListBase(_base - 32);								// Sets The Base Character to 32
		glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
		glPopAttrib();										// Pops The Display List Bits
	}

	void gl_select(int x, int y, bool leftclick)
	{
		if(_accounted.size() == 0)
			return;

		GLuint buff[256] = {0};
		GLint hits, view[4];

		RECT gl_rect;
		GetClientRect(_gl_wnd,&gl_rect);

		//This choose the buffer where store the values for the selection data
		glSelectBuffer(256, buff);
 
		//This retrieve info about the viewport
		glGetIntegerv(GL_VIEWPORT, view);
 
		//Switching in selecton mode
		glRenderMode(GL_SELECT);
 
		//Clearing the name's stack
		//This stack contains all the info about the objects
		glInitNames();
 
		//Now fill the stack with one element (or glLoadName will generate an error)
		glPushName(0);
 
		//Now modify the vieving volume, restricting selection area around the cursor
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
 
		//restrict the draw to an area around the cursor
		gluPickMatrix(x, gl_rect.bottom-y, .1f, .1f, view);
		gluPerspective(45.0f,(GLfloat)gl_rect.right/(GLfloat)gl_rect.bottom,0.1f,500.0f);
 
		//Draw the objects onto the screen
		glMatrixMode(GL_MODELVIEW);
 
		//draw only the names in the stack, and fill the array
		SwapBuffers(_hDC);
		draw_gl_scene();
 
		//Do you remeber? We do pushMatrix in PROJECTION mode
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
 
		//get number of objects drawed in that area
		//and return to render mode
		hits = glRenderMode(GL_RENDER);
 
		//process hit
		if(hits > 0)
			process_hits(hits,buff,leftclick);
		else
		{
			for(unsigned i = 0; i < _accounted.size(); i++)
				_accounted[i]._print_text = false;
		}


		glMatrixMode(GL_MODELVIEW);
	}
	
	void init_gl()							// All Setup For OpenGL Goes Here
	{
		glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
		
		glClearColor(.94f, .94f, .94f, 0.5f);				// Black Background
		glClearDepth(1.0f);									// Depth Buffer Setup
		glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
		glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

		build_font();
	}

	void invert_all()
	{
		vector<Measurment> temp(_measurments);
		_measurments.clear();
		_standard.clear();
		_needs.clear();
		for(unsigned i = 0; i < temp.size(); i++)
		{
			_measurments.push_back(
				Measurment(
					temp[i]._l_f,
					temp[i]._l_i,
					temp[i]._w_f,
					temp[i]._w_i
				)
			);
			process_input();
		}
	}

	bool is_numeric(char *buff, int size)
	{
		for(int i = 0; i < size; i++)
		{
			if(buff[i] < '0' || buff[i] > '9')
				return false;
		}
		return true;
	}

	void kill_font()									// Delete The Font List
	{
		glDeleteLists(_base, 96);					// Delete All 96 Characters
	}

	void kill_gl_window()								// Properly Kill The Window
	{

		if (_hRC)											// Do We Have A Rendering Context?
		{
			if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
			{
				MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			}

			if (!wglDeleteContext(_hRC))						// Are We Able To Delete The RC?
			{
				MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			}
			_hRC=NULL;										// Set RC To NULL
		}

		if (_hDC && !ReleaseDC(_gl_wnd,_hDC))					// Are We Able To Release The DC
		{
			MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			_hDC=NULL;										// Set DC To NULL
		}

		if (_gl_wnd && !DestroyWindow(_gl_wnd))					// Are We Able To Destroy The Window?
		{
			MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			_gl_wnd=NULL;										// Set hWnd To NULL
		}

		if (!UnregisterClass(GL_WIN_CLASS,_hInstance))			// Are We Able To Unregister Class
		{
			MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			_hInstance=NULL;									// Set hInstance To NULL
		}

		kill_font();
	}

	void process_button_click(LPARAM lParam)
	{
		if((HWND)lParam == _add_button)
		{
			if(validate_input())
			{
				retrieve_measurment();
				process_input();
				clear_inputs();	
			}
		}
		else if((HWND)lParam == _invert_button)
		{
			if(!_done_with_input)
			{
				int res = MessageBox(_control_wnd,"Are you done inputting measurments?", "", MB_YESNO|MB_ICONQUESTION);
				if(res == IDYES)
				{
					invert_all();
					_done_with_input = true;
				}
			}
			else
			{
				invert_all();
			}
		}
		else if((HWND)lParam == _clear_button)
		{
			clear_all();
			
		}
		else if((HWND)lParam == _export_button)
		{
			create_html_document();
		}
		else if((HWND)lParam == _padd_check)
		{
			process_padd_ckeck();
		}
		else if((HWND)lParam == _install_check)
		{
			process_install_ckeck();
		}
		else if((HWND)lParam == _ripout_check)
		{
			process_ripout_ckeck();
		}
		SetFocus(_width_feet_edit);
		SetActiveWindow(_width_feet_edit);
	}
	
	void process_fee_filds_keystroke(LPARAM lParam)
	{
		if((HWND)lParam == last_updated_edit)
		{
			last_updated_edit = NULL;
			return;
		}

		if((HWND)lParam == _cost_edit || 
			(HWND)lParam == _steps_edit || 
			(HWND)lParam == _padd_edit || 
			(HWND)lParam == _install_edit || 
			(HWND)lParam == _ripout_edit)
		{
			TCHAR text[30];
			GetWindowText((HWND)lParam,text,30);
			size_t size = strlen(text);
			for(size_t i = 0; i < size; i++)
			{
				if((text[i] < '0' || text[i] > '9') && text[i] != '.')
				{
					text[i] = '\0';
					break;
				}
				if((HWND)lParam == _steps_edit && text[i] == '.')
				{
					text[i] = '\0';
					break;
				}
			}
			last_updated_edit = (HWND)lParam;
			SetWindowText((HWND)lParam,text);
			SendMessage((HWND)lParam,EM_SETSEL,(WPARAM)size,(LPARAM)size);

			if((HWND)lParam == _steps_edit)
			{
				update_steps(text);
				update_standard_display();
			}

			update_display_dynamic();
		}
	}

	void process_hits(GLint hits, GLuint buff[], bool leftclick)
	{
		GLuint *ptr = (GLuint *) buff;
		GLuint minZ = 0xffffffff;
		GLuint names,numberOfNames, *ptrNames = 0;
		for (int i = 0; i < hits; i++)
		{	
			names = *ptr;
			ptr++;
			if (*ptr < minZ)
			{
				numberOfNames = names;
				minZ = *ptr;
				ptrNames = ptr+2;
			}
			ptr += names+2;
		}

		for(unsigned i = 0; i < _accounted.size(); i++)
		{
			if(i+1 == *ptrNames)
			{
				_accounted[i]._print_text = true;
				if(leftclick)
				{
					_accounted[i]._is_clicked = true;
					motion_part = *ptrNames;
				}
			}
			else
			{
				if(leftclick)
				{
					_accounted[i]._is_clicked = false;
				}
				_accounted[i]._print_text = false;
			}
		}
	}
	
	void process_input()
	{
		split_needs();
		consolidate_needs();
		consolidate_standards();
		calculate_totals();
		update_display();
	}
	
	void process_install_ckeck()
	{
		LRESULT res = SendMessage(_install_check,BM_GETCHECK,(WPARAM)0,(LPARAM)0);

		if(res == BST_CHECKED)
		{
			EnableWindow(_install_edit,TRUE);
			SetWindowText(_install_edit,"3.50");
		}
		else
		{
			EnableWindow(_install_edit,FALSE);
			SetWindowText(_install_edit,"");
		}
		update_additions_diplay();
	}

	void process_keydown(WPARAM wParam)
	{
		switch(wParam)
		{
		case VK_ADD: z_dist += 1.0f; break;
		case VK_SUBTRACT: z_dist -= 1.0f; break;
		case VK_UP: y_dist += 1.0f; break;
		case VK_DOWN: y_dist -= 1.0f; break;
		case VK_LEFT: x_dist -= 1.0f; break;
		case VK_RIGHT: x_dist += 1.0f; break;
		}

	}

	void process_left_mouse_up()
	{
		//Measurment *pm = &_accounted[motion_part-1];

		//work on snap
		/*
		for(unsigned i = 0; i < _accounted.size(); i++)
		{
			if(pm != &_accounted[i])
			{
				if(_accounted[i].get_width_plus_x_fraction() > pm->_x && 
					_accounted[i].get_length_plus_y_fraction() >= pm->_y)
				{
					pm->_x = _accounted[i].get_width_plus_x_fraction();
				}
			}
		}*/

		motion_part = -1;
	}

	void process_mouse_motion(WPARAM wParam,LPARAM lParam)
	{
		WORD xPos = LOWORD(lParam); 
		WORD yPos = HIWORD(lParam);
		if(wParam == MK_RBUTTON)
		{
			float dist = get_motion_dist();

			if(xPos < last_xPos)
				x_dist -= dist;
			else if(xPos > last_xPos)
				x_dist += dist;

			if(yPos < last_yPos)
				y_dist += dist;
			else if(yPos > last_yPos)
				y_dist -= dist;

			last_xPos = xPos;
			last_yPos = yPos;
		}
		else if(wParam == MK_LBUTTON)
		{
			if(_accounted.size() > 0 && motion_part > -1)
			{
				float dist = get_motion_dist();

				if(xPos < last_xPos)
					_accounted[motion_part-1]._x -= dist;
				else if(xPos > last_xPos)
					_accounted[motion_part-1]._x += dist;

				if(yPos < last_yPos)
					_accounted[motion_part-1]._y -= dist;
				else if(yPos > last_yPos)
					_accounted[motion_part-1]._y += dist;

				last_xPos = xPos;
				last_yPos = yPos;
				update_display_dynamic();
			}
		}
		gl_select(xPos,yPos,false);
	}

	void process_padd_ckeck()
	{
		LRESULT res = SendMessage(_padd_check,BM_GETCHECK,(WPARAM)0,(LPARAM)0);

		if(res == BST_CHECKED)
		{
			EnableWindow(_padd_edit,TRUE);
			SetWindowText(_padd_edit,"2.75");
		}
		else
		{
			EnableWindow(_padd_edit,FALSE);
			SetWindowText(_padd_edit,"");
		}

		update_additions_diplay();

	}
	
	void process_ripout_ckeck()
	{
		LRESULT res = SendMessage(_ripout_check,BM_GETCHECK,(WPARAM)0,(LPARAM)0);

		if(res == BST_CHECKED)
		{
			EnableWindow(_ripout_edit,TRUE);
			SetWindowText(_ripout_edit,"1.00");
		}
		else
		{
			EnableWindow(_ripout_edit,FALSE);
			SetWindowText(_ripout_edit,"");
		}

		update_additions_diplay();

	}

	void process_wheel(WORD w)
	{
		if(w > WHEEL_DELTA)
		{
			if(z_dist > -100)
				z_dist -= 1.0f;
		}
		else
		{
			if(z_dist < -2)
				z_dist += 1.0f;
		}
	}

	void reg_win_class()
	{
		WNDCLASS wc;
		ZeroMemory(&wc, sizeof (wc));

		wc.style = CS_PARENTDC;
		wc.lpfnWndProc = WndProcFrame;
		wc.hInstance = _hInstance;
		wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor (NULL, IDC_ARROW);
		//wc.hbrBackground = (HBRUSH) COLOR_CAPTIONTEXT;
		wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
		wc.lpszClassName = FRAME_WIN_CLASS;
		RegisterClass (&wc);

		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WndProcGL;
		wc.lpszClassName = GL_WIN_CLASS;
		RegisterClass (&wc);

		wc.style = CS_CLASSDC;
		wc.lpfnWndProc = WndProcControls;
		wc.lpszClassName = CONTROLS_WIN_CLASS;
		RegisterClass (&wc);

		wc.lpfnWndProc = WndProcTopInput;
		wc.lpszClassName = TOP_INPUT_WIN_CLASS;
		RegisterClass (&wc);

		wc.lpfnWndProc = WndProcMeasurmentDisplayTotals;
		wc.lpszClassName = MEASURMENT_DIPLAY_TOTALS_WIN_CLASS;
		RegisterClass (&wc);

		wc.lpfnWndProc = WndProcMeasurmentDisplay;
		wc.lpszClassName = MEASURMENT_DIPLAY_WIN_CLASS;
		RegisterClass (&wc);
	
	}
	
	void resize_gl_scene(RECT frame_rect, RECT control_rect)		// Resize And Initialize The GL Window
	{
		GLsizei width = frame_rect.right-control_rect.right;
		GLsizei height = frame_rect.bottom;

		glViewport(0,0,width,height);						// Reset The Current Viewport

		glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glLoadIdentity();									// Reset The Projection Matrix

		// Calculate The Aspect Ratio Of The Window
		gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,500.0f);

		glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
		glLoadIdentity();									// Reset The Modelview Matrix
	}

	void retrieve_measurment()
	{
		TCHAR buff[10];
		Measurment m;
		GetWindowText(_width_feet_edit,buff,10);
		m._w_f = atoi(buff);

		GetWindowText(_width_inch_edit,buff,10);
		m._w_i = atoi(buff);

		GetWindowText(_length_feet_edit,buff,10);
		m._l_f = atoi(buff);

		GetWindowText(_length_inch_edit,buff,10);
		m._l_i = atoi(buff);

		m.normalize_width();
		m.normalize_length();
		
		_measurments.push_back(m);
	}
	
	void set_default_dists()
	{
		z_dist = -50.0f;
		x_dist = -6.0f;
		y_dist = 20.0f;
	}

	void set_up_gl_context()
	{
		int		PixelFormat;				// Holds The Results After Searching For A Match
		static	PIXELFORMATDESCRIPTOR pfd=	// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
			1,								// Version Number
			PFD_DRAW_TO_WINDOW |			// Format Must Support Window
			PFD_SUPPORT_OPENGL |			// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,				// Must Support Double Buffering
			PFD_TYPE_RGBA,					// Request An RGBA Format
			16,								// Select Our Color Depth
			0, 0, 0, 0, 0, 0,				// Color Bits Ignored
			0,								// No Alpha Buffer
			0,								// Shift Bit Ignored
			0,								// No Accumulation Buffer
			0, 0, 0, 0,						// Accumulation Bits Ignored
			16,								// 16Bit Z-Buffer (Depth Buffer)  
			0,								// No Stencil Buffer
			0,								// No Auxiliary Buffer
			PFD_MAIN_PLANE,					// Main Drawing Layer
			0,								// Reserved
			0, 0, 0							// Layer Masks Ignored
		};

		_hDC = GetDC(_gl_wnd);
		PixelFormat = ChoosePixelFormat(_hDC,&pfd);
		SetPixelFormat(_hDC,PixelFormat,&pfd);
		_hRC=wglCreateContext(_hDC);
		wglMakeCurrent(_hDC,_hRC);

		init_gl();
	}

	void size_control_window()
	{
		RECT c_frame_rect, c_control_rect;
		GetClientRect(_frame_wnd,&c_frame_rect);
		GetClientRect(_control_wnd,&c_control_rect);
		
		MoveWindow(_control_wnd,0,0,c_control_rect.right,c_frame_rect.bottom,TRUE); //good so far

		SendMessage(_measure_display_wnd,WM_SIZE,(WPARAM)NULL,(WPARAM)NULL);
	}
	
	void size_frame_window()
	{
		SendMessage(_control_wnd,WM_SIZE,(WPARAM)NULL,(WPARAM)NULL);
		SendMessage(_gl_wnd,WM_SIZE,(WPARAM)NULL,(WPARAM)NULL);
	}
	
	void size_gl_window()
	{
		RECT c_frame_rect, c_control_rect, c_gl_rect;
		
		GetClientRect(_frame_wnd,&c_frame_rect);
		GetClientRect(_control_wnd,&c_control_rect);
		GetClientRect(_gl_wnd,&c_gl_rect);

		MoveWindow(_gl_wnd,c_control_rect.right,c_gl_rect.top,c_frame_rect.right-c_control_rect.right,c_frame_rect.bottom,TRUE);
		resize_gl_scene(c_frame_rect,c_control_rect);
	}

	void size_MeasurmentDisplay_window()
	{
		RECT measure_display_rect;
		RECT c_control_rect, top_input_rect;

		GetClientRect(_control_wnd,&c_control_rect);
		GetClientRect(_topInput_wnd,&top_input_rect);
		
		int lower_control_height = c_control_rect.bottom-top_input_rect.bottom;
		MoveWindow(_measure_display_wnd,0,top_input_rect.bottom,
			c_control_rect.right,lower_control_height,TRUE);

		GetClientRect(_measure_display_wnd,&measure_display_rect);

		int bottom = measure_display_rect.bottom - 244;

		MoveWindow(_measurements_edit,10,20,120,bottom,TRUE);
		MoveWindow(_standard_edit,135, 20, 120,bottom,TRUE);
		MoveWindow(_needs_edit,260, 20, 120,bottom,TRUE);

		bottom += 25;

		MoveWindow(_measure_displayTotals_wnd,0,bottom,c_control_rect.right,220,TRUE);

	}

	void split_needs()
	{
		Measurment *pm = &_measurments.back();
		int w_f = pm->_w_f;
		bool good = false;

		while(!good)
		{
			if(w_f > 11)
			{
				_standard.push_back(Measurment(12,0,pm->_l_f,pm->_l_i));
				w_f -= 12;
			}
			else if(w_f > 0)
			{
				_needs.push_back(Measurment(w_f,pm->_w_i,pm->_l_f,pm->_l_i));
				good = true;
			}
			else if(pm->_w_i > 0)
			{
				_needs.push_back(Measurment(w_f,pm->_w_i,pm->_l_f,pm->_l_i));
				good = true;
			}
			else
			{
				good = true;
			}
		}
	}
	
	void update_additions_diplay()
	{
		bool pad ,install,rip;

		get_additions_check(&pad,&install,&rip);

		string with;

		if(pad)
		{
			with.append("Pad");
		}
		if(install)
		{
			if(pad)
				with.append(",");

			with.append("Install");
		}
		if(rip)
		{
			if(pad || install)
				with.append(",");

			with.append("Rip");
		}
		SetWindowText(_with_edit,with.c_str());
	}

	void update_calculated_needs_display()
	{
		if(_accounted.size() > 0)
		{
			Measurment m = get_calculated_needs_length();
			m._w_f = 12;
			TCHAR buff[30];

			m.get_string(buff);

			SetWindowText(_calculated_needs_edit,buff);
		}
		else
			SetWindowText(_calculated_needs_edit,"");
	}
	
	void update_cost_display()
	{
		if(_standard.size() > 0 || _needs.size() > 0 || _steps._w_f > 0)
		{
		/*	Measurment m = get_total_standard_length();
			m.add_length(get_calculated_needs_length());
			m._w_f = 12;
			TCHAR buff[30];
			
			
			float cost = 0;
			float pad_cost = 0;
			float install_cost = 0;
			float rip_cost = 0;
			float total = 0;
			int total_sqft = m.get_square_footage();

			istringstream iss;

			GetWindowText(_cost_edit,buff,30);
			iss = istringstream(buff);
			iss >> cost;

			GetWindowText(_padd_edit,buff,30);
			iss = istringstream(buff);
			iss >> pad_cost;

			GetWindowText(_install_edit,buff,30);
			iss = istringstream(buff);
			iss >> install_cost;

			GetWindowText(_ripout_edit,buff,30);
			iss = istringstream(buff);
			iss >> rip_cost;

			bool pad, install, rip;
			get_additions_check(&pad, &install, &rip);

			if(pad)
			{
				total += (float)total_sqft * pad_cost/9.0f;
			}
			if(install)
			{
				total += (float)total_sqft * install_cost/9.0f;
			}
			if(rip)
			{
				total += (float)total_sqft * rip_cost/9.0f;
			}


			total += (cost*total_sqft)/9.0f;

			total += (cost*_steps.get_square_footage())/9.0f;

			sprintf(buff,"%.2f",total);
			SetWindowText(_total_cost_edit,buff);*/
		}
		else
			SetWindowText(_total_cost_edit,"");
	}
	
	void update_display()
	{
		update_measurment_display();
		update_standard_display();
		update_needs_display();

		update_total_standards_display();

		update_display_dynamic();
	}
	
	void update_display_dynamic()
	{
		update_calculated_needs_display();
		update_total_roll_display();
		update_square_footage_display();
		update_square_yardage_display();
		update_waste_footage_display();
		update_waste_yardage_display();
		update_cost_display();
	}

	void update_measurment_display()
	{
		if(_measurments.size() > 0)
		{
			string str;
			char buff[500];
			for(unsigned i = 0; i < _measurments.size()-1; i++)
			{
				_measurments[i].get_string(buff);
				str.append(buff);
				str.append("\r\n");
			}

			_measurments.back().get_string(buff);
			str.append(buff);

			SetWindowText(_measurements_edit,str.c_str());
		}
		else
			SetWindowText(_measurements_edit,"");
	}

	void update_needs_display()
	{
		if(_needs.size() > 0)
		{
			string str;
			char buff[500];
			for(unsigned i = 0; i < _needs.size()-1; i++)
			{
				_needs[i].get_string(buff);
				str.append(buff);
				str.append("\r\n");
			}

			_needs.back().get_string(buff);
			str.append(buff);

			SetWindowText(_needs_edit,str.c_str());
		}
		else
			SetWindowText(_needs_edit,"");
	}

	void update_square_footage_display()
	{
		if(_standard.size() > 0 || _needs.size() > 0)
		{
			TCHAR buff[30];
			get_total_square_footage_string(buff);
			SetWindowText(_square_foot_edit,buff);
		}
		else
			SetWindowText(_square_foot_edit,"");
	}
	
	void update_square_yardage_display()
	{
		if(_standard.size() > 0 || _needs.size() > 0)
		{
			TCHAR buff[30];
			get_total_square_yardage_string(buff);
			SetWindowText(_square_yard_edit,buff);
		}
		else
			SetWindowText(_square_yard_edit,"");
	}

	void update_standard_display()
	{
		string str;
		char buff[500];
		if(_standard.size() > 0)
		{
			for(unsigned i = 0; i < _standard.size(); i++)
			{
				_standard[i].get_string(buff);
				str.append(buff);
				str.append("\r\n");
			}

			if(_steps._w_f > 0)
			{
				_steps.get_string(buff);
				str.append(buff);
			}

			SetWindowText(_standard_edit,str.c_str());
		}
		else if(_steps._w_f > 0)
		{
			_steps.get_string(buff);
			str.append(buff);
			SetWindowText(_standard_edit,str.c_str());
		}
		else
			SetWindowText(_standard_edit,"");
	}

	void update_steps(char *cnt)
	{
		/*istringstream iss(cnt);

		int count = 0;
		iss >> count;
		
		if(count == 0)
		{
			_steps._w_f = 0;
			_steps._l_f = 0;
			return;
		}

		int val = (int)ceil((count/3.0f)*2.0f);

		while(true)
		{
			if(val%2!=0)
				val++;
			else
				break;
		}
		
		_steps._w_f = 12;
		_steps._l_f = val;*/

	}

	void update_total_standards_display()
	{
		if(_standard.size() > 0)
		{
			Measurment m = get_total_standard_length();
			m.add_length(_steps);
			m._w_f = 12;
			TCHAR buff[30];

			m.get_string(buff);

			SetWindowText(_standard_total_edit,buff);
		}
		else
			SetWindowText(_standard_total_edit,"");
	}

	void update_total_roll_display()
	{
		if(_standard.size() > 0 || _needs.size() > 0)
		{
			TCHAR buff[30];
			get_total_roll_string(buff);
			SetWindowText(_ts_cn_edit,buff);
		}
		else
			SetWindowText(_ts_cn_edit,"");
	}
	
	void update_waste_footage_display()
	{
		if(_standard.size() > 0 || _needs.size() > 0)
		{
			TCHAR buff[30];
			sprintf(buff,"%d", get_waste_footage_int());
			SetWindowText(_square_foot_waste_edit,buff);
		}
		else
			SetWindowText(_square_foot_waste_edit,"");
	}
	
	void update_waste_yardage_display()
	{
		if(_standard.size() > 0 || _needs.size() > 0)
		{
			TCHAR buff[30];
			sprintf(buff,"%d", get_waste_yardage_int());
			SetWindowText(_square_yard_waste_edit,buff);
		}
		else
			SetWindowText(_square_yard_waste_edit,"");
	}
	
	bool validate_input()
	{
		TCHAR buff[30];
		TCHAR wf[10];
		TCHAR wi[10];
		TCHAR lf[10];
		TCHAR li[10];

		GetWindowText(_width_feet_edit,wf,10);
		GetWindowText(_width_inch_edit,wi,10);
		GetWindowText(_length_feet_edit,lf,10);
		GetWindowText(_length_inch_edit,li,10);

		if(strlen(wf) == 0)
		{
			wf[0] = '0';
			wf[1] = '\0';
		}
		if(strlen(wi) == 0)
		{
			wi[0] = '0';
			wi[1] = '\0';
		}
		if(strlen(lf) == 0)
		{
			lf[0] = '0';
			lf[1] = '\0';
		}
		if(strlen(li) == 0)
		{
			li[0] = '0';
			li[1] = '\0';
		}
		if(!isdigit(wf[0]))
		{
			sprintf(buff,"Invalid feet value: %s",wf);
			MessageBox(_topInput_wnd,buff,"Warning",MB_OK);
			SetFocus(_width_feet_edit);
			SendMessage(_width_feet_edit, EM_SETSEL, 0, -1);
			return false;
		}
		if(!isdigit(wi[0]))
		{
			sprintf(buff,"Invalid inch value: %s",wi);
			MessageBox(_topInput_wnd,buff,"Warning",MB_OK);
			SetFocus(_width_inch_edit);
			SendMessage(_width_inch_edit, EM_SETSEL, 0, -1);
			return false;
		}
		if(!isdigit(lf[0]))
		{
			sprintf(buff,"Invalid feet value: %s",lf);
			MessageBox(_topInput_wnd,buff,"Warning",MB_OK);
			SetFocus(_length_feet_edit);
			SendMessage(_length_feet_edit, EM_SETSEL, 0, -1);
			return false;
		}
		if(!isdigit(li[0]))
		{
			sprintf(buff,"Invalid inch value: %s",li);
			MessageBox(_topInput_wnd,buff,"Warning",MB_OK);
			SetFocus(_length_inch_edit);
			SendMessage(_length_inch_edit, EM_SETSEL, 0, -1);
			return false;
		}


		int intwf = atoi (wf);
		int intwi = atoi (wi);
		int intlf = atoi (lf);
		int intli = atoi (li);

		if(intwf == 0 && intwi == 0)
		{
			MessageBox(_topInput_wnd,"Invalid Width:","Warning",MB_OK);
			SetFocus(_width_feet_edit);
			SendMessage(_width_feet_edit, EM_SETSEL, 0, -1);
			return false;
		}

		if(intlf == 0 && intli == 0)
		{
			MessageBox(_topInput_wnd,"Invalid Length:","Warning",MB_OK);
			SetFocus(_length_feet_edit);
			SendMessage(_length_feet_edit, EM_SETSEL, 0, -1);
			return false;
		}

		return true;
	}

public:

	MeasureWin(HINSTANCE hInstance) : _hInstance(hInstance)
	{
		last_xPos = 0;
		last_yPos = 0;
		motion_part = -1;
		_done_with_input = false;
		set_default_dists();
		reg_win_class();
	}

	int msg_loop()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof (msg));

		_frame_wnd = CreateWindow (FRAME_WIN_CLASS, "Measure", 
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,0, 0, 900, 600,
			NULL, NULL, _hInstance, this);

		size_frame_window();

		SetFocus(_width_feet_edit);

		while(GetMessage(&msg, NULL, 0, 0) > 0)
		{
			//this is for tab key to work
			if(!IsDialogMessage(_topInput_wnd,&msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			draw_gl_scene();
		}

		kill_gl_window();

		return (int)msg.message;
	}
};

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow)
{
	MeasureWin mw (hInstance);
	return mw.msg_loop();
}

