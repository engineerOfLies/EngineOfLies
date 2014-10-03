# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####

# <pep8 compliant>

import os
import time

import bpy
import mathutils
import bpy_extras.io_utils


def name_compat(name):
    if name is None:
        return 'None'
    else:
        return name.replace(' ', '_')


def mesh_triangulate(me):
    import bmesh
    bm = bmesh.new()
    bm.from_mesh(me)
    bmesh.ops.triangulate(bm, faces=bm.faces)
    bm.to_mesh(me)
    bm.free()

def veckey3d(v):
    return round(v.x, 4), round(v.y, 4), round(v.z, 4)

def veckey2d(v):
    return round(v[0], 4), round(v[1], 4)

def findVertexGroupName(face, vWeightMap):
    """
    Searches the vertexDict to see what groups is assigned to a given face.
    We use a frequency system in order to sort out the name because a given vetex can
    belong to two or more groups at the same time. To find the right name for the face
    we list all the possible vertex group names with their frequency and then sort by
    frequency in descend order. The top element is the one shared by the highest number
    of vertices is the face's group
    """
    weightDict = {}
    for vert_index in face.vertices:
        vWeights = vWeightMap[vert_index]
        for vGroupName, weight in vWeights:
            weightDict[vGroupName] = weightDict.get(vGroupName, 0.0) + weight

    if weightDict:
        return max((weight, vGroupName) for vGroupName, weight in weightDict.items())[1]
    else:
        return '(null)'

def write_poses(file, ob, scene, frame_range):
    fw = file.write
    orig_frame = scene.frame_current
    if (len(frame_range) > 0):
        fw("poses:\n")
    for frame in frame_range:
        scene.frame_set(frame, 0.0)
        fw("  - frame: %i\n" % frame)
        bones = ob.pose.bones
        if (len(bones) > 0):
            fw("    bones:\n")
            for bone in bones:
                fw("    - bone: %s\n" % bone.name);
                fw('      matrix:\n')
                for i in bone.matrix:
                    fw('      - %.6f %.6f %.6f %.6f\n' % tuple(i))
    scene.frame_set(orig_frame, 0.0)

def write_armature_file(filepath, objects, scene, frame_range):
    file = open(filepath, "a", encoding="utf8", newline="\n")
    fw = file.write

    for ob_main in objects:
        # ignore dupli children
        obs = [(ob_main, ob_main.matrix_world)]
    
        for ob, ob_mat in obs:
            if (ob.type != "ARMATURE"):
                continue
            arm = ob.data;
            fw('armature: %s\n' % ob_main.name)
            fw('matrix:\n')
            for i in ob_mat:
                fw('  - %.6f %.6f %.6f %.6f\n' % tuple(i))
            bones = arm.bones
            if (len(bones) > 0):
                fw("bones:\n")
                for bone in bones:
                    fw("  - bone: %s\n" % bone.name);
                    if (bone.parent != None):
                        fw('    parent: %s\n' % bone.parent.name)
                    if (len(bone.children.keys())):
                        fw('    children:\n')
                        for child in bone.children:
                            fw('      - %s\n' % child.name)
                    fw('    matrix:\n')
                    for i in bone.matrix_local:
                        fw('      - %.6f %.6f %.6f %.6f\n' % tuple(i))


            write_poses(file,ob,scene,frame_range)
    return

