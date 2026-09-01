#include <unistd.h>

int balance = 0;

int read_balance()
{
    usleep(250000);
    return balance;
}

void write_balance(int new_balance)
{
    usleep(250000);
    balance = new_balance;
}
