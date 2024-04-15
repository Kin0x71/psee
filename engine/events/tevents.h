#ifndef _TEVENTS_H_
#define _TEVENTS_H_

#include <engine/common/list.h>

#include <stdio.h>
//#include <engine/input/input.h>

template </*typename TSenderObject, class TEventObject,*/ typename TArgObject>
struct _event_item
{
	_event_item* next = 0;
	//TSenderObject* psender = 0;
	//TEventObject* pevent = 0;
	//int args_count{ sizeof ...(Arguments) };
	TArgObject arg;
};

template <typename TCallback, typename TArgObject/*, class TSenderObject, class TEventObject*/>
class cTEvent_System{
public:

	cList<TCallback*> subscriber_list;
	_event_item<TArgObject>* events_root = 0;
	_event_item<TArgObject>* events_last = 0;

	cTEvent_System()
	{
		events_root = events_last = new _event_item<TArgObject>();
	}

	~cTEvent_System()
	{
		subscriber_list.free();

		_event_item<TArgObject>* cur = events_root;

		while(cur)
		{
			_event_item<TArgObject>* next = cur->next;
			delete cur;
			cur = next;
		}
	}

	void Subscribe(TCallback* pCallback)
	{
		subscriber_list.push(pCallback);
	}

	void Unsubscribe(TCallback* pCallback)
	{
		int index = subscriber_list.find(pCallback);

		if(index != -1){
			subscriber_list.pop_index(index);
		}
	}
	
	void CallSubscribersEvent(TArgObject arg)
	{
		subscriber_list.reset_enumerator();
		while(TCallback* pCallback = subscriber_list.next())
		{
			//pCallback(pSander, pEvent);
			//pCallback(_KEY_KODE_::_KEY_0, 1);
			pCallback(arg);
		}
	}
/*
	void CallSubscriberEvents(TSenderObject* pSander, TEventObject* pEvent)
	{
		subscriber_list.reset_enumerator();
		while(TCallback* pCallback = subscriber_list.next())
		{
			pCallback(pSander, pEvent);
		}
	}*/

	void PushEvent(TArgObject arg)
	{
		//events_last->psender = pSender;
		//events_last->pevent = pEvent;

		events_last->arg = arg;

		events_last->next = new _event_item<TArgObject>;
		events_last = events_last->next;
	}

	/*void PushEvent(TSenderObject* pSender, TEventObject* pEvent)
	{
		events_last->psender = pSender;
		events_last->pevent = pEvent;

		events_last->next = new _event_item<TSenderObject, TEventObject>();
		events_last = events_last->next;
	}*/

	void UpdateCalls()
	{
		_event_item<TArgObject>* cur = events_root;

		while(cur->next)
		{
			CallSubscribersEvent(cur->arg);

			_event_item<TArgObject>* next = cur->next;
			delete cur;
			cur = next;
		}

		events_root = cur;
	}
};

#endif