def write_mesh_file(filepath, objects, scene,
               EXPORT_TRI=True,
               EXPORT_EDGES=True,
               EXPORT_SMOOTH_GROUPS=False,
               EXPORT_SMOOTH_GROUPS_BITFLAGS=False,
               EXPORT_NORMALS=False,
               EXPORT_UV=True,
               EXPORT_APPLY_MODIFIERS=True,
               EXPORT_BLEN_OBS=True,
               EXPORT_GROUP_BY_OB=False,
               EXPORT_GROUP_BY_MAT=False,
               EXPORT_KEEP_VERT_ORDER=False,
               EXPORT_POLYGROUPS=False,
               EXPORT_GLOBAL_MATRIX=None,
               EXPORT_PATH_MODE='AUTO',
               ):
    
    print('EOL Mesh Export path: %r' % filepath)

    time1 = time.time()

    file = open(filepath, "w", encoding="utf8", newline="\n")
    fw = file.write

    # Write Header
    fw('# Blender v%s EOL Model File: %r\n' % (bpy.app.version_string, os.path.basename(bpy.data.filepath)))
    fw('# www.blender.org\n')
    fw('# http://engineeroflies.github.io/EngineOfLies/\n')

    # Initialize totals, these are updated each object
    totverts = totuvco = totno = 1

    face_vert_index = 1
    copy_set = set()
    # Get all meshes
    fw('objects:\n')
    for ob_main in objects:
        # ignore dupli children
        if ob_main.parent and ob_main.parent.dupli_type in {'VERTS', 'FACES'}:
            # XXX
            print(ob_main.name, 'is a dupli child - ignoring')
            continue
        obs = []
        if ob_main.dupli_type != 'NONE':
            # XXX
            print('creating dupli_list on', ob_main.name)
            ob_main.dupli_list_create(scene)

            obs = [(dob.object, dob.matrix) for dob in ob_main.dupli_list]
            # XXX debug print
            print(ob_main.name, 'has', len(obs), 'dupli children')
        else:
            obs = [(ob_main, ob_main.matrix_world)]
    
        for ob, ob_mat in obs:
            uv_unique_count = no_unique_count = 0

            try:
                me = ob.to_mesh(scene, EXPORT_APPLY_MODIFIERS, 'PREVIEW', calc_tessface=False)
            except RuntimeError:
                me = None

            if me is None:
                continue
            fw('  - object: %s\n' % ob_main.name)

            me.transform(EXPORT_GLOBAL_MATRIX * ob_mat)

            if EXPORT_TRI:
                # _must_ do this first since it re-allocs arrays
                mesh_triangulate(me)

            if EXPORT_UV:
                faceuv = len(me.uv_textures) > 0
                if faceuv:
                    uv_texture = me.uv_textures.active.data[:]
                    uv_layer = me.uv_layers.active.data[:]
            else:
                faceuv = False

            me_verts = me.vertices[:]

            # Make our own list so it can be sorted to reduce context switching
            face_index_pairs = [(face, index) for index, face in enumerate(me.polygons)]
            # faces = [ f for f in me.tessfaces ]

            if EXPORT_EDGES:
                edges = me.edges
            else:
                edges = []

            if not (len(face_index_pairs) + len(edges) + len(me.vertices)):  # Make sure there is somthing to write

                # clean up
                bpy.data.meshes.remove(me)

                continue  # dont bother with this mesh.

            loops = me.loops

            if (EXPORT_SMOOTH_GROUPS or EXPORT_SMOOTH_GROUPS_BITFLAGS) and face_index_pairs:
                smooth_groups, smooth_groups_tot = me.calc_smooth_groups(EXPORT_SMOOTH_GROUPS_BITFLAGS)
                if smooth_groups_tot <= 1:
                    smooth_groups, smooth_groups_tot = (), 0
            else:
                smooth_groups, smooth_groups_tot = (), 0

            materials = me.materials[:]
            material_names = [m.name if m else None for m in materials]

            # avoid bad index errors
            if not materials:
                materials = [None]
                material_names = [name_compat(None)]

            # Sort by Material, then images
            # so we dont over context switch in the obj file.
            if EXPORT_KEEP_VERT_ORDER:
                pass
            else:
                if faceuv:
                    if smooth_groups:
                        sort_func = lambda a: (a[0].material_index,
                                               hash(uv_texture[a[1]].image),
                                               smooth_groups[a[1]] if a[0].use_smooth else False)
                    else:
                        sort_func = lambda a: (a[0].material_index,
                                               hash(uv_texture[a[1]].image),
                                               a[0].use_smooth)
                else:
                    # no materials
                    if smooth_groups:
                        sort_func = lambda a: smooth_groups[a[1] if a[0].use_smooth else False]
                    else:
                        sort_func = lambda a: a[0].use_smooth

                face_index_pairs.sort(key=sort_func)

                del sort_func

            # Set the default mat to no material and no image.
            contextMat = 0, 0  # Can never be this, so we will label a new material the first chance we get.
            contextSmooth = None  # Will either be true or false,  set bad to force initialization switch.

            vertGroups = ob.vertex_groups
            if vertGroups:
                fw('    groups:\n')
                for vgroup in vertGroups:
                    fw('    - %s\n' % vgroup.name)
            # Vertices
            fw('    vertices:\n')
            for v in me_verts:
                fw('      - vertex: %.6f %.6f %.6f\n' % v.co[:])
                if (len(v.groups) > 0):
                    fw('        groups:\n')
                    for group in v.groups:
                        fw('        - group: %d\n' % group.group)
                        fw('         weight: %.6f\n' % group.weight)

            # UV
            fw('    uvs:\n')
            if faceuv:
                # in case removing some of these dont get defined.
                uv = f_index = uv_index = uv_key = uv_val = uv_ls = None

                uv_face_mapping = [None] * len(face_index_pairs)

                uv_dict = {}
                uv_get = uv_dict.get
                for f, f_index in face_index_pairs:
                    uv_ls = uv_face_mapping[f_index] = []
                    for uv_index, l_index in enumerate(f.loop_indices):
                        uv = uv_layer[l_index].uv
                        uv_key = veckey2d(uv)
                        uv_val = uv_get(uv_key)
                        if uv_val is None:
                            uv_val = uv_dict[uv_key] = uv_unique_count
                            fw('      - %.6f %.6f\n' % uv[:])
                            uv_unique_count += 1
                        uv_ls.append(uv_val)

                del uv_dict, uv, f_index, uv_index, uv_ls, uv_get, uv_key, uv_val
                # Only need uv_unique_count and uv_face_mapping

            # NORMAL, Smooth/Non smoothed.
            if EXPORT_NORMALS:
                fw('    normals:\n')
                for v in me_verts:
                    fw('      - %.6f %.6f %.6f\n' % v.normal[:])
            if not faceuv:
                f_image = None

            # XXX
            if EXPORT_POLYGROUPS:
                # Retrieve the list of vertex groups
                vertGroupNames = ob.vertex_groups.keys()
                if vertGroupNames:
                    currentVGroup = ''
                    # Create a dictionary keyed by face id and listing, for each vertex, the vertex groups it belongs to
                    vgroupsMap = [[] for _i in range(len(me_verts))]
                    for v_idx, v_ls in enumerate(vgroupsMap):
                        v_ls[:] = [(vertGroupNames[g.group], g.weight) for g in me_verts[v_idx].groups]

                    
            fw('    faces:\n')
            for f, f_index in face_index_pairs:

                if faceuv:
                    tface = uv_texture[f_index]
                    f_image = tface.image

                #f_v = [(vi, me_verts[v_idx]) for vi, v_idx in enumerate(f.vertices)]
                f_v = [(vi, me_verts[v_idx], l_idx) for vi, (v_idx, l_idx) in enumerate(zip(f.vertices, f.loop_indices))]
                
                fw('      - face:\n')
                fw('        - vertices:\n')
                for vi, v, li in f_v:
                    fw("          - %d\n" % v.index)
                if EXPORT_NORMALS:
                    fw('        - normals:\n')
                    for vi, v, li in f_v:
                        fw("          - %d\n" % v.index)
                if faceuv:
                    fw('        - uvs:\n')
                    for vi, v, li in f_v:
                        fw("          - %d\n" % uv_face_mapping[f_index][vi])
                    

