#!BPY
# -*- coding: utf-8 -*-

"""
Name: 'eol mesh (.eolmesh)...'
Blender: 243
Group: 'Export'
Tooltip: 'Save an EOL Mesh File'
"""

__author__ = "Engineer of Lies"
__url__ = ['www.blender.org', 'http://engineeroflies.github.com/EngineOfLies/']
__version__ = "0.1"

__bpydoc__ = """\
This script is an exporter to EOL Mesh file format.
Based on the OBJ exporter.
Usage:

Select the object(s) you wish to export and run this script from
"File->Export" menu.  Selecting the default options from the popup box will be
good in most cases.
"""


# --------------------------------------------------------------------------
# EOL MESH export for Engine of Lies
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
import BPyObject
import BPySys
import BPyMessages

# Returns a tuple - path,extension.
# 'hello.eolmesh' >  ('hello', '.eolmesh')
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

def write(filename, objects,\
EXPORT_APPLY_MODIFIERS=True, EXPORT_ROTX90=True):
	'''
	Basic write function. The context and options must be alredy set
	This can be accessed externaly
	eg.
	write( 'c:\\test\\foobar.obj', Blender.Object.GetSelected() ) # Using default options.
	'''
	
	def veckey3d(v):
		return round(v.x, 6), round(v.y, 6), round(v.z, 6)
		
	def veckey2d(v):
		return round(v.x, 6), round(v.y, 6)
	
	print 'EOL MESH Export path: "%s"' % filename
	temp_mesh_name = '~tmp-mesh'
	time1 = sys.time()
	scn = Scene.GetCurrent()
	file = open(filename, "w")
	
	# Write Header
	file.write('# Blender3D v%s EOL MESH File: %s\n' % (Blender.Get('version'), Blender.Get('filename').split('/')[-1].split('\\')[-1] ))
	file.write('# www.blender3d.org\n')
	
	# Get the container mesh. - used for applying modifiers and non mesh objects.
	containerMesh = meshName = tempMesh = None
	for meshName in Blender.NMesh.GetNames():
		if meshName.startswith(temp_mesh_name):
			tempMesh = Mesh.Get(meshName)
			if not tempMesh.users:
				containerMesh = tempMesh
	if not containerMesh:
		containerMesh = Mesh.New(temp_mesh_name)
	
	if EXPORT_ROTX90:
		mat_xrot90= Blender.Mathutils.RotationMatrix(-90, 4, 'x')
		
	del meshName
	del tempMesh
	
	# Initialize totals, these are updated each object
	totverts = totuvco = totno = 1
	
	face_vert_index = 1
	
	globalNormals = {}
	
	# Get all meshs
	for ob_main in objects:
		for ob, ob_mat in BPyObject.getDerivedObjects(ob_main):
			# Will work for non meshes now! :)
			# getMeshFromObject(ob, container_mesh=None, apply_modifiers=True, vgroups=True, scn=None)
			me= BPyMesh.getMeshFromObject(ob, containerMesh, EXPORT_APPLY_MODIFIERS, False, scn)
			if not me:
				continue
			
			faceuv= me.faceUV
			edges = me.edges
			
			faces = [ f for f in me.faces ]
			if EXPORT_ROTX90:
				me.transform(ob_mat*mat_xrot90)
			else:
				me.transform(ob_mat)
			
			# Vert
			mesh = ob.getData()
			objmat = ob.getMatrix()
			for i in objmat:
				file.write('obm: %.6f %.6f %.6f %.6f\n' % tuple(i))
			vgrouplist = mesh.getVertGroupNames()
			file.write('vgroupcount: %i\n' % len(vgrouplist))
			for vgname in vgrouplist:
				file.write('vgroup: %s\n' % vgname)
			file.write('vertexcount: %i\n' % len(mesh.verts))
			for v in mesh.verts:
				file.write('v %.6f %.6f %.6f\n' % tuple(v.co))
				influences = mesh.getVertexInfluences(v.index)
				file.write('influence: %i\n' % len(influences))
				for name,weight in influences:
					file.write('groupname: %s\n' % name)
					file.write('weight: %f\n' % weight)
			
			# UV
			if faceuv:
				uv_face_mapping = [[0,0,0,0] for f in faces] # a bit of a waste for tri's :/
				
				uv_dict = {} # could use a set() here
				for f_index, f in enumerate(faces):
					
					for uv_index, uv in enumerate(f.uv):
						uvkey = veckey2d(uv)
						try:
							uv_face_mapping[f_index][uv_index] = uv_dict[uvkey]
						except:
							uv_face_mapping[f_index][uv_index] = uv_dict[uvkey] = len(uv_dict)
							file.write('vt %.6f %.6f\n' % tuple(uv))
				
				uv_unique_count = len(uv_dict)
				del uv, uvkey, uv_dict, f_index, uv_index
				# Only need uv_unique_count and uv_face_mapping
				file.write('uvcount: %i\n' % uv_unique_count)
			# NORMAL, Smooth/Non smoothed.
			for f in faces:
				if f.smooth:
					for v in f:
						noKey = veckey3d(v.no)
						if not globalNormals.has_key( noKey ):
							globalNormals[noKey] = totno
							totno +=1
							file.write('vn %.6f %.6f %.6f\n' % noKey)
				else:
					# Hard, 1 normal from the face.
					noKey = veckey3d(f.no)
					if not globalNormals.has_key( noKey ):
						globalNormals[noKey] = totno
						totno +=1
						file.write('vn %.6f %.6f %.6f\n' % noKey)
			file.write('normalcount: %i\n' % len(globalNormals))
			
			if not faceuv:
				f_image = None
			
			file.write('facecount: %i\n' % len(faces))
			for f_index, f in enumerate(faces):
				f_v= f.v
				f_smooth= f.smooth
				if faceuv:
					f_uv= f.uv
				
				file.write('f')
				if faceuv:
					if f_smooth: # Smoothed, use vertex normals
						for vi, v in enumerate(f_v):
							file.write( ' %d/%d/%d' % (\
								v.index+totverts,\
								totuvco + uv_face_mapping[f_index][vi],\
								globalNormals[ veckey3d(v.no) ])) # vert, uv, normal

					else: # No smoothing, face normals
						no = globalNormals[ veckey3d(f.no) ]
						for vi, v in enumerate(f_v):
							file.write( ' %d/%d/%d' % (\
								v.index+totverts,\
								totuvco + uv_face_mapping[f_index][vi],\
								no)) # vert, uv, normal
					
					face_vert_index += len(f_v)
				
				else: # No UV's
					if f_smooth: # Smoothed, use vertex normals
						for v in f_v:
							file.write( ' %d//%d' % (\
								v.index+totverts,\
								globalNormals[ veckey3d(v.no) ]))
					else: # No smoothing, face normals
						no = globalNormals[ veckey3d(f.no) ]
						for v in f_v:
							file.write( ' %d//%d' % (\
								v.index+totverts,\
								no))
						
				file.write('\n')
			
			# Write edges.
			LOOSE= Mesh.EdgeFlags.LOOSE
			for ed in edges:
				if ed.flag & LOOSE:
					file.write('f %d %d\n' % (ed.v1.index+totverts, ed.v2.index+totverts))
				
			# Make the indicies global rather then per mesh
			totverts += len(me.verts)
			if faceuv:
				totuvco += uv_unique_count
			me.verts= None
	file.close()
	
	
	# Now we have all our materials, save them
	
	print "MESH Export time: %.2f" % (sys.time() - time1)
	
	

