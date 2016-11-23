#include "../UIlib.h"
#include "UIHorizontalSplitLayout.h"
#include "../Utils/log.h"

namespace DuiLib
{
	IMPLEMENT_DUICONTROL(CHorizontalSplitLayoutUI);

	CHorizontalSplitLayoutUI::CHorizontalSplitLayoutUI():
		m_splitterWidth(5),
		m_bDrag(false),
		m_dwSplitterBkColor(0xFFffffff),
		m_dragSplitter(nullptr),
		m_bFirstLayout(true),
		m_oldHeight(0)
	{

	}

	CHorizontalSplitLayoutUI::~CHorizontalSplitLayoutUI()
	{
		for (auto splitter : m_splitters)
		{
			delete splitter;
		}
		m_splitters.clear();
	}

	LPCTSTR CHorizontalSplitLayoutUI::GetClass() const
	{
		return L"HorizontalSpliteLayoutUI";
	}

	LPVOID CHorizontalSplitLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_HORIZONTALSPLITELAYOUT) == 0)
			return static_cast<CHorizontalSplitLayoutUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	bool CHorizontalSplitLayoutUI::Add(CControlUI * pControl)
	{
		bool ret = CContainerUI::Add(pControl);

		if (!m_splitters.empty())
		{
			CHorizontalSplitterUI* lastSplitter = m_splitters.back();
			if (lastSplitter)
			{
				lastSplitter->m_nextControll = pControl;
			}
		}

		CHorizontalSplitterUI* splitter = new CHorizontalSplitterUI();
		splitter->m_prevControll = pControl;
		splitter->SetBkColor(m_dwSplitterBkColor);
		m_splitters.push_back(splitter);
		return ret;
	}

	void CHorizontalSplitLayoutUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		CControlUI::SetPos(rc, bNeedInvalidate);
		rc = m_rcItem;

		// Adjust for inset
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		if (m_items.GetSize() == 0)
			return;

		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
		// 首次载入，先动态计算一次
		if (m_bFirstLayout)
		{
			m_bFirstLayout = false;
			
			int nAdjustables = 0;
			int usedHeight = 0;
			for (int it1 = 0; it1 < m_items.GetSize(); it1++)
			{
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
				if (!pControl->IsVisible())
					continue;
				if (pControl->IsFloat())
					continue;

				int cy = pControl->GetFixedHeight();
				if (cy == 0)
					nAdjustables++;
				else
					usedHeight += cy;
			}

			int dh = ((szAvailable.cy - usedHeight) - (m_items.GetSize() - 1) * m_splitterWidth) / nAdjustables;
			if (dh < 0)
				dh = 0;

			int current_top = rc.top;
			for (int it2 = 0; it2 < m_items.GetSize(); it2++)
			{
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
				if (!pControl->IsVisible())
					continue;
				if (pControl->IsFloat())
				{
					SetFloatPos(it2);
					continue;
				}

				RECT rcPadding = pControl->GetPadding();
				int ctrl_w = pControl->GetFixedWidth();
				int ctrl_h = pControl->GetFixedHeight();

				if (ctrl_w <= 0)
					ctrl_w = min(szAvailable.cx, pControl->GetMaxWidth());

				if (ctrl_h <= 0)
					ctrl_h = min(dh, pControl->GetMaxHeight());

				int left = rc.left + rcPadding.left;
				int top = current_top + rcPadding.top;
				int right = rc.left + ctrl_w - rcPadding.right;
				int bottom = top + ctrl_h - rcPadding.bottom;
				if (it2 == m_items.GetSize() - 1)	// 最后一个控件，补上误差
					bottom = rc.bottom - rcPadding.bottom;
				RECT rcCtrl = { left, top, right, bottom };
				pControl->SetPos(rcCtrl, false);
				pControl->SetFixedWidth(ctrl_w);	// 将调整后的尺寸存入
				pControl->SetFixedHeight(ctrl_h);	// drag时会修改这2个值

				RECT rcSplitter = { left, bottom, right, bottom + m_splitterWidth };
				auto splitter = m_splitters[it2];
				splitter->SetPos(rcSplitter, false);
				if (!splitter->IsStatic())
					m_oldHeight += ctrl_h;

				current_top += ctrl_h + m_splitterWidth;
			}
		}
		else
		{
			if (m_bDrag && m_dragSplitter)// 拖动splitter引起的setpos，重新定位相邻controll
			{
				CControlUI* pControll = nullptr;
				RECT rcSplitter;
				RECT rcControll;

				rcSplitter = m_dragSplitter->GetPos();

				// 设置上面的控件
				pControll = m_dragSplitter->m_prevControll;
				assert(pControll);
				rcControll = pControll->GetPos();
				rcControll.bottom = rcSplitter.top;
				rcControll.right = rcSplitter.right;
				pControll->SetPos(rcControll);
				pControll->SetFixedHeight(rcControll.bottom - rcControll.top);
				
				// 设置下面的控件
				pControll = m_dragSplitter->m_nextControll;
				rcControll = pControll->GetPos();
				rcControll.top = rcSplitter.bottom;
				rcControll.right = rcSplitter.right;
				pControll->SetPos(rcControll);
				pControll->SetFixedHeight(rcControll.bottom - rcControll.top);

				//重新计算oldheight
				m_oldHeight = 0;
				for (auto splitter : m_splitters)
				{
					if (!splitter->IsStatic())
					{
						CControlUI* pControl = splitter->m_prevControll;
						m_oldHeight += pControl->GetFixedHeight();
					}
				}
			}
			else  //窗口大小变化引起的setpos，重新计算所有控件位置
			{
				int nAdjustables = 0;
				int usedHeight = 0;
				for (auto splitter : m_splitters)
				{
					if (splitter->IsStatic())
					{
						CControlUI* pControl = splitter->m_prevControll;
						usedHeight += pControl->GetFixedHeight();
					}
				}

				int unUsedHeight = szAvailable.cy - usedHeight;
				int current_top = rc.top;
				for (int it2 = 0; it2 < m_items.GetSize(); it2++)
				{
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
					if (!pControl->IsVisible())
						continue;
					if (pControl->IsFloat())
					{
						SetFloatPos(it2);
						continue;
					}

					auto splitter = m_splitters[it2];
					bool bStatic = splitter->IsStatic();

					RECT rcPadding = pControl->GetPadding();
					int oldHeight = pControl->GetFixedHeight();
					int newHeight = oldHeight * unUsedHeight / m_oldHeight;
					int ctrl_w = min(szAvailable.cx, pControl->GetMaxWidth());
					int ctrl_h = bStatic ? pControl->GetFixedHeight() : newHeight;

					int left = rc.left + rcPadding.left;
					int top = current_top + rcPadding.top;
					int right = rc.left + ctrl_w - rcPadding.right;
					int bottom = top + ctrl_h - rcPadding.bottom;
					if (it2 == m_items.GetSize() - 1)	// 最后一个控件，补上误差
						bottom = rc.bottom - rcPadding.bottom;
					RECT rcCtrl = { left, top, right, bottom };
					pControl->SetPos(rcCtrl, false);
					
					RECT rcSplitter = { left, bottom, right, bottom + m_splitterWidth };
					splitter->SetPos(rcSplitter, false);

					current_top += ctrl_h + m_splitterWidth;
				}
			}
		}
	}

	void CHorizontalSplitLayoutUI::DoPaint(HDC hDC, const RECT & rcPaint)
	{
		RECT rcTemp = { 0 };
		if (!::IntersectRect(&rcTemp, &rcPaint, &m_rcItem)) 
			return;

		//CRenderClip clip;
		//CRenderClip::GeneratelClip(hDC, rcTemp, clip);
		//CControlUI::DoPaint(hDC, rcPaint);

		if (m_items.GetSize() <= 0)
			return;

		RECT rc = m_rcItem;
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		if (!::IntersectRect(&rcTemp, &rcPaint, &rc))
		{
			for (int it = 0; it < m_items.GetSize(); it++)
			{
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
				if (!pControl->IsVisible())
					continue;
				if (!::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()))
					continue;
				if (pControl->IsFloat())
				{
					if (!::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()))
						continue;
					pControl->DoPaint(hDC, rcPaint);
				}
			}
		}
		else
		{
			CRenderClip childClip;
			CRenderClip::GenerateClip(hDC, rcTemp, childClip);
			for (int it = 0; it < m_items.GetSize(); it++) 
			{
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
				if (!pControl->IsVisible()) 
					continue;
				if (!::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos())) 
					continue;
				if (pControl->IsFloat())
				{
					if (!::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos())) 
						continue;
					CRenderClip::UseOldClipBegin(hDC, childClip);
					pControl->DoPaint(hDC, rcPaint);
					CRenderClip::UseOldClipEnd(hDC, childClip);
				}
				else 
				{
					if (!::IntersectRect(&rcTemp, &rc, &pControl->GetPos())) 
						continue;
					pControl->DoPaint(hDC, rcPaint);
				}
				auto splitter = m_splitters[it];
				if (splitter)
					splitter->DoPaint(hDC, rcPaint);
			}
		}
	}

	void CHorizontalSplitLayoutUI::DoEvent(TEventUI & event)
	{
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) 
		{
			if (m_pParent != NULL)
				m_pParent->DoEvent(event);
			else 
				CContainerUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_SETFOCUS)
		{
			Invalidate();
		}
		if (event.Type == UIEVENT_KILLFOCUS)
		{
			Invalidate();
		}
		
		if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK)
		{
			for (auto splitter : m_splitters)
			{
				RECT rc = splitter->GetPos();
				if (PtInRect(&rc, event.ptMouse))
				{
					//LOG("mouse down: %d-%d", event.ptMouse.x, event.ptMouse.y);
					m_bDrag = true;
					m_ptDrag = event.ptMouse;
					m_dragSplitter = splitter;
					break;
				}
			}
			
			return;
		}
		if (event.Type == UIEVENT_MOUSEMOVE)
		{
			// hover只是用来改变鼠标指针
			bool hover = false;
			for (auto splitter : m_splitters)
			{
				RECT rc = splitter->GetPos();
				if (PtInRect(&rc, event.ptMouse))
				{
					hover = true;
					break;
				}
			}
			if (hover)
				::SetCursor(::LoadCursor(NULL, (IDC_SIZENS)));
			else
				::SetCursor(::LoadCursor(NULL, (IDC_ARROW)));

			// drag是在click时确定的
			if (m_bDrag && m_dragSplitter)
			{
				RECT pos = m_dragSplitter->GetPos();
				pos.top += (event.ptMouse.y - m_ptDrag.y);
				pos.bottom += (event.ptMouse.y - m_ptDrag.y);
				m_dragSplitter->SetPos(pos);
				m_ptDrag = event.ptMouse;
				//LOG("drag spliter: %d-%d", m_ptDrag.x, m_ptDrag.y /*pos.top, pos.bottom*/);
				SetPos(GetPos());
			}

			return;
		}
		if (event.Type == UIEVENT_BUTTONUP)
		{
			m_bDrag = false;
			m_dragSplitter = nullptr;
			return;
		}
		
		if (event.Type == UIEVENT_MOUSEENTER)
		{
			//if (IsEnabled()) {
			//	m_uButtonState |= UISTATE_HOT;
			//	Invalidate();
			//}
		}
		if (event.Type == UIEVENT_MOUSELEAVE)
		{
			//if (IsEnabled()) {
			//	m_uButtonState &= ~UISTATE_HOT;
			//	Invalidate();
			//}
		}
		
		CContainerUI::DoEvent(event);
	}

	void CHorizontalSplitLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName, _T("spliterwidth")) == 0)
		{
			SetSpliterWidth(_ttoi(pstrValue));
		}
		else if (_tcsicmp(pstrName, _T("spliterbkcolor")) == 0) 
		{
			while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) 
				pstrValue = ::CharNext(pstrValue);
			if (*pstrValue == _T('#')) 
				pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSpliterBkColor(clrColor);
		}
		else if (_tcsicmp(pstrName, L"staticitem") == 0)
		{
			std::set<int> items;
			SplitString2Int(pstrValue, items);
			for (int i : items)
			{
				m_splitters[i]->SetStatic(true);
			}
		}
		return 
			CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	void CHorizontalSplitLayoutUI::SetSpliterWidth(int w)
	{
		m_splitterWidth = w;
	}

	void CHorizontalSplitLayoutUI::SetSpliterBkColor(DWORD dwBackColor)
	{
		if (m_dwSplitterBkColor == dwBackColor) 
			return;

		m_dwSplitterBkColor = dwBackColor;
		Invalidate();
	}

	void CHorizontalSplitLayoutUI::SplitString2Int(LPCTSTR str, std::set<int> &result)
	{
		TCHAR buf[30] = { 0 };
		bool will_not_number = false;
		while (1)
		{
			if (*str >= '0' && *str <= '9')
			{
				if (will_not_number)
				{
					result.insert(_ttoi(buf));
					break;
				}

				_tcsncat(buf, str, 1);
				str++;
				continue;
			}
			else
			{
				if (_tcslen(buf) != 0)
				{
					if (*str == ',')
					{
						result.insert(_ttoi(buf));
						memset(buf, 0, sizeof(buf));
						will_not_number = false;
					}
					else if (*str == ' ')
					{
						will_not_number = true;
					}
					else if (*str == '\0')
					{
						result.insert(_ttoi(buf));
						break;
					}
				}
				
				str++;
			}
		}
	}
}

