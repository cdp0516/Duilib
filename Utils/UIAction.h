#pragma once
#include <functional>
#include <queue>
#include <mutex>

#define TIMER_UI_ACTION		25930

namespace DuiLib {
	class UILIB_API CUIAction
	{
	public:
		void InitUIAction(HWND hwnd)
		{
			if (hwnd != nullptr)
			{
				::SetTimer(hwnd, TIMER_UI_ACTION, 10, nullptr);
			}
		}

		void AddAction(std::function<void()>f)
		{
			std::lock_guard<std::mutex> _lock(_action_lock);
			_actions.push(f);
		}

		void DoOneAction()
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

	protected:
		std::queue<std::function<void()>> _actions;
		std::mutex _action_lock;
	};

#define ADD_ACTION(wnd, action) wnd->AddAction(action)
};
