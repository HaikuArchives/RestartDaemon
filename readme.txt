Restart Daemon Version 0.10
============================
This program is a simple to use daemon or server that will restart an application upon
request. It's primary purpose is to allow an application to restart itself at some point,
for instance after performing an upgrade. It should be simple to use:

Define these two values in your application:

#define RD_RESTART_APP 'rdra'
#define RD_APP_RESTARTED 'rdar'

Alternatively, you may copy the #defines from the source/rdapp.h file.

Send a BMessage to a running instance of the "restart_daemon" (even if you have to launch
it before restarting) with the RD_RESTART_APP value as the what value and containing these
fields:

"be:signature" or "rd:app_sig": A string containing the application's signature
"rd:app_name": (Optional) The name of the application.
"rd:ref": (Optional if the signature is passed) An entry_ref to the executable.
"rd:start_message": (Optional) One or more messages your application is to receive at start-up.
"rd:restart_time": (Optional) Don't restart the application before this time. (Should be calculated based on real_time_clock().)

As long as the restart_daemon is still running, the next time your application shutsdown,
the restart_daemon will start the application again.

Although this is still listed as an alpha, it is held back from being an official 1.0 release only because I feel there are more features that could be implemented to make this a much more useful application to the average end-user. Hopefully, new features will be added by the community now that this application has been donated to BeUnited.

Please let me know what you think of it!

Raymond C. Rodgers
bls@bbnk.dhs.org
