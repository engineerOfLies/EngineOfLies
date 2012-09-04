#!BPY
# -*- coding: utf-8 -*-

"""
Name: 'eol Armature (.eolarm)...'
Blender: 243
Group: 'Export'
Tooltip: 'Save an EOL Armature File'
"""

__author__ = "Engineer of Lies"
__url__ = ['www.blender.org', 'http://engineeroflies.github.com/EngineOfLies/']
__version__ = "0.1"

__bpydoc__ = """\
This script is an exporter to EOL Armature file format.
Based on the OBJ exporter.
Usage:

Select the armature object you wish to export and run this script from
"File->Export" menu.  Selecting the default options from the popup box will be
good in most cases.
"""


# --------------------------------------------------------------------------
# EOL ARMATURE export for Engine of Lies
# --------------------------------------------------------------------------
# ***** BEGIN GPL LICENSE BLOCK *****
#
#    Copyright 2012 Engineer of Lies
#    This file is part of the Engine of Lies game engine library
#
#    The Engine of Lies (EOL) is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    EOL is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with the EOL game engine.  If not, see <http://www.gnu.org/licenses/>.
#
# ***** END GPL LICENCE BLOCK *****
# --------------------------------------------------------------------------


import Blender
from Blender import Mesh, Scene, Window, sys, Image, Draw
import BPyMesh
import BPyArmature
import BPyObject
import BPySys
import BPyMessages

# Returns a tuple - path,extension.
# 'hello.eolarm' >  ('hello', '.eolarm')
def splitExt(path):
	dotidx = path.rfind('.')
	if dotidx == -1:
		return path, ''
	else:
		return path[:dotidx], path[dotidx:] 

def fixName(name):
	if name == None:
		return 'None'
	else:
		return name.replace(' ', '_')

def copy_file(source, dest):
	file = open(source, 'rb')
	data = file.read()
	file.close()
	
	file = open(dest, 'wb')
	file.write(data)
	file.close()

#Engineer of Lies new function
def write_armature(file, ob):
	if ob.getType() != "Armature":
		return
	file.write('armature %s\n' % ob.name)
	arm = ob.getData()
	armobmat = ob.getMatrix()
	for i in armobmat:
		file.write('armmat: %.6f %.6f %.6f %.6f\n' % tuple(i))
	bones = arm.bones.values()
	file.write('bonecount %i\n' % len(bones))
	for bone in bones:
		file.write('bone: %s\n' % bone.name)
		if bone.hasParent():
			file.write('parent: %s\n' % bone.parent.name)
		if bone.hasChildren():
			file.write('childcount %i\n' % len(bone.children))
			for child in bone.children:
				file.write('child: %s\n' % child.name)
		head = bone.head
		tail = bone.tail
		headrad = bone.headRadius
		tailrad = bone.tailRadius
		for i in bone.matrix["ARMATURESPACE"]:
			file.write('m: %.6f %.6f %.6f %.6f\n' % tuple(i))
	file.write('\n\n')

def write_poses(file,ob):
	if ob.getType() != "Armature":
		return
	scn = Scene.GetCurrent()
	context = scn.getRenderingContext()
	scene_frames = xrange(context.startFrame(), context.endFrame()+1)
	file.write('framecount %i\n' % len(scene_frames))
	for frame in scene_frames:
		Blender.Set('curframe', frame)
		file.write('frame: %i\n' % frame)
		pose = ob.getPose()
		for b in pose.bones.values():
			file.write('posebone: %s\n' % b.name)
			for i in b.localMatrix:
				file.write('lm: %.6f %.6f %.6f %.6f\n' % tuple(i))
			file.write('scale: %.6f %.6f %.6f\n' % tuple(b.size))
	file.write('\n')

def write(filename, objects):
	
	def veckey3d(v):
		return round(v.x, 6), round(v.y, 6), round(v.z, 6)
		
	def veckey2d(v):
		return round(v.x, 6), round(v.y, 6)
	
	print 'OBJ Export path: "%s"' % filename
	temp_mesh_name = '~tmp-mesh'

	time1 = sys.time()
	scn = Scene.GetCurrent()

	file = open(filename, "w")
	
	# Write Header
	file.write('# Blender3D v%s EOL Armature File: %s\n' % (Blender.Get('version'), Blender.Get('filename').split('/')[-1].split('\\')[-1] ))
	file.write('# www.blender3d.org\n')
	
	# Get all armatures
	for ob_main in objects:
		for ob, ob_mat in BPyObject.getDerivedObjects(ob_main):
				write_armature(file,ob)
				write_poses(file,ob)
			
	file.close()
	
	print "Armature Export time: %.2f" % (sys.time() - time1)

def write_ui(filename):
	
	if not filename.lower().endswith('.eolarm'):
		filename += '.eolarm'
	
	if not BPyMessages.Warning_SaveOver(filename):
		return
	
	EXPORT_ARMATURE = Draw.Create(1)
	
	# removed too many options are bad!
	
	# Get USER Options
	pup_block = [\
	('Context...'),\
	('Armature', EXPORT_ARMATURE, 'Export Armature'),\
	]
	
	if not Draw.PupBlock('Export...', pup_block):
		return
	
	Window.EditMode(0)
	Window.WaitCursor(1)
	
	EXPORT_ARMATURE = EXPORT_ARMATURE.val
	
	base_name, ext = splitExt(filename)
	context_name = [base_name, '', '', ext] # basename, scene_name, framenumber, extension
	
	# Use the options to export the data using write()
	# def write(filename, objects, EXPORT_EDGES=False, EXPORT_NORMALS=False, EXPORT_MTL=True, EXPORT_COPY_IMAGES=False, EXPORT_APPLY_MODIFIERS=True):
	orig_scene = Scene.GetCurrent()
	export_scenes = [orig_scene]
	
	# Export all scenes.
	for scn in export_scenes:
		scn.makeCurrent() # If alredy current, this is not slow.
		context = scn.getRenderingContext()
		orig_frame = Blender.Get('curframe')
		
		# Export an animation?
		scene_frames = [orig_frame] # Dont export an animation.
		
		# Loop through all frames in the scene and export.
		for frame in scene_frames:
			
			Blender.Set('curframe', frame)
			export_objects = scn.objects.context
			
			full_path= ''.join(context_name)
			
			# erm... bit of a problem here, this can overwrite files when exporting frames. not too bad.
			# EXPORT THE FILE.
			write(full_path, export_objects)
		
		Blender.Set('curframe', orig_frame)
	
	# Restore old active scene.
	orig_scene.makeCurrent()
	Window.WaitCursor(0)


if __name__ == '__main__':
	Window.FileSelector(write_ui, 'Export EOL Armature', sys.makename(ext='.eolarm'))