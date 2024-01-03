#ifndef GFC_H_INCLUDED
#define GFC_H_INCLUDED

#include "SharedBase.h"

class bCRect;
class bCPoint;
class CFFGFCWnd;
#include "GFC/gfc_bitmap.h"
#include "GFC/gfc_bitmapmap.h"
#include "GFC/gfc_imagelist.h"
#include "GFC/gfc_imagelistmap.h"

#include "GFC/gfc_dataexchange.h"
#include "GFC/gfc_devicecontext.h"

#include "GFC/gfc_wnd.h"
#include "GFC/gfc_wndmap.h"

#include "GFC/gfc_scrollbar.h"
#include "GFC/gfc_animatectrl.h"
#include "GFC/gfc_animatedimage.h"

#include "GFC/gfc_tooltipctrl.h"

#include "GFC/gfc_button.h"
#include "GFC/gfc_checkbox.h"
#include "GFC/gfc_devicecontextmap.h"
#include "GFC/gfc_dialog.h"
#include "GFC/gfc_edit.h"
#include "GFC/gfc_font.h"
#include "GFC/gfc_listbox.h"
#include "GFC/gfc_listctrl.h"
#include "GFC/gfc_module.h"
#include "GFC/gfc_progressbar.h"
#include "GFC/gfc_radiobutton.h"
#include "GFC/gfc_slider.h"
#include "GFC/gfc_static.h"
#include "GFC/gfc_tabctrl.h"
#include "GFC/gfc_toolbarctrl.h"
#include "GFC/gfc_view.h"

void GE_STDCALL FF_DDX_Control( CFFGFCDataExchange *, GEInt, CFFGFCWnd & );

#endif