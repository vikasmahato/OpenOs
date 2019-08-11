#ifndef _KERNEL_KB_H_
#define _KERNEL_KB_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <devices/driver.h>

class Keyboard : public Driver{
    private:
    struct KeyboardState {
        int caps_lock;
        int shift_held;
    };

    static KeyboardState state;
    public:
        Keyboard();
        void initialize();
        void reset();
        void destroy();

    protected:
        static void keyboardHandler(struct regs *r);


};

#ifdef __cplusplus
}
#endif

#endif  // _KERNEL_KB_H_