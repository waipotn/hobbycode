/************************************************************************
* Open Physics Abstraction Layer                                        *
* http://opal.sourceforge.net                                           *
* Copyright (C) 2004-2008                                               *
* Alan Fischer (http://www.lightningtoads.com)                          *
* Andres Reinot (http://www.reinot.com)                                 *
* Tyler Streeter (http://www.tylerstreeter.net)                         *
*                                                                       *
* This library is free software; you can redistribute it and/or         *
* modify it under the terms of EITHER:                                  *
*   (1) The GNU Lesser General Public License as published by the Free  *
*       Software Foundation; either version 2.1 of the License, or (at  *
*       your option) any later version. The text of the GNU Lesser      *
*       General Public License is included with this library in the     *
*       file license-LGPL.txt.                                          *
*   (2) The BSD-style license that is included with this library in     *
*       the file license-BSD.txt.                                       *
*                                                                       *
* This library is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
* license-LGPL.txt and license-BSD.txt for more details.                *
************************************************************************/

#ifndef OPAL_VOLUME_SENSOR_DATA_H
#define OPAL_VOLUME_SENSOR_DATA_H

#include "Defines.h"
#include "SensorData.h"

namespace opal
{
	/// A data structure describing a VolumeSensor.
	class VolumeSensorData : public SensorData
	{
	public:
		VolumeSensorData()
		: SensorData()
		{
			mType = VOLUME_SENSOR;
		}

		/// Copy constructor.
		VolumeSensorData(const VolumeSensorData& data)
		{
			(*this) = data;
		}

		virtual ~VolumeSensorData()
		{
		}

		/// Makes a deep copy.
		virtual void operator=(const VolumeSensorData& data)
		{
			mType = data.mType;
			enabled = data.enabled;
			name = data.name;
			solid = data.solid;
			internal_solidIndex = data.internal_solidIndex;
			solidBlueprintRefName = data.solidBlueprintRefName;
			transform = data.transform;
		}

	protected:

	private:
	};
}

#endif
