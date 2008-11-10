/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is wxFaceEd
 *
 * The Initial Developer of the Original Code is
 * ITC-irst, TCC Division (http://tcc.fbk.eu) Trento / ITALY.
 * Portions created by the Initial Developer are Copyright (C) 2004 - 2008
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 * - Koray Balci (koraybalci@gmail.com)
 * ***** END LICENSE BLOCK ***** */
#include "wxNumberEditCtrl.h"

#include "PreviewPanel.h"

BEGIN_EVENT_TABLE(wxNumberEditCtrl, wxTextCtrl)
    EVT_KILL_FOCUS(wxNumberEditCtrl::OnKillFocus)
END_EVENT_TABLE()

void wxNumberEditCtrl::OnKillFocus(wxFocusEvent& event)
{
	if(!GetValue().IsNumber())
	{
		Clear();
		*this << *m_pValue;
	}
	else
	{
		GetValue().ToLong(m_pValue);
		m_pParent->OnNumberEditCtrlChanged();
	}

	event.Skip();
}