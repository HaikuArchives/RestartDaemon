/*!
\file
\brief This file contains the RDDBIcon class.
*/
#ifndef _restart_daemon_deskbar_icon_
#define _restart_daemon_deskbar_icon_

#include <View.h>
#include <Deskbar.h>
#include <PopUpMenu.h>
#include <MenuItem.h>

/*!
\brief Places a clickable icon in the Deskbar.

This class places a clickable icon in the user's Deskbar which will allow them to
either restart or exit the restart_daemon. In future revisions, it may be possible
to see a list of applications  that have requested to be restarted, and remove them
independently or collectively. In addition, it may also be possible, one day, to allow
the user to schedule applications to be started through this class.
*/
class RDDBIcon:public BView
{
	private:
	BDeskbar *deskbar;//!< Pointer to the Deskbar.
	BPopUpMenu *pop_up_menu;//!< The pop-up menu that appears when clicking on the icon.
	BMenuItem *restart_rd_mi;//!< The restart [restart_daemon] menu item.
	BMenuItem *quit_rd_mi;//!< The quit [restart_daemon] menu item.
	bool replicant_mode;//!< Is the view a replicant or not.
	void Init(void);//!< Initializes the variables and sets up initial pop-up menu.
	bool attached_to_window;//!< Is the view attached to a window (regardless of replicant_mode).
	
	public:
	//! The normal constructor.
	RDDBIcon(BRect frame, const char *name, uint32 resize_mode,uint32 flags);
	//! The replicant/archive constructor.
	RDDBIcon(BMessage *archive);
	//! The destructor.
	~RDDBIcon();
	//! AttachedToWindow hook. See the Be Book.
	void AttachedToWindow();
	//! DetachedFromWindow hook. Likewise, see the Be Book.
	void DetachedFromWindow();
	//! Draws the icon in the Deskbar's tray.
	void Draw(BRect update_rect);
	//! Do a little bit of work; handle restart and quit requests.
	void MessageReceived(BMessage *msg);
	//! Archive this class into a replicant... sort of.
	status_t Archive(BMessage *archive, bool deep=true) const;
	//! Create an instance of this class.
	static RDDBIcon *Instantiate(BMessage *archive);
	//! Trigger the pop-up menu.
	void MouseDown(BPoint point);
};

#endif

/*
$Id: rddbicon.h,v 1.1 2004/07/13 15:45:05 bls Exp $

$Log: rddbicon.h,v $
Revision 1.1  2004/07/13 15:45:05  bls
source files added

Revision 1.2  2004/07/03 18:29:32  bls
* Implemented the update and cancel features.
* Fixed the Deskbar icon by removing the bitmap that I was trying to draw there; now I use DrawString to create a viewable icon.
* Fixed an undiscovered problem with the micro_restarter: namely that because I wasn't doing a proper string comparison, the micro_restarter stopped restarting the restart_daemon itself when requested. The question is: Why did it work previously?
* Tweaks here and there.

Revision 1.1  2004/06/30 05:40:41  bls
I have begun the process of adding the Deskbar icon, and I also added some
more code to the restart_daemon application to flesh it out a bit more.
Strangely enough, I haven't broken anything, although the deskbar icon
is appearing as a blank white square. This appears to be caused by the
archiving process as the image is added to the Deskbar... It should be
loading the icon image from the resource file, and showing the letters
"RD" in black letters on a white background, but it doesn't seem to be
doing that... Strange. Any how, added more documentation as well.

*/
