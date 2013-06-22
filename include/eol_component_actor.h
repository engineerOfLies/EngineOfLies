#ifndef __EOL_COMPONENT_ACTOR__
#define __EOL_COMPONENT_ACTOR__
/*
    Copyright 2013 Engineer of Lies
    This file is part of the Engine of Lies game engine library

    The Engine of Lies (EOL) is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EOL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the EOL game engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "eol_component.h"
#include "eol_types.h"
#include "eol_actor.h"

typedef struct
{
  eolFloat        scaleToFitFactor;
  eolRectFloat    bounds3D;
  eolLine         actorFile;
  eolActor      * actor;
  eolLine         action;
  eolOrientation  ori;
  eolOrientation  vectorOri;/**<set a rotation, or pulse or fade in/out*/
  eolBool         rotating;
  eolBool         fading;
}eolComponentActor;

eolComponent *eol_component_actor_create_from_config(eolKeychain *def, eolRect parentRect);

#endif
