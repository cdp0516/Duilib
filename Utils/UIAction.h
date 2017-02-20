#pragma once
#include <functional>
#include <queue>
#include <mutex>

#define TIMER_UI_ACTION		25930

namespace DuiLib {
	class UILIB_API CUIAction
	{
	public:
		void InitUIAction(HWND hwnd);
		void AddAction(std::function<void()>f);
		void DoOneAction();
		
	protected:
		std::queue<std::function<void()>> _actions;
		std::mutex _action_lock;
	};

#define ADD_ACTION(wnd, action) wnd->AddAction(action)
};
