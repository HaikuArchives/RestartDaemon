/*!
\file
\brief The restart_daemon's main function.
*/
/*!
\mainpage restart_daemon

\section introduction Introduction
\par
The <i>restart_daemon</i> is a strange beast; one without a solid market at
the time of its creation. It's only real purpose in existance is to make it
easier to restart any given BeOS application with trivial effort and minimal
(if any) user intervention. There are other potential uses for this application suite
listed in the \ref goals section.
\par
Included in this distribution is a sample application, aptly named rd_sample_app.
You may find it useful to review the SampleApp class for more details on how
to use this suite.


\subsection goals Goals
\par
The primary goal of this project is to make it easy for developers to restart their applications
without having to write a lot of code, depend on shell scripts, or do anything overly complicated.
A secondary goal for this project is to allow users to have programs started at specific times similar
to the way that cron works. Ideally, this feature will have a more user friendly interface than cron,
and will be accessible through a Deskbar replicant.
\par
While it is my intention to see that the end-user goal be met, it will likely be done after the
application has been released as open source software.

\subsection requirements System Requirements
\par
At this point, the only system requirements for this application are a BeOS compatible machine running
R5 or later. In theory, it should run on any version of BeOS that has the application roster and a BRoster
class compatible with R5's. This should mean that this application should run on R5 PPC, yellowTab's Zeta,
Haiku (formerly OpenBeOS) when it becomes available, and any other OSBOS implmentations. It is possible,
that this application can even be compiled and run on older versions of BeOS, but this has not been attempted.

\subsection license License
\par
Permission is hereby granted to utilize the software, in whole or part, in binary or source forms to anyone
and everyone on the condition that credit for the original work is given to the original author: <a href="http://bbnk.dhs.org/~rrodgers/">Raymond C. Rodgers</a> bls@bbnk.dhs.org .
\par
No warranty is expressed, granted, given, or implied in association with this software, and the original
author will not be held liable for any damages or expenses that might occur as a result of use or misuse
of the software. Adapting this code for malicious or illegal intent is expressly forbidden.

\section usage Usage
\subsection enduser End User
\par
Typical end-user usage would be to include the two application binaries micro_restarter and restart_daemon in the
/boot/home/config/bin/ directory, and add "restart_daemon &" to the 
/boot/home/config/boot/UserBootscript file. After this has been done, any
application that supports the restart_daemon can then request that it be
restarted when shutdown.

\subsection developer Developer
\par
Developer usage of the restart_daemon can be significantly more involved depending
on the depth of feature implementation the developer wishes to establish. For instance,
the sample application rd_sample_app (SampleApp) is an extremely simplistic implementation,
that launches the restart_daemon if it isn't already running, demonstrates the
functionality of the system, and then quits.
\par
A more fully fleshed out application might first execute a query to see if the
restart_daemon is installed on the user's computer, allow the user to choose to
restart the application, launch the restart_daemon if it isn't already running,
execute the restart, and then shutdown the daemon if the application started it.
*/

#include <stdio.h>
#include "rdapp.h"
#include <signal.h>

void signal_interrupt(int num)
{
	be_app_messenger.SendMessage(B_QUIT_REQUESTED);
}


int main()
{
	signal(SIGINT,signal_interrupt);
	(new RestarterDaemonApp())->Run();
	delete be_app;
	return 0;	
}
/*
$Id: rdmain.cpp,v 1.1 2004/07/13 15:45:05 bls Exp $

$Log: rdmain.cpp,v $
Revision 1.1  2004/07/13 15:45:05  bls
source files added

Revision 1.6  2004/07/03 22:17:12  bls
* More documentation!
* Switched over to the newer way of adding a Deskbar replicant: using AddItem(entry_ref *addon,int32 *id).
* Made a few replies to the Deskbar replicant asynchronous.
* Switched compiler flags over to optimize in a more permanent fashion.

I have also now contacted BeUnited about donating the application and source to them for
whomsoever wishes to take up development. I am also looking into adding support for
restart_daemon to OpenTracker. This will probably be a few weeks away however. More because
of lack of time than difficulty.

Revision 1.5  2004/07/03 18:29:32  bls
* Implemented the update and cancel features.
* Fixed the Deskbar icon by removing the bitmap that I was trying to draw there; now I use DrawString to create a viewable icon.
* Fixed an undiscovered problem with the micro_restarter: namely that because I wasn't doing a proper string comparison, the micro_restarter stopped restarting the restart_daemon itself when requested. The question is: Why did it work previously?
* Tweaks here and there.

Revision 1.4  2004/06/29 20:26:43  bls
Even more documentation added.

Revision 1.3  2004/06/29 04:21:22  bls
More documenting; added more #defines for future functionality. Cleaned
up a bit more code. Still much more documenting to do.

Revision 1.2  2004/06/28 05:12:25  bls
Added CVS keywords to most files, added Doxygen file as well for documentation.

*/
