/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Input.h - Simple input interface
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __INPUT_H_INCLUDED__
#define __INPUT_H_INCLUDED__

/////////////////////////////////////////////////////////////////////////////
//
// Keyboard key codes.  These values correspond to the keyboard
// scan codes on an IBM PC keyboard.  But the actual integer values
// shouldn't really matter, as the symbolic names should be used
// everywhere.  These are listed in this file in the approximate order
// of keyboard layout.
//
/////////////////////////////////////////////////////////////////////////////

// Main keyboard keys, organized by row

const int kKeyEsc = 1;
const int kKeyF1 = 59;
const int kKeyF2 = 60;
const int kKeyF3 = 61;
const int kKeyF4 = 62;
const int kKeyF5 = 63;
const int kKeyF6 = 64;
const int kKeyF7 = 65;
const int kKeyF8 = 66;
const int kKeyF9 = 67;
const int kKeyF10 = 68;
const int kKeyF11 = 87;
const int kKeyF12 = 88;

const int kKeyReverseSingleQuote = 41;
const int kKey1 = 2;
const int kKey2 = 3;
const int kKey3 = 4;
const int kKey4 = 5;
const int kKey5 = 6;
const int kKey6 = 7;
const int kKey7 = 8;
const int kKey8 = 9;
const int kKey9 = 10;
const int kKey0 = 11;
const int kKeyMinus = 12;
const int kKeyEquals = 13;
const int kKeyBackspace = 14;

const int kKeyTab = 15;
const int kKeyQ = 16;
const int kKeyW = 17;
const int kKeyE = 18;
const int kKeyR = 19;
const int kKeyT = 20;
const int kKeyY = 21;
const int kKeyU = 22;
const int kKeyI = 23;
const int kKeyO = 24;
const int kKeyP = 25;
const int kKeyOpenBracket = 26;
const int kKeyCloseBracket = 27;
const int kKeyBackSlash = 43;

const int kKeyCapsLock = 58;
const int kKeyA = 30;
const int kKeyS = 31;
const int kKeyD = 32;
const int kKeyF = 33;
const int kKeyG = 34;
const int kKeyH = 35;
const int kKeyJ = 36;
const int kKeyK = 37;
const int kKeyL = 38;
const int kKeySemicolon = 39;
const int kKeySingleQuite = 40;
const int kKeyEnter = 28;

const int kKeyLeftShift = 42;
const int kKeyZ = 44;
const int kKeyX = 45;
const int kKeyC = 46;
const int kKeyV = 47;
const int kKeyB = 48;
const int kKeyN = 49;
const int kKeyM = 50;
const int kKeyComma = 51;
const int kKeyPeriod = 52;
const int kKeySlash = 53;
const int kKeyRightShift = 54;

// Grey keys to the right of the main keyboard

const int kKeyLeftCtrl = 29;
const int kKeyLeftAlt = 56;
const int kKeySpace = 57;
const int kKeyRightAlt = 312;
const int kKeyRightCtrl = 285;

const int kKeyScrollLock = 70;

const int kKeyInsert = 338;
const int kKeyHome = 327;
const int kKeyPageUp = 329;

const int kKeyDelete = 339;
const int kKeyEnd = 335;
const int kKeyPageDown = 337;

const int kKeyUp = 328;
const int kKeyLeft = 331;
const int kKeyDown = 336;
const int kKeyRight = 333;

// Keys on the numpad

const int kKeyNumLock = 69;
const int kKeyNumSlash = 309;
const int kKeyNumAsterisk = 342;
const int kKeyNumMinus = 74;
const int kKeyNum7 = 71;
const int kKeyNum8 = 72;
const int kKeyNum9 = 73;
const int kKeyNum4 = 75;
const int kKeyNum5 = 76;
const int kKeyNum6 = 77;
const int kKeyNum1 = 79;
const int kKeyNum2 = 80;
const int kKeyNum3 = 81;
const int kKeyNum0 = 82;
const int kKeyNumPeriod = 83;
const int kKeyNumPlus = 78;
const int kKeyNumEnter = 284;

// "Virtual" keys

// Generic Shift, Alt, Ctrl keys.  These can
// be used when we don't care to distinguish between
// the left and right versions