def _write(context, filepath,
              EXPORT_TRI,  # ok
              EXPORT_EDGES,
              EXPORT_SMOOTH_GROUPS,
              EXPORT_SMOOTH_GROUPS_BITFLAGS,
              EXPORT_NORMALS,  # not yet
              EXPORT_UV,  # ok
              EXPORT_MTL,
              EXPORT_APPLY_MODIFIERS,  # ok
              EXPORT_BLEN_OBS,
              EXPORT_GROUP_BY_OB,
              EXPORT_GROUP_BY_MAT,
              EXPORT_KEEP_VERT_ORDER,
              EXPORT_POLYGROUPS,
              EXPORT_CURVE_AS_NURBS,
              EXPORT_SEL_ONLY,  # ok
              EXPORT_ARMATURE,
              EXPORT_GLOBAL_MATRIX,
              EXPORT_PATH_MODE,
              ):  # Not used

    base_name, ext = os.path.splitext(filepath)
    context_name = [base_name, '', '', ext]  # Base name, scene name, frame number, extension

    scene = context.scene

    # Exit edit mode before exporting, so current object states are exported properly.
    if bpy.ops.object.mode_set.poll():
        bpy.ops.object.mode_set(mode='OBJECT')

    orig_frame = scene.frame_current

    # Export an animation?
    #if EXPORT_ARMATURE:
    #    scene_frames = range(scene.frame_start, scene.frame_end + 1)  # Up to and including the end frame.
    #else:
    #    scene_frames = [orig_frame]  # Dont export an animation.

    scene_frames = [orig_frame]  # Dont export an animation.
    # Loop through all frames in the scene and export.
    for frame in scene_frames:

        scene.frame_set(frame, 0.0)
        if EXPORT_SEL_ONLY:
            objects = context.selected_objects
        else:
            objects = scene.objects

        full_path = ''.join(context_name)

        # erm... bit of a problem here, this can overwrite files when exporting frames. not too bad.
        # EXPORT THE FILE.
        write_mesh_file(full_path, objects, scene,
                   EXPORT_TRI,
                   EXPORT_EDGES,
                   EXPORT_SMOOTH_GROUPS,
                   EXPORT_SMOOTH_GROUPS_BITFLAGS,
                   EXPORT_NORMALS,
                   EXPORT_UV,
                   EXPORT_APPLY_MODIFIERS,
                   EXPORT_BLEN_OBS,
                   EXPORT_GROUP_BY_OB,
                   EXPORT_GROUP_BY_MAT,
                   EXPORT_KEEP_VERT_ORDER,
                   EXPORT_POLYGROUPS,
                   EXPORT_GLOBAL_MATRIX,
                   EXPORT_PATH_MODE,
                   )
        
        if EXPORT_ARMATURE:
        # write armature
            write_armature_file(filepath, objects, scene, range(scene.frame_start, scene.frame_end + 1))
        
    if EXPORT_ARMATURE:
        
        scene_frames = range(scene.frame_start, scene.frame_end + 1)  # Up to and including the end frame.
        for frame in scene_frames:

            scene.frame_set(frame, 0.0)
            if EXPORT_SEL_ONLY:
                objects = context.selected_objects
            else:
                objects = scene.objects

            full_path = ''.join(context_name)
            # export pose

    scene.frame_set(orig_frame, 0.0)



