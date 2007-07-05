/*!
\file
\brief The header file for the MicroRestarterApp class.

The class declared in this file is the BApplication class for an application that will automatically
restart the restart_daemon when such is requested by the daemon.
*/
#ifndef _micro_restarter_app_
#define _micro_restarter_app_

#include <Application.h>
/*!
\brief restart_daemon's restarter.

If for some reason the user decides to restart the restart_daemon, this application is the one that
will bear the responsibility. It will only run if the restart_daemon requests it to, and only until
the restart_daemon itself has finished restarting. That being said, it shares a lot of similar code
with the restart_daemon, and could be adapted by any BeOS developer to restart their application(s)
without depending on the restart_daemon. The idea, however, was to make the restart_daemon very simple
to use, so that other developers wouldn't need to implement an application similar to this one.
*/
class MicroRestarterApp:public BApplication
{
	public:
/*!
\brief The constructor.

This is the constructor for the MicroRestarterApp class. It doesn't do much.
*/
		MicroRestarterApp();
/*!
\brief The destructor.

This is the destructor for the MicroRestarterApp class. It doesn't do much either.
*/
		~MicroRestarterApp();
/*!
\brief Gives authorization to the system to terminate the application.

As any BeOS developer knows, all BApplication objects have a QuitRequested function that gives the
OS permission to kill the application. Also, as any BeOS developer knows, BeOS will not terminate the
application unless this function returns true. At least, it won't do it the "correct" way without this
function. This function is overridden here just to make sure that this application does quit when
asked to do so.
*/
		bool QuitRequested();
/*!
\brief The function where all the action happens.

This is where all of the action in this application happens. It watches the roster for applications
starting and quitting, and in particular looks for the restart_daemon to begin or end execution.
*/
		void MessageReceived(BMessage *msg);
};

#endif
/*
$Id: mrapp.h,v 1.1 2004/07/13 15:45:05 bls Exp $

$Log: mrapp.h,v $
Revision 1.1  2004/07/13 15:45:05  bls
source files added

Revision 1.3  2004/06/29 00:02:59  bls
Added more documentation, cleaned up some code a bit.

Revision 1.2  2004/06/28 05:12:25  bls
Added CVS keywords to most files, added Doxygen file as well for documentation.

*/
