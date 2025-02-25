/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Route Manager
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 **************************************************************************/

#ifndef _ROUTEMAN_H__
#define _ROUTEMAN_H__

#include <functional>

#include <wx/bitmap.h>
#include <wx/brush.h>
#include <wx/dynarray.h>
#include <wx/imaglist.h>
#include <wx/pen.h>
#include <wx/string.h>

#include "MarkIcon.h"
#include "nav_object_database.h"
#include "nmea0183.h"
#include "ocpn_types.h"
#include "observable_evtvar.h"
#include "route.h"
#include "route_point.h"
#include "select.h"

//----------------------------------------------------------------------------
//   constants
//----------------------------------------------------------------------------
#ifndef PI
#define PI 3.1415926535897931160E0 /* pi */
#endif

extern bool g_bPluginHandleAutopilotRoute;

//    List definitions for Waypoint Manager Icons

class markicon_bitmap_list_type;
class markicon_key_list_type;
class markicon_description_list_type;

WX_DEFINE_SORTED_ARRAY(MarkIcon *, SortedArrayOfMarkIcon);
WX_DEFINE_ARRAY(MarkIcon *, ArrayOfMarkIcon);

// Callbacks for RoutePropDlg
struct RoutePropDlgCtx {
  std::function<void(Route*)> SetRouteAndUpdate;
  std::function<void(Route*, RoutePoint*)> SetEnroutePoint;
  std::function<void(Route*)> Hide;
  RoutePropDlgCtx() :
      SetRouteAndUpdate([&](Route* r) {}),
      SetEnroutePoint([&](Route* r, RoutePoint* rt) {}),
      Hide([&](Route* r) {})
      { }
};

//----------------------------------------------------------------------------
//   Routeman
//----------------------------------------------------------------------------
//
class Routeman {

friend class RoutemanGui;

public:
  Routeman(struct RoutePropDlgCtx ctx,
           std::function<void()> RouteMgrDlgUpdateListCtrl);
  ~Routeman();

  bool DeleteRoute(Route *pRoute, NavObjectChanges* nav_obj_changes);
  void DeleteAllRoutes(NavObjectChanges* nav_obj_changes);


  bool IsRouteValid(Route *pRoute);

  Route *FindRouteByGUID(const wxString &guid);
  Track *FindTrackByGUID(const wxString &guid);
  Route *FindRouteContainingWaypoint(RoutePoint *pWP);
  Route *FindVisibleRouteContainingWaypoint(RoutePoint *pWP);
  wxArrayPtrVoid *GetRouteArrayContaining(RoutePoint *pWP);
  bool DoesRouteContainSharedPoints(Route *pRoute);
  void RemovePointFromRoute(RoutePoint *point, Route *route, int route_state);

  bool ActivateRoute(Route *pRouteToActivate, RoutePoint *pStartPoint = NULL);
  bool ActivateRoutePoint(Route *pA, RoutePoint *pRP);
  bool ActivateNextPoint(Route *pr, bool skipped);
  RoutePoint *FindBestActivatePoint(Route *pR, double lat, double lon,
                                    double cog, double sog);

  bool UpdateAutopilot();
  bool DeactivateRoute(bool b_arrival = false);
  bool IsAnyRouteActive(void) { return (pActiveRoute != NULL); }

  Route *GetpActiveRoute() { return pActiveRoute; }
  RoutePoint *GetpActivePoint() { return pActivePoint; }
  double GetCurrentRngToActivePoint() { return CurrentRngToActivePoint; }
  double GetCurrentBrgToActivePoint() { return CurrentBrgToActivePoint; }
  double GetCurrentRngToActiveNormalArrival() {
    return CurrentRangeToActiveNormalCrossing;
  }
  double GetCurrentXTEToActivePoint() { return CurrentXTEToActivePoint; }
  void ZeroCurrentXTEToActivePoint();
  double GetCurrentSegmentCourse() { return CurrentSegmentCourse; }
  int GetXTEDir() { return XTEDir; }

  void SetColorScheme(ColorScheme cs, double displayDPmm);
  wxPen *GetRoutePen(void) { return m_pRoutePen; }
  wxPen *GetTrackPen(void) { return m_pTrackPen; }
  wxPen *GetSelectedRoutePen(void) { return m_pSelectedRoutePen; }
  wxPen *GetActiveRoutePen(void) { return m_pActiveRoutePen; }
  wxPen *GetActiveRoutePointPen(void) { return m_pActiveRoutePointPen; }
  wxPen *GetRoutePointPen(void) { return m_pRoutePointPen; }
  wxBrush *GetRouteBrush(void) { return m_pRouteBrush; }
  wxBrush *GetSelectedRouteBrush(void) { return m_pSelectedRouteBrush; }
  wxBrush *GetActiveRouteBrush(void) { return m_pActiveRouteBrush; }
  wxBrush *GetActiveRoutePointBrush(void) { return m_pActiveRoutePointBrush; }
  wxBrush *GetRoutePointBrush(void) { return m_pRoutePointBrush; }

