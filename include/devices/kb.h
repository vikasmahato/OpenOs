#ifndef _KERNEL_KB_H_
#define _KERNEL_KB_H_

#ifdef __cplusplus
extern "C"
{
#endif


class Keyboard {
    private:
    struct KeyboardState {
        int caps_lock;
        int shift_held;
    };

    static KeyboardState state;
    public:
        Keyboard();

    protected:
        static void keyboard_handler(struct regs *r);


};

#ifdef __cplusplus
}
#endif

#endif  // _KERNEL_KB_H_