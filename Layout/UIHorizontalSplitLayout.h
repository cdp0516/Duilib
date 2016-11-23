#pragma once
#include <vector>
#include <set>

namespace DuiLib
{
	class UILIB_API CHorizontalSplitterUI : public CControlUI
	{
	public:
		friend class CHorizontalSplitLayoutUI;

	public:
		void SetStatic(bool b) { m_static = b; }
		bool IsStatic() { return m_static; }
	protected:
		DWORD m_dwSplitterBkColor;
		CControlUI* m_prevControll;
		CControlUI* m_nextControll;
		bool m_static;	//prev controll的尺寸是静态的，onsize时不会等比例缩放
	};

	class UILIB_API CHorizontalSplitLayoutUI : public CContainerUI
	{
	public:
		DECLARE_DUICONTROL(CHorizontalSplitLayoutUI)
	public:
		CHorizontalSplitLayoutUI();
		~CHorizontalSplitLayoutUI();

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
		void DoEvent(TEventUI& event);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetSpliterWidth(int w);
		void SetSpliterBkColor(DWORD dwBackColor);

	private:
		void SplitString2Int(LPCTSTR str, std::set<int> &result);

	protected:
		int m_splitterWidth;
		DWORD m_dwSplitterBkColor;
		std::vector<CHorizontalSplitterUI*> m_splitters;
		bool m_bDrag;
		POINT m_ptDrag;
		CHorizontalSplitterUI* m_dragSplitter;
		bool m_bFirstLayout;
		int m_oldHeight; //拖拽splitter之后，”非静态“控件的总高度
	};
}