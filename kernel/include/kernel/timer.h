#ifndef _KERNEL_TIMER_H_
#define _KERNEL_TIMER_H_

#ifdef __cplusplus
extern "C"
{
#endif

// Sets up the system clock
class Timer {
    private:
        // Holds how many ticks that the system has been running for
        static int timer_ticks;
        // Sets the frequency of system timer
        void timer_phase(int hz);
    public:
        Timer();

    protected:
        // IRQ Handler for the timer. Called at every clock tick
        static void timer_handler(struct regs *r);
};

#ifdef __cplusplus
}
#endif

#endif  // _KERNEL_TIMER_H_