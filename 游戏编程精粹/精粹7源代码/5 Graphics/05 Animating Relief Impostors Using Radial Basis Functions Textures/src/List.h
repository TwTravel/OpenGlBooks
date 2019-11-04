/*
    Imagedit - Demo for Animating Relief Impostors using Radial Basis Functions Textures
    Copyright (C) 2007 Vitor Fernando Pamplona (vitor@vitorpamplona.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LIST_H
#define LIST_H

#ifndef NULL
    #define NULL 0
#endif 

/**
 * Basic High performance list class
 * Use: List<Polygon*> p;
 */ 
template<class T>
class List
{
    private:
    
        Node<T>*  header;
        Node<T>*  win;
        int             _length;
    
    public:
        
        List();
        virtual ~List();
        
        T       	insert(const T);
        T       	append(const T);
        List<T>* 	append(List<T>*);
        T       	prepend(const T);
        T       	remove();
        void    	val(const T);
        T       	val();
        T       	next();
        T       	nextCircle();
        Node<T> *  	nextNode();
        T       	prev();
        Node<T> *  	prevNode();
        T       	first();
        Node<T> * 	firstNode();
        T       	last();
        int     	length();
        bool    	isFirst();
        bool    	isLast();
        bool    	isHead();
        
        void 		print();
        void 		clear();
        void 		deleteAll();
};

template<class T>
List<T>::List() : _length(0)
{
    header  = new Node<T>(NULL);
    win     = header;
    _length = 0;
}



template<class T>
List<T>::~List()
{
    while (length() > 0)
    {
        first();
        remove();    
    }
    delete header;
}    
        

        
template<class T>
T List<T>::insert(const T val)
{
       win->insert(new Node<T>(val));
       ++_length;

       return val;
}    
     
     
        
template<class T>
T List<T>::append(const T val)
{
    header->prev()->insert(new Node<T>(val));
    ++_length;
    
    return val;
}        
     
        
template<class T>        
List<T>* List<T>::append(List<T>* l)
{
    Node<T>* a = (Node<T>*)header->prev();
    
    a->splice(l->header);
    
    _length += l->_length;
    
    l->header->remove();
    
    l->_length  = 0;
    l->win      = header;
    
    return this;
}       
     
        
template<class T>      
T List<T>::prepend(const T val)
{
    header->insert(new Node<T>(val));
    ++_length;
    
    return val;
}       
     
     
        
template<class T>       
T List<T>::remove()
{
    if (win == header)
    {
        return NULL;
    }
    
    void* val = (void*)win->_val;
    
    win = (Node<T>*)win->prev();
    
    delete (Node<T>*)win->next()->remove();
    
    --_length;
    
    return (T)val; 
}       
     
        
template<class T>       
void List<T>::val(const T v)
{
    if (win != header)
    {
        win->_val= v;
    }
}       
     
        
template<class T>      
T List<T>::val()
{
    return win->_val;
}       
    
       
template<class T>      
T List<T>::next()
{
   	win = (Node<T>*)win->next();
    
    return win->_val;   
}       
   
template<class T>      
T List<T>::nextCircle()
{
	if (win == header->next()) {
		win = header->next(); 
	} else {
		win = (Node<T>*)win->next();
	}
	return win->_val;   
}     
        
template<class T>    
T List<T>::prev()
{
    win = (Node<T>*)win->prev();
    
    return win->_val;
}       
      
template<class T>      
Node<T>* List<T>::nextNode() {
    win = (Node<T>*)win->next();
    
    return win;   
}       
     
        
template<class T>    
Node<T>* List<T>::prevNode() {
    win = (Node<T>*)win->prev();
    
    return win;
}  
      
        
template<class T>        
T List<T>::first()
{
    win = (Node<T>*)header->next();
    
    return win->_val;
}       
        
template<class T>        
Node<T> * List<T>::firstNode()
{
    win = (Node<T>*)header->next();
    
    return win;
}               
        
template<class T>
T List<T>::last()
{
    win = (Node<T>*)header->prev();
    
    return win->_val;
}       
       
        
template<class T>
int List<T>::length()
{
    return _length;
}       
        
        
template<class T>
bool List<T>::isFirst()
{
    return ((win == header->next()) && (_length > 0));
}       
        
        
template<class T>
bool List<T>::isLast()
{
    return ((win == header->prev()) && (_length > 0));    
}       
        
        
template<class T>
bool List<T>::isHead()
{
    return (win == header);
}       
        
// Se mandar exibir int, vai exibir o ponteiro p/ variavel   
template<class T>
void List<T>::print() {
    std::cout << "List: ";
    
    Node<T>* aux = (Node<T>*)header->next();
    
    for (int i = 0; i < _length; ++i)
    {
        std::cout <<  *aux->_val << ", ";
        aux = (Node<T>*)aux->next();
    }
}        

template<class T>
void List<T>::clear() {
    Node<T>* aux = (Node<T>*)header->next();
    Node<T>* toDelete;
    for (int i = 0; i < _length; ++i) {
        toDelete = aux;
        aux = (Node<T>*)aux->next();
        
        delete toDelete;
    }
    
    header  = new Node<T>(NULL);
    win     = header;
    _length = 0;
}    
    
template<class T>
void List<T>::deleteAll() {
    Node<T>* aux = (Node<T>*)header->next();
    Node<T>* toDelete;
    for (int i = 0; i < _length; ++i) {
        toDelete = aux;
        aux = (Node<T>*)aux->next();
        
        toDelete->deleteVal();
        delete toDelete;
    }
    
    header  = new Node<T>(NULL);
    win     = header;
    _length = 0;
}        
    
        
template<class T> 
List<T>* arrayToList(T a[], int n)
{
    List<T>* s = new List<T>;
    
    for (int i = 0; i < n; ++i)
    {
        s->append(a[i]);    
    }
    
    return s;
}


template<class T>
T leastItem(List<T>& s, int(*cmp)(T, T))
{
    //int i; // Precisa disso?
    
    if (s.length() == 0)
    {
        return NULL;   
    }   
    
    T v = s.first();
    
    for (s.next(); !s.isHead(); s.next())
    {
        if (cmp(s.val(), v) < 0)
        {
            v = s.val();
        }    
    } 
    
    return v;
}


#endif