def write_ui(filename):
	
	if not filename.lower().endswith('.eolmesh'):
		filename += '.eolmesh'
	
	if not BPyMessages.Warning_SaveOver(filename):
		return
	
	EXPORT_APPLY_MODIFIERS = Draw.Create(1)
	EXPORT_ROTX90 = Draw.Create(1)
	EXPORT_SEL_ONLY = Draw.Create(1)
	
	# removed too many options are bad!
	
	# Get USER Options
	pup_block = [\
	('Context...'),\
	('Selection Only', EXPORT_SEL_ONLY, 'Only export objects in visible selection. Else export whole scene.'),\
	('Object Prefs...'),\
	('Apply Modifiers', EXPORT_APPLY_MODIFIERS, 'Use transformed mesh data from each object. May break vert order for morph targets.'),\
	('Rotate X90', EXPORT_ROTX90 , 'Rotate on export so Blenders UP is translated into OBJs UP'),\
	]
	
	if not Draw.PupBlock('Export...', pup_block):
		return
	
	Window.EditMode(0)
	Window.WaitCursor(1)
	
	EXPORT_APPLY_MODIFIERS = EXPORT_APPLY_MODIFIERS.val
	EXPORT_ROTX90 = EXPORT_ROTX90.val
	EXPORT_SEL_ONLY = EXPORT_SEL_ONLY.val
	
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
			if EXPORT_SEL_ONLY:
				export_objects = scn.objects.context
			else:   
				export_objects = scn.objects
			
			full_path= ''.join(context_name)
			
			# erm... bit of a problem here, this can overwrite files when exporting frames. not too bad.
			# EXPORT THE FILE.
			write(full_path, export_objects,\
			EXPORT_APPLY_MODIFIERS,\
			EXPORT_ROTX90)
		
		Blender.Set('curframe', orig_frame)
	
	# Restore old active scene.
	orig_scene.makeCurrent()
	Window.WaitCursor(0)


if __name__ == '__main__':
	Window.FileSelector(write_ui, 'Export EOL Mesh', sys.makename(ext='.eolmesh'))