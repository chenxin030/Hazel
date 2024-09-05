#pragma once
#include "Hazel/Core.h"

#include <string>
#include <functional>

namespace Hazel {

	// Events in Hazel are currently blocking, meaning when an event occurs it
	// immediately gets dispatched and must be dealt with right then an there.
	// For the future, abetter strategy might be to buffer events in an evnet
	// bus and process them during the "event" part of the update stage.

	enum class EventType {
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory {
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

	//宏中的 ## 运算符，它会将宏参数 type 拼接到 EventType:: 后，生成完整的 EventType 枚举值。
	//#type 是一个字符串化操作符，将宏参数 type 转换为对应的字符串字面量。
	//下面2个宏在每个Event子类都要使用，用来定义自己的
#define EVENT_CLASS_TYPE(type)	static EventType GetStaticType(){ return EventType::##type; }\
								virtual EventType GetEventType()const override {return GetStaticType();}\
								virtual const char* GetName() const override {return #type;}

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class HAZEL_API Event {
		friend class EventDispatcher;

	public:

		virtual ~Event() = default;

		//下面2个函数通过上面的EVENT_CLASS_TYPE宏在各个类中声明
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;

		//通过上面的EVENT_CLASS_CATEGORY宏在各个类中声明
		virtual int GetCategoryFlags() const = 0;

		// 每个事件自己定义，用来将自己的事件类型
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}

		bool m_Handled = false;

	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;

	public:
		EventDispatcher(Event& event)
			:m_Event(event) {}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};

	// 用来将事件类型输出到日志
	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}