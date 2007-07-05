/*!
\file
\brief Header file for the RestarterDaemonApp class.

This file contains the declaration for the RestarterDaemonApp class, which is the
primary class of the restarter daemon application.
*/
#ifndef _restarter_daemon_app_
#define _restarter_daemon_app_

#include <Application.h>
#include <List.h>
#include <String.h>
#include <Entry.h>
#include <Message.h>
#include <MessageRunner.h>

/*!
\brief Defines the value to be recognized by the daemon to restart a requested application.
*/
#define RD_RESTART_APP 'rdra'

/*!
\brief A notification value that indicates to the restarted app that it was reloaded.
*/
#define RD_APP_RESTARTED 'rdar'

/*!
\brief A successful restart request has been noted by the restart_daemon.
*/
#define RD_RESTARTD_OK 'rdok'

/*!
\brief An error has occurred in the restart request. Application will not be restarted.
*/
#define RD_RESTARTD_ERROR 'rder'

/*!
\brief Update restart information request.
*/
#define RD_UPDATE_APP 'rdup'

/*!
\brief Cancel restart request.
*/
#define RD_CANCEL_APP 'rdca'
/*!
\brief Restart the restart_daemon.

This value is sent from the Deskbar replicant (RDDBIcon) to the restart_daemon
application to request that it restart. This action will trigger the daemon
to launch the micro_restarter (MicroRestarterApp), shutdown the restart_daemon,
and subsequently be restarted.

\note
This will clear any and all restart requests currently in the system; the
restart_daemon does not currently store restart requests. (Version 0.10alpha)

*/
#define RD_RESTART_RD 'rsrd'
/*!
\brief Shutdown the restart_daemon.

This value is sent from the Deskbar replicant (RDDBIcon) to the restart_daemon
application requesting it to shutdown. This action will shutdown the restart_daemon
almost immediately, and all application restart requests in queue will be lost.
*/
#define RD_SHUTDOWN_RD 'exit'
/*!
\brief The workhorse of the restarter_daemon.

This class does all the important work of the daemon. Simply put, this class monitors the roster
looking for applications that have started or shutdown, and compares them to a list that it has
built (based on requests). If an application shutting down matches the list, it is restarted either
immediately or at a scheduled time.
*/
class RestarterDaemonApp:public BApplication
{
	private:
	
		/*!
		\brief Information structure.
		
		This structure keeps track of all the necessary information that will enable
		an application to be restarted.
		*/
		struct app_struct
		{
			entry_ref ref;//!< An entry_ref; points to the application's actual location on disk.
			bool ref_set;//!< An entry_ref has been passed in, so use it if possible.
			BString app_sig;//!< Application signature (mime type) of the application; may not necessarily be the launching instance.
			BString app_name;//!< Application's name; optional.
			BList *messages;//!< A BList of BMessages that will be sent at application start-up.
			time_t restart_time;//!< Restart at or after this time.
			bool app_has_quit;//!< Application has quit. This will probably go away as it's not necessary.
			/*!
				\brief The constructor for the app_struct structure.
				
				Yes. Structures can have constructors. This one sets the
				default values for the structure's contents.
			*/
			app_struct()
			{
				messages=new BList();
				restart_time=0;
				app_has_quit=false;
			}
			/*!
				\brief The destructor for the app_struct structure.
				
				Yep, they can have destructors too!
			*/
			~app_struct()
			{
				if (messages!=NULL)
				{
					BMessage *msg;
					while(!messages->IsEmpty())
					{
						msg=(BMessage*)messages->RemoveItem(0L);
						delete msg;	
					}
					delete messages;
				}
				
			}
			
		};

		BList *apps;//!< BList of app_struct records; one for each application.
		bool im_restarting;//!< Is the restart_daemon restarting?
		BMessageRunner *update_runner;//!< BMessageRunner object allows timed and/or scheduled restarts. 
		int32 deskbar_icon_id;
	
	public:
		/*!
			\brief The restart_daemon's constructor.
			
			This is the constructor of the restart_daemon's main class.
			It starts watching the roster for any signs of applications
			starting or quitting, and also creates the list of applications
			that it is monitoring. Initially, obviously, this list is empty.
			
			\param APP1SIG This is a #define variable passed in by the makefile.
			By default, the value is application/x-vnd.bls-restart_daemon
		*/
		RestarterDaemonApp();
		/*!
			\brief The restart_daemon's destructor.
			
			The destructor cleans up after the app when it is shutting down.
		*/
		~RestarterDaemonApp();
		/*!
			\brief The MessageReceived function handles most of the functionality.
			
			Ultimately, most of the work of this entire suite happens here in the
			MessageReceived function. This function processes command BMessages received
			from applications wishing to be restarted after they quit, as well as
			messages from the roster stating which applications are running. In addition,
			it also receives <i>TICK</i> BMessages which will allow applications to be
			launched at a scheduled time.

			@param msg A pointer to a BMessage object.
			
			\note			
			It appears to be a bug in Be's BeOS R5.0.x that messages sent as part of the 
			BRoster Launch() function don't actually get sent... It appears that only B_REFS_RECEIVED
			messages are actually sent, and that all others get lost to the nether regions of
			our machines...
			
		*/
		void MessageReceived(BMessage *msg);
		/*!
			\brief RestarterDaemonApp is ready to run!
		*/
		void ReadyToRun();
		bool QuitRequested();
};

#endif

/*
$Id: rdapp.h,v 1.1 2004/07/13 15:45:05 bls Exp $

$Log: rdapp.h,v $
Revision 1.1  2004/07/13 15:45:05  bls
source files added

Revision 1.7  2004/07/03 22:17:12  bls
* More documentation!
* Switched over to the newer way of adding a Deskbar replicant: using AddItem(entry_ref *addon,int32 *id).
* Made a few replies to the Deskbar replicant asynchronous.
* Switched compiler flags over to optimize in a more permanent fashion.

I have also now contacted BeUnited about donating the application and source to them for
whomsoever wishes to take up development. I am also looking into adding support for
restart_daemon to OpenTracker. This will probably be a few weeks away however. More because
of lack of time than difficulty.

Revision 1.6  2004/07/03 18:29:32  bls
* Implemented the update and cancel features.
* Fixed the Deskbar icon by removing the bitmap that I was trying to draw there; now I use DrawString to create a viewable icon.
* Fixed an undiscovered problem with the micro_restarter: namely that because I wasn't doing a proper string comparison, the micro_restarter stopped restarting the restart_daemon itself when requested. The question is: Why did it work previously?
* Tweaks here and there.

Revision 1.5  2004/06/30 05:40:41  bls
I have begun the process of adding the Deskbar icon, and I also added some
more code to the restart_daemon application to flesh it out a bit more.
Strangely enough, I haven't broken anything, although the deskbar icon
is appearing as a blank white square. This appears to be caused by the
archiving process as the image is added to the Deskbar... It should be
loading the icon image from the resource file, and showing the letters
"RD" in black letters on a white background, but it doesn't seem to be
doing that... Strange. Any how, added more documentation as well.

Revision 1.4  2004/06/29 20:26:43  bls
Even more documentation added.

Revision 1.3  2004/06/29 04:21:22  bls
More documenting; added more #defines for future functionality. Cleaned
up a bit more code. Still much more documenting to do.

Revision 1.2  2004/06/28 05:12:25  bls
Added CVS keywords to most files, added Doxygen file as well for documentation.

*/
