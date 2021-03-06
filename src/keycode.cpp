#include <QX11Info>
#include "keycode.h"
#include "keydialog.hpp"
#include <X11/XKBlib.h>

const QString ktos( int keycode )
{
    if (keycode > MAXKEY || keycode < 0) keycode = 0;

    if (keycode == 0) return "[NO KEY]";

    QString xname = XKeysymToString( XkbKeycodeToKeysym( QX11Info::display(), keycode, 0, 0 ) );

//this section of code converts standard X11 keynames into much nicer names
//which are prettier, fit the dialogs better, and are more readily understandable.
//This is really ugly and I wish I didn't have to do this... that's why there
//is a config option to define PLAIN_KEYS and drop this whole section of code,
//instead using the default names for keys.
#ifndef PLAIN_KEYS
    //the following code assumes xname is system independent and always
    //in the same exact format.

    QRegExp rx;
    rx.setPattern("^\\w$");
    //"a-z" -> "A-Z"
    if (rx.exactMatch(xname)) return xname.toUpper();

    rx.setPattern("(.*)_(.*)");
    if (rx.exactMatch(xname)) {
        QString first = rx.cap(1);
        QString second = rx.cap(2);

        rx.setPattern("^[RL]$");
        //"Control_R" -> "R Control"
        if (rx.exactMatch(second)) return second + " " + first;

        rx.setPattern("^(Lock|Enter)$");
        //"Caps_Lock" -> "Caps Lock"
        //"KP_Enter" -> "KP Enter"
        if (rx.exactMatch(second)) return first + " " + second;

        //the following assumes all number pads are laid out alike.
        if (xname == "KP_Home")		return "KP 7";
        if (xname == "KP_Up")		return "KP 8";
        if (xname == "KP_Prior")	return "KP 9";
        if (xname == "KP_Subtract")	return "KP -";
        if (xname == "KP_Left")		return "KP 4";
        if (xname == "KP_Begin")	return "KP 5";
        if (xname == "KP_Right")	return "KP 6";
        if (xname == "KP_Add")		return "KP +";
        if (xname == "KP_End")		return "KP 1";
        if (xname == "KP_Down")		return "KP 2";
        if (xname == "KP_Next")		return "KP 3";
        if (xname == "KP_Insert")	return "KP 0";
        if (xname == "KP_Delete")	return "KP .";
        if (xname == "KP_Multiply")	return "KP *";
        if (xname == "KP_Divide")	return "KP /";

        return xname;
    }

    if (xname == "minus")			return "-";
    if (xname == "equal")			return "=";
    if (xname == "bracketleft")		return "[";
    if (xname == "bracketright")	return "]";
    if (xname == "semicolon")		return ";";
    if (xname == "apostrophe")		return "'";
    if (xname == "grave")			return "`";
    if (xname == "backslash")		return "\\";
    if (xname == "comma")			return ",";
    if (xname == "period")			return ".";
    if (xname == "slash")			return "/";
    if (xname == "space")			return "Space";
    if (xname == "Prior")			return "PageUp";
    if (xname == "Next")			return "PageDown";
#endif

    //if none of that succeeded,
    return xname;
}


KeyButton::KeyButton( QString name, int val, QWidget* parent, bool m, bool nowMouse)
        :QPushButton(nowMouse?tr("Mouse %1").arg(val):ktos(val), parent) {
    mouse = m;
    mouseClicked = nowMouse;
    buttonname = name;
    value = val;
    connect( this, SIGNAL( clicked() ), SLOT( onClick() ));
}

void KeyButton::onClick() {
    //when clicked, ask for a key!
    int retValue = KeyDialog::getKey(buttonname, mouse, &mouseClicked, this->window());
    // -1 is a special value meaning that the window was simply
    // closed so we can ignore this
    if (retValue < 0) {
        return;
    } else {
        value = retValue;
    }
    //if the return value was a mouse click...
    if (mouseClicked) {
        setText(tr("Mouse %1").arg(value));
    }
    //otherwise, it was a key press!
    else {
        setText(ktos(value));
    }
}
