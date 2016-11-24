#include "../UIlib.h"
#include "UIVerticalSplitLayout.h"

namespace DuiLib
{
	IMPLEMENT_DUICONTROL(CVerticalSplitLayoutUI);

	CVerticalSplitLayoutUI::CVerticalSplitLayoutUI() :
		m_splitterWidth(5),
		m_bDrag(false),
		m_dwSplitterBkColor(0xFFffffff),
		m_dragSplitter(nullptr),
		m_bFirstLayout(true),
		m_oldWidth(0)
	{
	}

	CVerticalSplitLayoutUI::~CVerticalSplitLayoutUI()
	{
		for (auto splitter : m_splitters)
		{
			delete splitter;
		}
		m_splitters.clear();
	}

	LPCTSTR CVerticalSplitLayoutUI::GetClass() const
	{
		return L"VerticalSpliteLayoutUI";
	}

	LPVOID CVerticalSplitLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_VERTICALSPLITELAYOUT) == 0)
			return static_cast<CVerticalSplitLayoutUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	bool CVerticalSplitLayoutUI::Add(CControlUI * pControl)
	{
		bool ret = CContainerUI::Add(pControl);

		if (!m_splitters.empty())
		{
			CVerticalSplitterUI* lastSplitter = m_splitters.back();
			if (lastSplitter)
			{
				lastSplitter->m_nextControll = pControl;
			}
		}

		CVerticalSplitterUI* splitter = new CVerticalSplitterUI();
		splitter->m_prevControll = pControl;
		splitter->SetBkColor(m_dwSplitterBkColor);
		m_splitters.push_back(splitter);
		return ret;
	}

	void CVerticalSplitLayoutUI::SetPos(RECT rc, bool bNeedInvalidate)
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
			int usedWidth = 0;
			for (int it1 = 0; it1 < m_items.GetSize(); it1++)
			{
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
				if (!pControl->IsVisible())
					continue;
				if (pControl->IsFloat())
					continue;

				int cx = pControl->GetFixedWidth();
				if (cx == 0)
					nAdjustables++;
				else
					usedWidth += cx;
			}

			int dw = ((szAvailable.cx - usedWidth) - (m_items.GetSize() - 1) * m_splitterWidth) / nAdjustables;
			if (dw < 0)
				dw = 0;

			int current_left = rc.left;
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
					ctrl_w = min(dw, pControl->GetMaxWidth()); 

				if (ctrl_h <= 0)
					ctrl_h = min(szAvailable.cy, pControl->GetMaxHeight());


				int left = current_left + rcPadding.left;
				int top = rc.top + rcPadding.top;
				int right = left + ctrl_w - rcPadding.right;
				int bottom = top + ctrl_h - rcPadding.bottom;
				if (it2 == m_items.GetSize() - 1)	// 最后一个控件，补上误差
					right = rc.right - rcPadding.right;
				RECT rcCtrl = { left, top, right, bottom };
				pControl->SetPos(rcCtrl, false);
				pControl->SetFixedWidth(ctrl_w);	// 将调整后的尺寸存入
				pControl->SetFixedHeight(ctrl_h);	// drag时会修改这2个值

				RECT rcSplitter = { right, top, right + m_splitterWidth, bottom };
				auto splitter = m_splitters[it2];
				splitter->SetPos(rcSplitter, false);
				if (!splitter->IsStatic())
					m_oldWidth += ctrl_w;

				current_left += ctrl_w + m_splitterWidth;
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
				rcControll.right = rcSplitter.left;
				pControll->SetPos(rcControll);
				pControll->SetFixedWidth(rcControll.right - rcControll.left);

				// 设置下面的控件
				pControll = m_dragSplitter->m_nextControll;
				rcControll = pControll->GetPos();
				rcControll.left = rcSplitter.right;
				pControll->SetPos(rcControll);
				pControll->SetFixedWidth(rcControll.right - rcControll.left);

				//重新计算oldheight
				m_oldWidth = 0;
				for (auto splitter : m_splitters)
				{
					if (!splitter->IsStatic())
					{
						CControlUI* pControl = splitter->m_prevControll;
						m_oldWidth += pControl->GetFixedWidth();
					}
				}
			}
			else  //窗口大小变化引起的setpos，重新计算所有控件位置
			{
				int nAdjustables = 0;
				int usedWidth = 0;
				for (auto splitter : m_splitters)
				{
					if (splitter->IsStatic())
					{
						CControlUI* pControl = splitter->m_prevControll;
						usedWidth += pControl->GetFixedWidth();
					}
				}

				int unUsedWidth = szAvailable.cx - usedWidth;
				int current_left = rc.left;
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
					int oldWidth = pControl->GetFixedWidth();
					int newWidth = oldWidth * unUsedWidth / m_oldWidth;
					int ctrl_w = bStatic ? pControl->GetFixedWidth() : newWidth;  
					int ctrl_h = min(szAvailable.cy, pControl->GetMaxHeight());

					int left = current_left + rcPadding.left; 
					int top = rc.top + rcPadding.top;
					int right = left + ctrl_w - rcPadding.right;
					int bottom = top + ctrl_h - rcPadding.bottom;
					if (it2 == m_items.GetSize() - 1)	// 最后一个控件，补上误差
						right = rc.right - rcPadding.right;
					RECT rcCtrl = { left, top, right, bottom };
					pControl->SetPos(rcCtrl, false);

					RECT rcSplitter = { right, top, right + m_splitterWidth, bottom };
					splitter->SetPos(rcSplitter, false);

					current_left += ctrl_w + m_splitterWidth;
				}
			}
		}
	}

	void CVerticalSplitLayoutUI::DoPaint(HDC hDC, const RECT & rcPaint)
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

	void CVerticalSplitLayoutUI::DoEvent(TEventUI & event)
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
				::SetCursor(::LoadCursor(NULL, (IDC_SIZEWE)));
			else
				::SetCursor(::LoadCursor(NULL, (IDC_ARROW)));

			// drag是在click时确定的
			if (m_bDrag && m_dragSplitter)
			{
				RECT pos = m_dragSplitter->GetPos();
				pos.left += (event.ptMouse.x - m_ptDrag.x);
				pos.right += (event.ptMouse.x - m_ptDrag.x);
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

	void CVerticalSplitLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
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

	void CVerticalSplitLayoutUI::SetSpliterWidth(int w)
	{
		m_splitterWidth = w;
	}

	void CVerticalSplitLayoutUI::SetSpliterBkColor(DWORD dwBackColor)
	{
		if (m_dwSplitterBkColor == dwBackColor)
			return;

		m_dwSplitterBkColor = dwBackColor;
		Invalidate();
	}

	void CVerticalSplitLayoutUI::SplitString2Int(LPCTSTR str, std::set<int> &result)
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