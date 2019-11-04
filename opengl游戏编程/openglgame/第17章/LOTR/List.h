#ifndef _LIST_H
#define _LIST_H

/* 双向链表类 */
class list
{
	public:
		list* prev,*next;

	list(void)
		{prev=0; next=0;}

	~list()
	{
		if(prev!=0)
			prev->next=next;
		if(next!=0)
			next->prev=prev;
		prev=0;
		next=0;
	}


	void Insert(list *temp)
	{
		
		if(temp!=0)
		{
			temp->next=next;
			if(next!=0)
				next->prev=temp;
			next=temp;
			temp->prev=this;
		}
	}

	void Add(list *temp)
	{
		list *scan;
		scan=this;
		if(temp!=0)
		{
			while(scan->next!=0)
				{scan=scan->next;}
			scan->next=temp;
			temp->prev=scan;
			temp->next=0;
		}
	}

	void Detach()
	{
		if(prev!=0)
			prev->next=next;
		if(next!=0)
			next->prev=prev;
		next=0;
		prev=0;
	}
	
	void Remove(list* t)
	{
		//将对象从链表上移除 */
		list* temp=this;

		while(temp!=t && temp->next!=0)
		{
       		temp=temp->next;
		}

		if(temp==t)
		{
			if(prev!=0)
				prev->next=next;
			if(next!=0)
				next->prev=prev;
			next=0;
			prev=0;
		}
    }
	
	list* Get(long num)
	{
		list* temp=next;
		for(long i=0;i<num;i++)
		{
			if(temp==0)
				return 0;
      		temp=temp->next;
		}
		return temp;

	}

	long Count(void)
	{
		list* temp=next;
		long i=0;
		while(temp!=0)
		{
			i++;
      		temp=temp->next;
		}
		return i;
	}
	

};


#endif