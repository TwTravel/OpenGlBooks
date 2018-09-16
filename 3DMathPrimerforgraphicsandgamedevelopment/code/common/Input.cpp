/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Input.cpp - Simple input interface
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#include <string.h>

#include "input.h"

// Global keyboard class object

Keyboard gKeyboard;


/////////////////////////////////////////////////////////////////////////////
//
// local stuff
//
/////////////////////////////////////////////////////////////////////////////

// Current keyboard key state

const int kNumKeys = 512;
static bool	m_eKeyDown[kNumKeys];

// Key event queue

const int kMaxKeyEvent = 128;
static int	keyEventFilled = 0;
static KeyEvent	keyEventList[kMaxKeyEvent];

/////////////////////////////////////////////////////////////////////////////
//
// class Keyboard master init stuff
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Keyboard::Keyboard
//
// Constructor - Reset internal state variables

Keyboard::Keyboard(){
	clear();
}

/////////////////////////////////////////////////////////////////////////////
//
// class Keyboard key state functions
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Keyboard::isPressed
//
// Returns true if key currently pressed

bool	Keyboard::isPressed(int keyCode){ 
	if(keyCode < 0 || keyCode >= kNumKeys) return false; //bail and fail
	return m_eKeyDown[keyCode]; //normal exit
}

//---------------------------------------------------------------------------
// Keyboard::press
//
// Simulates a keypress and returns previous key state

bool	Keyboard::press(int keyCode){ 
	if(keyCode < 0 || keyCode >= kNumKeys) return false; //bail and fail
	bool OldState = m_eKeyDown[keyCode];
	m_eKeyDown[keyCode] = true;
	return OldState;
}

//---------------------------------------------------------------------------
// Keyboard::debounce
//
// Debounce a key, returns the previous key state.

bool	Keyboard::debounce(int keyCode){
	if(keyCode < 0 || keyCode >= kNumKeys) return false; //bail and fail
	bool OldState = m_eKeyDown[keyCode];
	m_eKeyDown[keyCode] = false;
	return OldState; 
}

//---------------------------------------------------------------------------
// Keyboard::debounceAll
//
// Debounce all the keys.

void	Keyboard::debounceAll(){ 
	for (int i = 0 ; i < kNumKeys ; ++i) //for each key
		m_eKeyDown[i] = false; //set it up by default
}

/////////////////////////////////////////////////////////////////////////////
//
// class Keyboard key queue functions
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Keyboard::getNextEvent
//
// Dequeue and return the next key event of the given type.  If no more events,
// then false is returned.

bool	Keyboard::getNextEvent(KeyEvent *event) {

	// Check for empty queue

	if (keyEventFilled < 1) {

		// No more events

		return false;
	}

	// Given them back the event at the head of the queue

	if (event != NULL) {
		*event = keyEventList[0];
	}

	// Remove it.  We *could* be "faster" by using a circular
	// queue and maintaining a head and tail pointers.  But this is
	// so much simpler, and really, we'll never have that many events in
	// the queue anyway, so we we'll only be moving around a couple
	// of hundred bytes in the most extreme cases.  If our queue gets
	// that full, then execution speed is probably a problem *elsewhere*
	// because you aren't pulling the events from the queue fast enough.

	--keyEventFilled;
	memmove(&keyEventList[0], &keyEventList[1], keyEventFilled*sizeof(keyEventList[0]));

	// We returned an event

	return true;
}

//---------------------------------------------------------------------------
// Keyboard::queueEvent
//
// Queue an event.

void	Keyboard::queueEvent(const KeyEvent &event) { 

	// Make sure we have room in our queue

	if (keyEventFilled < kMaxKeyEvent) {

		// Add it to the end

		keyEventList[keyEventFilled] = event;
		++keyEventFilled;
	}
}

//---------------------------------------------------------------------------
// Keyboard::clearEventQueue
//
// Clear the event queue

void	Keyboard::clearEventQueue(){ 
	keyEventFilled = 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// class Keyboard misc functions
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Keyboard::clear
//
// Clear all input - debounce all keys and clear queue

void	Keyboard::clear(){ 
	debounceAll(); // set all keys up
	clearEventQueue(); //clear event queue
}

//---------------------------------------------------------------------------
// Keyboard::getModifiers
//
// Query for current modifier state

int	Keyboard::getModifiers() {

	// Assume no modifiers will be pressed

	int	r = 0;
	
	// Check for known modifiers

	if (isPressed(kKeyLeftShift)) {
		r |= kKeyModifier_LeftShift;
	}
	if (isPressed(kKeyRightShift)) {
		r |= kKeyModifier_RightShift;
	}
	if (isPressed(kKeyLeftAlt)) {
		r |= kKeyModifier_LeftAlt;
	}
	if (isPressed(kKeyRightAlt)) {
		r |= kKeyModifier_RightAlt;
	}
	if (isPressed(kKeyLeftCtrl)) {
		r |= kKeyModifier_LeftCtrl;
	}
	if (isPressed(kKeyRightCtrl)) {
		r |= kKeyModifier_RightCtrl;
	}

	// Return it

	return r;
}