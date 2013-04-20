#include "cbase.h"

#include <VAwesomium.h>
#include "inputsystem/iinputsystem.h"

#define DEPTH 4

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
	m_BitmapSurface = NULL;
}

void VAwesomium::Think()
{
	m_WebCore->Update();
}

void VAwesomium::Paint()
{
	BaseClass::Paint();

	m_BitmapSurface = (BitmapSurface*)m_WebView->surface();

	if (m_BitmapSurface)
	{
		AllocateViewBuffer();
		DrawBrowserView();
	}
}

int VAwesomium::NearestPowerOfTwo(int v)
{
	// http://stackoverflow.com/questions/466204/rounding-off-to-nearest-power-of-2
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}

void VAwesomium::AllocateViewBuffer()
{
	unsigned char* buffer = new unsigned char[m_iNearestPowerWidth * m_iNearestPowerHeight * DEPTH];
	m_BitmapSurface->CopyTo(buffer, m_BitmapSurface->width() * DEPTH, DEPTH, true, false);

	vgui::surface()->DrawSetTextureRGBA(m_iTextureId, buffer, m_BitmapSurface->width(), m_BitmapSurface->height(), true, true);

	delete buffer;
}

void VAwesomium::DrawBrowserView()
{
	vgui::surface()->DrawSetTexture(m_iTextureId);
	vgui::surface()->DrawSetColor(255, 255, 255, 255);

	float scalerW = float(m_BitmapSurface->width()) / float(m_iNearestPowerWidth);
	float scalerT = float(m_BitmapSurface->height()) / float(m_iNearestPowerHeight);

	vgui::surface()->DrawTexturedSubRect(0, 0, m_BitmapSurface->width(), m_BitmapSurface->height(), 0.0f, 0.0f, scalerW, scalerT);
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

void VAwesomium::ResizeView()
{
	m_WebView->Resize(GetWide(), GetTall());
	if (m_BitmapSurface)
	{
		m_iNearestPowerWidth = NearestPowerOfTwo(GetWide());
		m_iNearestPowerHeight = NearestPowerOfTwo(GetTall());
	}
}

void VAwesomium::OpenURL(const char *address)
{
	m_WebView->LoadURL(WebURL(WSLit(address)));
	ResizeView();
}

void VAwesomium::PerformLayout()
{
	BaseClass::PerformLayout();
	ResizeView();
}