﻿/** 
* @file llfloaterexperiencepicker.cpp
* @brief Implementation of llfloaterexperiencepicker
* @author dolphin@lindenlab.com
*
* $LicenseInfo:firstyear=2014&license=viewerlgpl$
* Second Life Viewer Source Code
* Copyright (C) 2014, Linden Research, Inc.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation;
* version 2.1 of the License only.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*
* Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
* $/LicenseInfo$
*/

#include "llviewerprecompiledheaders.h"

#include "llfloaterexperiencepicker.h"


#include "lllineeditor.h"
#include "llfloaterreg.h"
#include "llscrolllistctrl.h"
#include "llviewerregion.h"
#include "llagent.h"
#include "llexperiencecache.h"
#include "llslurl.h"
#include "llavatarnamecache.h"
#include "llfloaterexperienceprofile.h"
#include "llcombobox.h"
#include "llviewercontrol.h"
#include "lldraghandle.h"
#include "llpanelexperiencepicker.h"

LLFloaterExperiencePicker* LLFloaterExperiencePicker::show( select_callback_t callback, const LLUUID& key, BOOL allow_multiple, BOOL closeOnSelect, LLView * frustumOrigin )
{
	LLFloaterExperiencePicker* floater =
		LLFloaterReg::showTypedInstance<LLFloaterExperiencePicker>("experience_search", key);
	if (!floater)
	{
		llwarns << "Cannot instantiate experience picker" << llendl;
		return NULL;
	}

	floater->mSelectionCallback = callback;
	floater->mCloseOnSelect = closeOnSelect;
	floater->mAllowMultiple = allow_multiple;

	if(frustumOrigin)
	{
		floater->mFrustumOrigin = frustumOrigin->getHandle();
	}

	return floater;
}

void LLFloaterExperiencePicker::drawFrustum()
{
	if(mFrustumOrigin.get())
	{
		LLView * frustumOrigin = mFrustumOrigin.get();
		LLRect origin_rect;
		frustumOrigin->localRectToOtherView(frustumOrigin->getLocalRect(), &origin_rect, this);
		// draw context cone connecting color picker with color swatch in parent floater
		LLRect local_rect = getLocalRect();
		if (hasFocus() && frustumOrigin->isInVisibleChain() && mContextConeOpacity > 0.001f)
		{
			gGL.getTexUnit(0)->unbind(LLTexUnit::TT_TEXTURE);
			LLGLEnable(GL_CULL_FACE);
			gGL.begin(LLRender::QUADS);
			{
				gGL.color4f(0.f, 0.f, 0.f, mContextConeInAlpha * mContextConeOpacity);
				gGL.vertex2i(origin_rect.mLeft, origin_rect.mTop);
				gGL.vertex2i(origin_rect.mRight, origin_rect.mTop);
				gGL.color4f(0.f, 0.f, 0.f, mContextConeOutAlpha * mContextConeOpacity);
				gGL.vertex2i(local_rect.mRight, local_rect.mTop);
				gGL.vertex2i(local_rect.mLeft, local_rect.mTop);

				gGL.color4f(0.f, 0.f, 0.f, mContextConeOutAlpha * mContextConeOpacity);
				gGL.vertex2i(local_rect.mLeft, local_rect.mTop);
				gGL.vertex2i(local_rect.mLeft, local_rect.mBottom);
				gGL.color4f(0.f, 0.f, 0.f, mContextConeInAlpha * mContextConeOpacity);
				gGL.vertex2i(origin_rect.mLeft, origin_rect.mBottom);
				gGL.vertex2i(origin_rect.mLeft, origin_rect.mTop);

				gGL.color4f(0.f, 0.f, 0.f, mContextConeOutAlpha * mContextConeOpacity);
				gGL.vertex2i(local_rect.mRight, local_rect.mBottom);
				gGL.vertex2i(local_rect.mRight, local_rect.mTop);
				gGL.color4f(0.f, 0.f, 0.f, mContextConeInAlpha * mContextConeOpacity);
				gGL.vertex2i(origin_rect.mRight, origin_rect.mTop);
				gGL.vertex2i(origin_rect.mRight, origin_rect.mBottom);

				gGL.color4f(0.f, 0.f, 0.f, mContextConeOutAlpha * mContextConeOpacity);
				gGL.vertex2i(local_rect.mLeft, local_rect.mBottom);
				gGL.vertex2i(local_rect.mRight, local_rect.mBottom);
				gGL.color4f(0.f, 0.f, 0.f, mContextConeInAlpha * mContextConeOpacity);
				gGL.vertex2i(origin_rect.mRight, origin_rect.mBottom);
				gGL.vertex2i(origin_rect.mLeft, origin_rect.mBottom);
			}
			gGL.end();
		}

		if (gFocusMgr.childHasMouseCapture(getDragHandle()))
		{
			mContextConeOpacity = lerp(mContextConeOpacity, gSavedSettings.getF32("PickerContextOpacity"), LLCriticalDamp::getInterpolant(mContextConeFadeTime));
		}
		else
		{
			mContextConeOpacity = lerp(mContextConeOpacity, 0.f, LLCriticalDamp::getInterpolant(mContextConeFadeTime));
		}
	}
}

void LLFloaterExperiencePicker::draw()
{
	drawFrustum();
	LLFloater::draw();
}

LLFloaterExperiencePicker::LLFloaterExperiencePicker( const LLSD& key )
	:LLFloater(key)
	,mSearchPanel(NULL)
	,mContextConeOpacity(0.f)
	,mContextConeInAlpha(0.f)
	,mContextConeOutAlpha(0.f)
	,mContextConeFadeTime(0.f)
{
	mContextConeInAlpha = gSavedSettings.getF32("ContextConeInAlpha");
	mContextConeOutAlpha = gSavedSettings.getF32("ContextConeOutAlpha");
	mContextConeFadeTime = gSavedSettings.getF32("ContextConeFadeTime");
}

LLFloaterExperiencePicker::~LLFloaterExperiencePicker()
{
	gFocusMgr.releaseFocusIfNeeded( this );
}

BOOL LLFloaterExperiencePicker::postBuild()
{
	mSearchPanel = static_cast<LLPanelExperiencePicker*>(getChild<LLUICtrl>("panel_experience_search"));
	
	mSearchPanel->mSelectionCallback = mSelectionCallback;
	mSearchPanel->mCloseOnSelect = mCloseOnSelect;
	mSearchPanel->setAllowMultiple(mAllowMultiple);
	mSearchPanel->addFilters(mFilters.begin(), mFilters.end());

	return LLFloater::postBuild();
}
