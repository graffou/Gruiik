struct single_data
{
	uint32_t data = 0;
	void get_data(uint32_t mode, uint32_t* data_o)
	{
		*data_o = data;
	}
	void set_data(uint32_t mode, uint32_t data_i)
	{
		data = data_i;
	}
};

template<int N>
struct vector_data
{
	uint32_t data[N];
	uint32_t size = N;
	void get_data(uint32_t mode, uint32_t* data_o)
	{
		//gprintf("vec data get sz %\n",N);
		if (mode & ADDRESSED)
		{
			*data_o = data[int(mode&0xFFFFFF)];
			//gprintf("read % data %\n", to_hex(mode&0xFFFFFF), int32_t(*data_o));
		}
		else if (mode & RETURN_SIZE)
			{*data_o = size;}
		else if (mode & RETURN_POINTER)
			*data_o = data[int(mode&0xFFFFFF)];
	}
	void set_data(uint32_t mode, uint32_t data_i)
	{
		//gprintf(">");
		if (mode & ADDRESSED)
		{
			data[mode&0xFFFFFF] = data_i;
			//gprintf("write % data %\n", mode&0xFFFFFF, int32_t(data_i));

		}
		else if (mode & SET_SIZE)

			size = data_i;
	}
};

// complete signal type-----------------------------------------------------
template <class T>
struct signal_t : T, signal_base
{
	signal_link* entry = NULL; // last entry in LL. previous entry is in signal_base

	gstring name = "$";
	uint8_t pending = 0;

	// CTOR no args
	signal_t()
	{
		//gprintf("#Ucreate signal_t, % %", pconv(this), pconv(&(void)attach(signal_link*))));
		gprintf("#Ucreate signal_t, % %", pconv(this));//, pconv(&signal_t<T>::attach(signal_link*)));
	}

	// CTOR with cchar strings
	template<class T2>
	signal_t(T2 x)
	{
		name = x;
		//gprintf("#Ucreate signal_t, % %", pconv(this), pconv(&(void)attach(signal_link*))));
		gprintf("#Ucreate signal_t, % %", name, pconv(this));//, pconv(&signal_t<T>::attach(signal_link*)));
	}

	// Parse LL to send an event to every connected gruik obj
	void send_event(uint8_t data)
	{
		gprintf("#V%Y sending event %Y", name, int(data));
		signal_link* link = entry;
		//int cnt = 0;
		while (link != NULL)
		{
			//gprintf(">>");
			link->attached->signal_process(data); //
			link = link->prev_entry;
			gprintf("#Bnew link is %R", pconv(link));
			//if (cnt > 10) {gprintf("#RLOOP!!!!!"); break;} else cnt ++;
		}
		gprintf("#VLeaving sending event");
	}

	// Attach a signal link (gruik <-> signal) to this signal linked list
	void attach(signal_link* x)
	{
		gprintf("#gAttaching %M to %M", x->attached->name, name);

		// if gruik obj has no signal connected
		// or if is connected to another signal, then add link to LL
		// If already conn. to this signal, just do nothing.
		if ( (x->attached->signal == NULL) or (x->attached->signal->psignal != static_cast<signal_base*>(this) ) )// verify links is not already active !
		{
			// add link to LL
			signal_link* tmp = entry;
			entry = x;
			entry->prev_entry = tmp;
			//check is gruik obj is already linked to something (else!). Then, detach it.
			if ((x->attached->signal!= NULL) )// linked gruik obj already has a signal link
				x->attached->signal->psignal->detach(x); // detach it (single signal for now)

			entry->attached->signal = x; // update link in gruik obj
			if (entry->prev_entry)
				gprintf("   attach = % %\n", entry->attached->name, entry->prev_entry->attached->name);
			else
				gprintf("   attach = % %\n", entry->attached->name, "NULL");
		}
		else gprintf("#MAlready attached!!");

	}

	// detach signal link from this signals's LL.
	void detach(signal_link* x)
	{
		gprintf("#rDetaching %M", x->attached->name);
		int success = 0;
		// find detached object in LL of links to this signal
		if (x->attached == entry->attached) //  if directly the top object (entry)
		{
			entry = entry->prev_entry; // Prune top object
			x->attached->signal = NULL;// remove link from gruik obj
			success = 1;
			gprintf("direct detach\n");
		}
		else
		{
			gprintf("#crecursive detach obj = % (detach %)\n", entry->attached->name,x->attached->name);

			signal_link* obj = entry;//x->psignal->entry;
			while( (obj != NULL) ) //and (not completed))
			{
				if (obj->prev_entry->attached == x->attached) // cur object points to object to detach
				{
					gprintf(" %r %r %r", pconv(obj), pconv(obj->prev_entry), pconv(obj->prev_entry->prev_entry));
					if (obj->prev_entry->prev_entry)
					gprintf("#WReplacing % by % in pre_entry of link to %", obj->prev_entry->attached->name,
							obj->prev_entry->prev_entry->attached->name, entry->attached->name);
					obj->prev_entry = obj->prev_entry->prev_entry; // remove from linked list
					x->attached->signal = NULL; // remove link from gruik obj
					obj = NULL;
				}
				else
				{
					obj = obj->prev_entry;
					if (obj != NULL)
					gprintf("#cskip to %r", obj->attached->name);
				}
			}
			success = 1;
		}
		if (success) gprintf("#GSuccess!"); else
			gprintf("#RFail !");
	}

	// this one can be calles using a signal_base type
	void set_event(uint8_t data)
	{
		send_event(data);
		pending = 1;
	}

	bool is_set()
	{
		return pending;
	}

	void reset_event()
	{
		pending = 0;
	}
	// Data accessors: base class T must implement these methods
	void get_data(uint32_t mode, uint32_t* data)
	{
		 T::get_data(mode, data);
	}
	void set_data(uint32_t mode, uint32_t data)
	{
		 T::set_data(mode, data);
	}

};
