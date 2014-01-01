VAwesomium
==========

A class for the Source SDK 2007 used for creating an [Awesomium](http://awesomium.com) panel in VGUI.

Please note: The [source-sdk-2013](https://github.com/ValveSoftware/source-sdk-2013) includes a full implementation of the Chromium Embedded Framework, supporting features such as CSS3, HTML5 and WebM video out of the box. It is recommended that you use that instead if you are using the 2013 SDK.

Pre-requisites
--------------

Awesomium linked in your Client project. [See here](http://wiki.awesomium.com/getting-started/setting-up-on-windows.html#set_up_your_project) for a tutorial on the Awesomium site.

Basic Usage
-----------

A very simple way of using this is to instanciate the class as it is in your VGUI element.

Header:

	class MyPanel : public vgui::Panel
	{
	public:
		MyPanel(vgui::Panel *parent, const char *panelName);
		[...]
	private:
		VAwesomium *m_Browser;
	};

CPP:

	MyPanel::MyPanel(vgui::Panel *parent, const char *panelName) : vgui::Panel(parent, panelName)
	{
		m_Browser = new VAwesomium(this, "Awesomium");
		m_Browser->OpenURL("https://github.com/");
	}
	
Advanced Usage
--------------

A more advanced way is to inherit from the VAwesomium class, and with that you can override methods that support the declaration and deletion of JavaScript objects etc. Example:

Header:

	#include "VAwesomium.h"

	#define EXIT_COMMAND "ExitMyVAwesomium"

	#include <vgui/VGUI.h>
	#include <vgui_controls/Panel.h>

	class MyVAwesomium : public VAwesomium
	{
		DECLARE_CLASS_SIMPLE(MyVAwesomium, VAwesomium);
	public:
		MyVAwesomium(vgui::Panel *parent, const char *panelName) : VAwesomium(parent, panelName){};

		virtual void OnDocumentReady(Awesomium::WebView* caller, const Awesomium::WebURL& url);

		virtual void OnMethodCall(Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args);
	};

CPP:

	using namespace Awesomium;

	#ifdef PostMessage
	#undef PostMessage
	#endif

	void MyVAwesomium::OnDocumentReady(WebView* caller, const WebURL& url)
	{
		JSValue result = caller->CreateGlobalJavascriptObject(WSLit("MyObject"));

		JSObject &myObject = result.ToObject();
		myObject.SetCustomMethod(WSLit("ReturnToGame"), false);

		caller->ExecuteJavascript(WSLit("AwesomiumInitialized()"), WSLit(""));
	}

	void MyVAwesomium::OnMethodCall(Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args)
	{
		if (method_name == WSLit("ReturnToGame"))
		{
			BaseClass::GetParent()->PostMessage(BaseClass::GetParent(), new KeyValues("command", "command", EXIT_COMMAND));
		}
	}
	
With the Above, MyObject.ReturnToGame() will be available to your HTML page.

It is also worth noting that there is a slight delay before the object is available - so AwesomiumInitialized() is called on the page to let the page know that the object will now be available.