const int kKeyShift = 512;
const int kKeyCtrl = 513;
const int kKeyAlt = 514;

// !FIXME! What are these.....we had them in our file.

//const int kKeySTAR = 55; // ?
//const int kKeyLEFT_WINDOWS = 347;
//const int kKeyRIGHT_WINDOWS = 348;
//const int kKeyAPPLICATION = 349;
//const int kKeyEXT_NUMLOCK = 325;
//const int kKeyMETA = 340;
//const int kKeyRMETA = 341;
//const int kKeyF13 = 343;
//const int kKeyF14 = 344;
//const int kKeyF15 = 345;

/////////////////////////////////////////////////////////////////////////////
//
// Misc const and struct
//
/////////////////////////////////////////////////////////////////////////////

// Key event type.  Note that these form a bitfield

enum EKeyEventType {
	eKeyEventNone,		// No event
	eKeyEventPressed = 1,	// A key was pressed.  The data is the key code.
	eKeyEventReleased = 2,	// A key was released.  The data is the key code.
	eKeyEventChar = 4,	// A character was typed.  The data is the ASCII character code.
	eKeyEventAny = -1	// Any keyboard event.
};

// Keyboard modifier bit mask.  For the shift, ctrl, and alt keys,
// there is one bit for each of the left anf right versions, plus
// a symbolic name that includes both bits, for testing if either
// was pressed.

const int kKeyModifier_LeftShift = 1;
const int kKeyModifier_RightShift = 2;
const int kKeyModifier_Shift = 3;
const int kKeyModifier_LeftCtrl = 4;
const int kKeyModifier_RightCtrl = 8;
const int kKeyModifier_Ctl = 12;
const int kKeyModifier_LeftAlt = 16;
const int kKeyModifier_RightAlt = 32;
const int kKeyModifier_Alt = 48;

// Keyboard event structure

struct KeyEvent {

	// the type of event.

	EKeyEventType	type;

	// The data for the event

	union {
		int	keyCode;	// key code (code key pressed or released)
		char	asciiCode;	// ascii code of the character (for eKeyEventChar)
	};

	// Bitfield of keyboard modifiers at the time of the event.

	int	modifiers;
};

/////////////////////////////////////////////////////////////////////////////
//
// class Keyboard
//
// Simple interface to the keyboard state
//
/////////////////////////////////////////////////////////////////////////////

class Keyboard {
public:
	Keyboard();

// Key button state.  This information only reflects current state of the
// physical keys on the keyboard and has no relation to ASCII values.
// There is no queing or ordering of the keys.  It is primarily useful
// for asking questions like "is this key currently pressed."  Notice
// that the keys are "debouncable."  See the note at the top of this
// file regarding "debouncable" buttons.

	// Return true if a key is currently pressed

	bool	isPressed(int keyCode);

	// Simulate a keypress.  Returns the previous key state

	bool	press(int keyCode);

	// Debounce ("release") a key.  Retusns the previous key state.
	// This function is very useful for testing if a key is down,
	// while at the same time clearing the key.

	bool	debounce(int keyCode);

	// Debouce all the keys.  Useful for clearing all the input so
	// no "leftover" keys are thought to be pressed.

	void	debounceAll();

// Keyboard event queue.  This is a queue of keyboard events.  This is
// useful for accepting input that is typed by the user.  If the frame
// rate is not very high, then the simple "polling" interface above is
// inadequate for many purposes, since the user can possibly type
// multiple keystrokes on one frame, and the ordering is important.  What's
// more, it is often useful to know the state of certain key modifiers
// (such as shift, ctrl, caps lock, etc) at the time of the event.

	// Get the next key event of the given type.  If no more events,
	// then false is returned.

	bool	getNextEvent(KeyEvent *event);

	// Queue an event.

	void	queueEvent(const KeyEvent &event);

	// Clear the event queue

	void	clearEventQueue();

// Misc

	// Clear all input - debounce all keys and clear the queue

	void	clear();

	// Query for current modifier state

	int	getModifiers();
};

// The global keyboard class object.

extern Keyboard gKeyboard;

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __INPUT_H_INCLUDED__

