#include "cbase.h"

#include <VAwesomium.h>
#include "inputsystem/iinputsystem.h"

using namespace vgui;
using namespace Awesomium;

VAwesomium::VAwesomium(Panel *parent, const char *panelName) : Panel(parent, panelName)
{
	WebConfig config;

	m_iTextureId = surface()->CreateNewTextureID(true);
	m_WebCore = WebCore::Initialize(config);
	m_WebView = m_WebCore->CreateWebView(GetTall(), GetWide());

	SetPaintEnabled(true);
	SetPaintBackgroundEnabled(false);
}

VAwesomium::~VAwesomium()
{
	m_WebView->Destroy();
	m_WebCore->Shutdown();

	m_WebView = NULL;
	m_WebCore = NULL;
}

void VAwesomium::Think()
{
	m_WebCore->Update();
}

void VAwesomium::Paint()
{
	BaseClass::Paint();

	BitmapSurface* surface = (BitmapSurface*)m_WebView->surface();

	if (surface)
	{
		unsigned char* buffer = new unsigned char[surface->width() * 4 * surface->height()];
		surface->CopyTo(buffer, surface->width() * 4, 4, true, false);

		vgui::surface()->DrawSetTextureRGBA(m_iTextureId, buffer, surface->width(), surface->height(), true, true);
		vgui::surface()->DrawSetTexture(m_iTextureId);
		vgui::surface()->DrawTexturedRect(0, 0, surface->width(), surface->height());

		delete buffer;
	}
}

void VAwesomium::OnCursorMoved(int x, int y)
{
	m_WebView->InjectMouseMove(x, y);
}

void VAwesomium::OnMousePressed(MouseCode code)
{
	RequestFocus();
	m_WebView->Focus();

	MouseButtonHelper(code, false);
}

void VAwesomium::OnMouseReleased(MouseCode code)
{
	MouseButtonHelper(code, true);
}

void VAwesomium::MouseButtonHelper(MouseCode code, bool isUp)
{
	Awesomium::MouseButton awesomiumButton;

	switch (code)
	{
	case MOUSE_LEFT:
		awesomiumButton = kMouseButton_Left;
		break;
	case MOUSE_RIGHT:
		awesomiumButton = kMouseButton_Right;
		break;
	case MOUSE_MIDDLE:
		awesomiumButton = kMouseButton_Middle;
		break;
	}

	isUp ? m_WebView->InjectMouseUp(awesomiumButton) : m_WebView->InjectMouseDown(awesomiumButton);
}

void VAwesomium::OnMouseWheeled(int delta)
{
	m_WebView->InjectMouseWheel(delta * WHEEL_DELTA, 0);
}

void VAwesomium::OnKeyTyped(wchar_t unichar)
{
	WebKeyboardEvent event;

	event.text[0] = unichar;
	event.type = WebKeyboardEvent::kTypeChar;
	m_WebView->InjectKeyboardEvent(event);
}

void VAwesomium::KeyboardButtonHelper(KeyCode code, bool isUp)
{
	WebKeyboardEvent event;

	event.virtual_key_code = inputsystem->ButtonCodeToVirtualKey(code);
	event.type = isUp ? WebKeyboardEvent::kTypeKeyUp : WebKeyboardEvent::kTypeKeyDown;

	m_WebView->InjectKeyboardEvent(event);
}

void VAwesomium::OnKeyCodePressed(KeyCode code)
{
	KeyboardButtonHelper(code, false);
}

void VAwesomium::OnKeyCodeReleased(KeyCode code)
{
	KeyboardButtonHelper(code, true);
}

void VAwesomium::OpenURL(const char *address)
{
	m_WebView->Resize(GetWide(), GetTall());
	m_WebView->LoadURL(WebURL(WSLit(address)));
}

void VAwesomium::PerformLayout()
{
	BaseClass::PerformLayout();

	m_WebView->Resize(GetWide(), GetTall());
}