/*!
\file
\brief The header for the SampleApp class.
*/
#ifndef _define_sample_program_
#define _define_sample_program_

#include <Application.h>

/*!
\brief The sample application.

When this application is run with the restart_daemon also running, the user will be prompted if they
wish to restart the application on the original launch. If the user clicks yes, then the application
sends a message to the restart_daemon requesting a restart. See the MessageReceived function for details.
The program then terminates. If the restart_daemon is running and has properly interpreted the request,
the sample application will be restarted almost immediately and the user will be given a different
dialog box stating that the application was successfully restarted. 
*/
class SampleApp:public BApplication
{
	public:
		bool show_prompt;//!< This flag determines if the user should be prompted to restart application.
		bool i_launched_rd;//!< This flag determines if the sample app launched the restart_daemon, if so it will shut it down too.
	
		SampleApp();
		~SampleApp();
		/*!
		\brief Examine this function for restart_daemon communication.

		
		*/
		void MessageReceived(BMessage *msg);
		bool QuitRequested();
		void ReadyToRun();
};

#endif
/*
$Id: sampleapp.h,v 1.1 2004/07/13 15:45:05 bls Exp $

$Log: sampleapp.h,v $
Revision 1.1  2004/07/13 15:45:05  bls
source files added

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
