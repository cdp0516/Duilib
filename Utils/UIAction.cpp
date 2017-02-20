#include "../UIlib.h"

namespace DuiLib
{
	void CUIAction::InitUIAction(HWND hwnd)
	{
		if (hwnd != nullptr)
		{
			::SetTimer(hwnd, TIMER_UI_ACTION, 10, nullptr);
		}
	}

	void CUIAction::AddAction(std::function<void()> f)
	{
		std::lock_guard<std::mutex> _lock(_action_lock);
		_actions.push(f);
	}

	void CUIAction::DoOneAction()
	{
		std::function<void()> f = nullptr;
		{
			std::lock_guard<std::mutex> _lock(_action_lock);
			if (_actions.empty())
				return;

			f = _actions.front();
			_actions.pop();
		}
		if (f)
			f();
	}
};