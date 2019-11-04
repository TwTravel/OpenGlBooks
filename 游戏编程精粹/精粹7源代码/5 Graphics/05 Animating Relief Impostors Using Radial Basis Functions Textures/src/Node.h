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


#ifndef NODE_H
#define NODE_H

/**
 * The Node is each item of the List class.  
 */
template<class T> 
class Node {

    protected:
    
        Node* _next;
        Node* _prev;

    public:
		T  _val;
        
        Node() { 
          _next = this;
          _prev = this; 
        }

        Node(const T val) { 
        	_val = val;
            _next = this;
            _prev = this; 
        }
        
        virtual ~Node() { }
        
        Node* next() {
    		return _next;    
		}
		
        Node* prev(){
    		return _prev;    
		}
		
		T val() { return _val; }
        
        Node* insert(Node* b) {
		    Node* c   = _next;
		    b->_next    = c;
		    b->_prev    = this;
		    _next       = b;
		    c->_prev    = b;
		    
		    return b;
		}
        
        Node* remove() {
    		_prev->_next    = _next;
    		_next->_prev    = _prev;
		    _next           = this;
    
    		return this;
		}
        
        void deleteVal() {
        	delete _val;
        }
        
        void splice(Node* b) {
		    Node* a   = this;
		    Node* an  = a->_next;
		    Node* bn  = b->_next;
		    
		    a->_next = bn;
		    b->_next = an;
		    
		    an->_prev = b;
		    bn->_prev = a;
		}
};

#endif
