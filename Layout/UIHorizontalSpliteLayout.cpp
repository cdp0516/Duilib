#include "StdAfx.h"
#include "UIHorizontalSpliteLayout.h"

namespace DuiLib
{
	IMPLEMENT_DUICONTROL(CHorizontalSpliteLayoutUI);

	CHorizontalSpliteLayoutUI::CHorizontalSpliteLayoutUI():
		m_spliterWidth(5)
	{

	}

	LPCTSTR CHorizontalSpliteLayoutUI::GetClass() const
	{
		return L"HorizontalSpliteLayoutUI";
	}

	LPVOID CHorizontalSpliteLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_HORIZONTALSPLITELAYOUT) == 0)
			return static_cast<CHorizontalSpliteLayoutUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	bool CHorizontalSpliteLayoutUI::Add(CControlUI * pControl)
	{
		bool ret = CContainerUI::Add(pControl);

		return ret;
	}

	void CHorizontalSpliteLayoutUI::SetPos(RECT rc, bool bNeedInvalidate)
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

		int nAdjustables = 0;
		int existHeight = 0;
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
				existHeight += cy;
		}
	
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
		int dh = ((szAvailable.cy - existHeight) - (nAdjustables - 1) * m_spliterWidth) / nAdjustables;
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
			RECT rcCtrl = { left, top, right, bottom};
			pControl->SetPos(rcCtrl, false);

			current_top += ctrl_h + m_spliterWidth;
		}
	}

	void CHorizontalSpliteLayoutUI::DoPaint(HDC hDC, const RECT & rcPaint)
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
			}
		}
	}

	void CHorizontalSpliteLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName, _T("spliterwidth")) == 0) 
			SetSpliterWidth(_ttoi(pstrValue));
		return 
			CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	void CHorizontalSpliteLayoutUI::SetSpliterWidth(int w)
	{
		m_spliterWidth = w;
	}
}
