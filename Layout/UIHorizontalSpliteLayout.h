#pragma once
namespace DuiLib
{
	class UILIB_API CHorizontalSpliteLayoutUI : public CContainerUI
	{
		DECLARE_DUICONTROL(CHorizontalSpliteLayoutUI)
	public:
		CHorizontalSpliteLayoutUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		bool Add(CControlUI* pControl);
		//bool AddAt(CControlUI* pControl, int iIndex);
		//bool Remove(CControlUI* pControl);
		//void RemoveAll();
		//int GetCurSel() const;
		//virtual bool SelectItem(int iIndex);
		//virtual bool SelectItem(CControlUI* pControl);

		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void DoPaint(HDC hDC, const RECT& rcPaint);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetSpliterWidth(int w);

	protected:
		int m_spliterWidth;
	};
}