"""
Currently the exporter lacks these features:
* multiple scene export (only active scene is written)
* particles
"""


def save(operator, context, filepath="",
         use_triangles=False,
         use_edges=True,
         use_normals=False,
         use_smooth_groups=False,
         use_smooth_groups_bitflags=False,
         use_uvs=True,
         use_materials=True,
         use_mesh_modifiers=True,
         use_blen_objects=True,
         group_by_object=False,
         group_by_material=False,
         keep_vertex_order=False,
         use_vertex_groups=False,
         use_nurbs=True,
         use_selection=True,
         use_armature=False,
         global_matrix=None,
         path_mode='AUTO'
         ):

    _write(context, filepath,
           EXPORT_TRI=use_triangles,
           EXPORT_EDGES=use_edges,
           EXPORT_SMOOTH_GROUPS=use_smooth_groups,
           EXPORT_SMOOTH_GROUPS_BITFLAGS=use_smooth_groups_bitflags,
           EXPORT_NORMALS=use_normals,
           EXPORT_UV=use_uvs,
           EXPORT_MTL=use_materials,
           EXPORT_APPLY_MODIFIERS=use_mesh_modifiers,
           EXPORT_BLEN_OBS=use_blen_objects,
           EXPORT_GROUP_BY_OB=group_by_object,
           EXPORT_GROUP_BY_MAT=group_by_material,
           EXPORT_KEEP_VERT_ORDER=keep_vertex_order,
           EXPORT_POLYGROUPS=use_vertex_groups,
           EXPORT_CURVE_AS_NURBS=use_nurbs,
           EXPORT_SEL_ONLY=use_selection,
           EXPORT_ARMATURE=use_armature,
           EXPORT_GLOBAL_MATRIX=global_matrix,
           EXPORT_PATH_MODE=path_mode,
           )

    return {'FINISHED'}