  wxString GetRouteReverseMessage(void);
  wxString GetRouteResequenceMessage(void);

  bool m_bDataValid;

  /**
   * Notified with message targeting all plugins. Contains a message type
   * string and a wxJSONValue shared_ptr.
   */
  EventVar json_msg;

  /** Notified with a shared_ptr<ActiveLegDat>, leg info to all plugins.  */
  EventVar json_leg_info;

private:

  Route *pActiveRoute;
  RoutePoint *pActivePoint;
  double RouteBrgToActivePoint;  // TODO all these need to be doubles
  double CurrentSegmentBeginLat;
  double CurrentSegmentBeginLon;
  double CurrentRngToActivePoint;
  double CurrentBrgToActivePoint;
  double CurrentXTEToActivePoint;
  double CourseToRouteSegment;
  double CurrentRangeToActiveNormalCrossing;
  RoutePoint *pActiveRouteSegmentBeginPoint;
  RoutePoint *pRouteActivatePoint;
  double CurrentSegmentCourse;
  int XTEDir;
  bool m_bArrival;
  wxPen *m_pRoutePen;
  wxPen *m_pTrackPen;
  wxPen *m_pSelectedRoutePen;
  wxPen *m_pActiveRoutePen;
  wxPen *m_pActiveRoutePointPen;
  wxPen *m_pRoutePointPen;
  wxBrush *m_pRouteBrush;
  wxBrush *m_pSelectedRouteBrush;
  wxBrush *m_pActiveRouteBrush;
  wxBrush *m_pActiveRoutePointBrush;
  wxBrush *m_pRoutePointBrush;

  NMEA0183 m_NMEA0183;  // For autopilot output

  double m_arrival_min;
  int m_arrival_test;
  struct RoutePropDlgCtx m_prop_dlg_ctx;
  std::function<void()> m_route_mgr_dlg_update_list_ctrl;
};

//----------------------------------------------------------------------------
//   WayPointman
//----------------------------------------------------------------------------

class WayPointman {

friend class WayPointmanGui;

public:
  WayPointman();
  ~WayPointman();
  wxBitmap *GetIconBitmap(const wxString &icon_key);
  bool GetIconPrescaled(const wxString &icon_key);
  unsigned int GetIconTexture(const wxBitmap *pmb, int &glw, int &glh);
  int GetIconIndex(const wxBitmap *pbm);
  int GetIconImageListIndex(const wxBitmap *pbm);
  int GetXIconImageListIndex(const wxBitmap *pbm);
  int GetFIconImageListIndex(const wxBitmap *pbm);
  int GetNumIcons(void) { return m_pIconArray->Count(); }
  wxString CreateGUID(RoutePoint *pRP);
  RoutePoint *GetNearbyWaypoint(double lat, double lon, double radius_meters);
  RoutePoint *GetOtherNearbyWaypoint(double lat, double lon,
                                     double radius_meters,
                                     const wxString &guid);
  bool IsReallyVisible(RoutePoint *pWP);
  bool SharedWptsExist();
  void DeleteAllWaypoints(bool b_delete_used);
  RoutePoint *FindRoutePointByGUID(const wxString &guid);
  void DestroyWaypoint(RoutePoint *pRp, bool b_update_changeset = true);
  void ClearRoutePointFonts(void);

  bool DoesIconExist(const wxString &icon_key) const;
  wxBitmap GetIconBitmapForList(int index, int height);
  wxString *GetIconDescription(int index);
  wxString *GetIconKey(int index);
  wxString GetIconDescription(wxString icon_key);

  wxImageList *Getpmarkicon_image_list(int nominal_height);

  bool AddRoutePoint(RoutePoint *prp);
  bool RemoveRoutePoint(RoutePoint *prp);
  RoutePointList *GetWaypointList(void) { return m_pWayPointList; }


private:
  wxImage CreateDimImage(wxImage &image, double factor);

  RoutePointList *m_pWayPointList;
  wxBitmap *CreateDimBitmap(wxBitmap *pBitmap, double factor);

  wxImageList *pmarkicon_image_list;  // Current wxImageList, updated on
                                      // colorscheme change
  int m_markicon_image_list_base_count;
  ArrayOfMarkIcon *m_pIconArray;

  int m_nGUID;
  double m_iconListScale;

  SortedArrayOfMarkIcon *m_pLegacyIconArray;
  SortedArrayOfMarkIcon *m_pExtendedIconArray;

  int m_bitmapSizeForList;
  int m_iconListHeight;
  ColorScheme m_cs;
};

#endif // _ROUTEMAN_H__
