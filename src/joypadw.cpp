#include "joypadw.h"

JoyPadWidget::JoyPadWidget( JoyPad* jp, int i, QWidget* parent )
        : QWidget(parent) {
    //initialize things, build the dialog
    joypad = jp;
    index = i;
    /* This was in below, no idea what it does :( ...
     * (joypad->axes+1)/2 +(joypad->buttons+1)/2 + 2
     */
    layoutMain = new QGridLayout(this);
    layoutMain->setSpacing(5);
    layoutMain->setMargin(5);
    flashcount = 0;
    int insertCounter = 0;
    quickset = NULL;

    foreach (Axis *axis, joypad->Axes) {
        AxisWidget *aw = new AxisWidget(axis,this);
        axes.append(aw);
        connect( aw, SIGNAL( flashed( bool ) ), this, SLOT( flash( bool )));
        layoutMain->addWidget(aw, insertCounter / 2, insertCounter %2);
        insertCounter++;
    }

    foreach (Button *button, joypad->Buttons) {
        ButtonWidget *bw = new ButtonWidget(button,this);
        buttons.append(bw);
        connect( bw, SIGNAL( flashed( bool ) ), this, SLOT( flash( bool )));
        layoutMain->addWidget(bw, insertCounter/2, insertCounter%2);
        insertCounter++;
    }
    
    
    if (insertCounter % 2 == 1) {
        insertCounter ++;
    }
    insertCounter += 2;
    btnClear = new QPushButton("Clear", this);
    connect(btnClear, SIGNAL(clicked()), this, SLOT(clear()));
    layoutMain->addWidget(btnClear, insertCounter / 2, insertCounter %2);
    insertCounter++;
    btnAll = new QPushButton("Quick Set", this);
    layoutMain->addWidget(btnAll, insertCounter /2, insertCounter%2);
    connect(btnAll, SIGNAL(clicked()), this, SLOT(setAll()));
}

JoyPadWidget::~JoyPadWidget() {
    //so the joypad knows that we're done.
    joypad->releaseWidget();
}

void JoyPadWidget::flash( bool on ) {
    //true iff this entire widget was considered "flashed" before
    bool wasOn = (flashcount != 0);

    //adjust the count based on this new flash
    flashcount += (on?1:-1);

    //if we were on and should now be off, or visa versa, flash the whole widget
    if (wasOn != (flashcount != 0))
        emit flashed(index);
}

void JoyPadWidget::update() {
    foreach (AxisWidget *axis, axes) {
        axis->update();
    }
    foreach (ButtonWidget *button, buttons) {
        button->update();
    }
}

void JoyPadWidget::clear() {
    joypad->toDefault();
    update();
}

void JoyPadWidget::setAll() {
    //quickset is NULL if there is no quickset dialog, and a pointer to the
    //dialog otherwise. This is so we can forward jsevents properly.
    quickset = new QuickSet(joypad, this);
    quickset->exec();
    update();
    delete quickset;
    quickset = NULL;
}

void JoyPadWidget::jsevent( js_event msg ) {
    //notify the component this event applies to. this cannot generate anything
    //other than a flash  :)
    if (msg.type == JS_EVENT_AXIS) {
        if (msg.number < axes.count()) axes[msg.number]->jsevent(msg.value);
    }
    else {
        if (msg.number < buttons.count()) buttons[msg.number]->jsevent(msg.value);
    }
    //if we're doing quickset, it needs to know when we do something.
    if (quickset != NULL)
        quickset->jsevent(msg);
}
