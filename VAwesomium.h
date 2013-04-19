#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>

#include <Awesomium/WebCore.h>
#include <Awesomium/WebString.h>
#include <Awesomium/STLHelpers.h>
#include <Awesomium/BitmapSurface.h>

namespace vgui
{
	class VAwesomium : public Panel
	{
		DECLARE_CLASS_SIMPLE(VAwesomium, Panel);
	public:
		VAwesomium(Panel *parent, const char *panelName);
		~VAwesomium();
		void OpenURL(const char *address);

	protected:
		virtual void PerformLayout();
		virtual void Paint();
		virtual void Think();
		virtual void OnMousePressed(MouseCode code);
		virtual void OnMouseReleased(MouseCode code);
		virtual void OnKeyCodePressed(KeyCode code);
		virtual void OnKeyCodeReleased(KeyCode code);
		virtual void OnKeyTyped(wchar_t unichar);
		virtual void OnCursorMoved(int x, int y);
		virtual void OnMouseWheeled(int delta);
	private:
		void		MouseButtonHelper(MouseCode code, bool isUp);
		void		KeyboardButtonHelper(KeyCode code, bool isUp);
		Awesomium::WebCore		*m_WebCore;
		Awesomium::WebView		*m_WebView;
		int			m_iTextureId;
	};
}