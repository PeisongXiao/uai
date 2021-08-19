# Ultimate Anti-Indulgence
The ultimate anti-indulgence program for linux

This program is intended to control the usage of computers running the linux system.

It will force-logoff restricted users during restriction times, and will randomize the password of these users so that they can't login again.

## Usage

### Configuration

In `main.cpp`, change all occurrences of `/home/peisongxiao/...` to your own desired directory and files.

In the `restrictions` file, the first line describes the number of rules, then for every line a `hour minute hour minute` set of four numbers is used to describe the rule's starting time and ending time

In the `usrs` file, the first line describes the number of rules, then for every line a `<username> <old_password>` set of two strings *(Do NOT contain spaces)* to describe the user and the password

> Because the file is necessary to use the program's functions, please note the risk of potentially leaking your passwords

Also, you would need to `touch` the `passwd` file.

Then you're all set.

### Compilation

This program needs at least C++11 to function *(for the support of `auto` and such)*, but needs no additional options.

### Deployment

The program needs *root privileges* to function.

Single deplyments can be done simply by running it in the background as *root* (e.g. nohup uai &)

Other methods include `crontab` to run it at startup with the `@reboot` option

And using `systemd` to run it as a service.

### Output

All outputs are directed to the `log` file, and is updated once per cycle.
