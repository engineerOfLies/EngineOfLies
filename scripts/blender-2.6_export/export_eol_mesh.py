# ##### BEGIN GPL LICENSE BLOCK #####
#
#Copyright 2012 EngineerOfLies
#This file is part of the Engine of Lies game engine library

#The Engine of Lies (EOL) is free software: you can redistribute it and/or modify
#it under the terms of the GNU Lesser General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.

#EOL is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU Lesser General Public License for more details.

#You should have received a copy of the GNU Lesser General Public License
#along with the EOL game engine.  If not, see <http://www.gnu.org/licenses/>.

# ##### END GPL LICENSE BLOCK #####

# <pep8-80 compliant>

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

def write_armature(file, ob):
    if (ob.type != 'ARMATURE'):
        return
    file.write('armature: %s\n' % ob.name)
    armobmat = ob.matrix_world
    for i in armobmat:
        file.write('armmat: %.6f %.6f %.6f %.6f\n' % tuple(i))
    file.write('\n')

    boneList = []
    # parse armature data
    pose = ob.pose
    pose_items = pose.bones.items()
    for name, pbone in pose_items:
        bone = pbone.bone
        boneData = {}
        boneData['name'] = name
        boneData['parent'] = getattr(bone.parent, "name", None)
        boneData['matrix'] = bone.matrix_local
        boneData['children'] = []
        boneList.append(boneData)
    # build children list
    for bone in boneList:
        if (bone['parent'] == None):
            continue
        for parent in boneList:
            if (bone['parent'] == parent['name']):
                parent['children'].append(bone['name'])
                break;
    #write armature data
    file.write('bonecount: %i\n' % len(boneList))
    file.write('\n')
    for bone in boneList:
        file.write('bone: %s\n' % bone['name'])
        if (bone['parent'] != None):
            file.write('parent: %s\n' % bone['parent'])
        if (len(bone['children']) > 0):
            file.write('childcount: %i\n' % len(bone['children']))
            for child in bone['children']:
                file.write('child: %s\n' % child)
        for i in bone['matrix']:
            file.write('m: %.6f %.6f %.6f %.6f\n' % tuple(i))
        file.write('\n')
    file.write('\n')
        

def write_poses(file,ob,scene):
    if (ob.type != 'ARMATURE'):
        return
        
    orig_frame = scene.frame_current
    scene_frames = range(scene.frame_start, scene.frame_end + 1)
    file.write('framecount: %i\n' % len(scene_frames))
    for frame in scene_frames:
        scene.frame_set(frame, 0.0)
        file.write('frame: %i\n' % frame)
        pose = ob.pose
        pose_items = pose.bones.items()
        for name, pbone in pose_items:
            file.write('posebone: %s\n' % name)
            for i in pbone.matrix:
                file.write('lm: %.6f %.6f %.6f %.6f\n' % tuple(i))
            file.write('scale: %.6f %.6f %.6f\n' % tuple(pbone.scale))
    file.write('\n')
    scene.frame_set(orig_frame, 0.0)

def write_animation(filename, objects, scene):

    def veckey3d(v):
        return round(v.x, 6), round(v.y, 6), round(v.z, 6)

    def veckey2d(v):
        return round(v.x, 6), round(v.y, 6)

    print('EOL Armature Export path:',filename)

    time1 = time.time()

    file = open(filename, "w")

    # Write Header
    file.write('# Blender v%s EOL ARMATURE File: %r\n' % (bpy.app.version_string, os.path.basename(bpy.data.filepath)))
    file.write('# www.blender3d.org\n')

    for ob in objects:
        write_armature(file,ob)
        write_poses(file,ob,scene)
            
                

    file.close()

    print("Armature Export time: %.2f" % (time.time() - time1))


def write_file(filepath, objects, scene,
               EXPORT_TRI=False,
               EXPORT_EDGES=False,
               EXPORT_NORMALS=False,
               EXPORT_UV=True,
               EXPORT_APPLY_MODIFIERS=True,
               EXPORT_KEEP_VERT_ORDER=False,
               EXPORT_POLYGROUPS=False,
               EXPORT_GLOBAL_MATRIX=None,
               EXPORT_PATH_MODE='AUTO',
               ):
    '''
    Basic write function. The context and options must be already set
    This can be accessed externaly
    eg.
    write( 'c:\\test\\foobar.obj', Blender.Object.GetSelected() ) # Using default options.
    '''

    if EXPORT_GLOBAL_MATRIX is None:
        EXPORT_GLOBAL_MATRIX = mathutils.Matrix()

    def veckey3d(v):
        return round(v.x, 6), round(v.y, 6), round(v.z, 6)

    def veckey2d(v):
        return round(v[0], 6), round(v[1], 6)

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

    print('EOL MESH Export path: %r' % filepath)

    time1 = time.time()

    file = open(filepath, "w", encoding="utf8", newline="\n")

    # Write Header
    file.write('# Blender v%s EOL MESH File: %r\n' % (bpy.app.version_string, os.path.basename(bpy.data.filepath)))
    file.write('# www.blender.org\n')

    # Initialize totals, these are updated each object
    totverts = totuvco = totno = 1

    face_vert_index = 1

    globalNormals = {}

    # A Dict of Materials
    # (material.name, image.name):matname_imagename # matname_imagename has gaps removed.
    mtl_dict = {}

    copy_set = set()

    # aggregating across all objects
    vertexGroupList = []
    polygonList = []
    vertexList = []
    normalList = []
    uvList = []
    # Get all meshes
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

            try:
                me = ob.to_mesh(scene, EXPORT_APPLY_MODIFIERS, 'PREVIEW')
            except RuntimeError:
                me = None

            if me is None:
                continue

            me.transform(EXPORT_GLOBAL_MATRIX * ob_mat)

            if EXPORT_UV:
                faceuv = len(me.uv_textures) > 0
                if faceuv:
                    uv_layer = me.uv_textures.active.data[:]
            else:
                faceuv = False

            me_verts = me.vertices[:]

            # Make our own list so it can be sorted to reduce context switching
            face_index_pairs = [(face, index) for index, face in enumerate(me.faces)]
            # faces = [ f for f in me.faces ]

            if EXPORT_EDGES:
                edges = me.edges
            else:
                edges = []

            if not (len(face_index_pairs) + len(edges) + len(me.vertices)):  # Make sure there is somthing to write

                # clean up
                bpy.data.meshes.remove(me)

                continue  # dont bother with this mesh.

            if EXPORT_NORMALS and face_index_pairs:
                me.calc_normals()

            materials = me.materials[:]
            material_names = [m.name if m else None for m in materials]

            # avoid bad index errors
            if not materials:
                materials = [None]
                material_names = [""]

            # Sort by Material, then images
            # so we dont over context switch in the obj file.
            if EXPORT_KEEP_VERT_ORDER:
                pass
            elif faceuv:
                face_index_pairs.sort(key=lambda a: (a[0].material_index, hash(uv_layer[a[1]].image), a[0].use_smooth))
            elif len(materials) > 1:
                face_index_pairs.sort(key=lambda a: (a[0].material_index, a[0].use_smooth))
            else:
                # no materials
                face_index_pairs.sort(key=lambda a: a[0].use_smooth)

            # Set the default mat to no material and no image.
            contextMat = 0, 0  # Can never be this, so we will label a new material the first chance we get.
            contextSmooth = None  # Will either be true or false,  set bad to force initialization switch.

            # Vert
            for v in me_verts:
                vertex = {}
                vertex['coordinate'] = 'v %.6f %.6f %.6f\n' % v.co[:]
                vertex['groups'] = []
                for vg in v.groups:
                    group = {}
                    group['name'] = vg.group
                    group['weight'] = vg.weight
                    vertex['groups'].append(group)
                vertexList.append(vertex)
                
            # UV
            if faceuv:
                # in case removing some of these dont get defined.
                uv = uvkey = uv_dict = f_index = uv_index = None

                uv_face_mapping = [[0, 0, 0, 0] for i in range(len(face_index_pairs))]  # a bit of a waste for tri's :/

                uv_dict = {}  # could use a set() here
                uv_layer = me.uv_textures.active.data
                for f, f_index in face_index_pairs:
                    for uv_index, uv in enumerate(uv_layer[f_index].uv):
                        uvkey = veckey2d(uv)
                        try:
                            uv_face_mapping[f_index][uv_index] = uv_dict[uvkey]
                        except:
                            uv_face_mapping[f_index][uv_index] = uv_dict[uvkey] = len(uv_dict)
                            uvCoord = 'vt %.6f %.6f\n' % uv[:]
                            uvList.append(uvCoord)
                            
                uv_unique_count = len(uv_dict)

                del uv, uvkey, uv_dict, f_index, uv_index
                # Only need uv_unique_count and uv_face_mapping

            # NORMAL, Smooth/Non smoothed.
            if EXPORT_NORMALS:
                for f, f_index in face_index_pairs:
                    if f.use_smooth:
                        for v_idx in f.vertices:
                            v = me_verts[v_idx]
                            noKey = veckey3d(v.normal)
                            if noKey not in globalNormals:
                                globalNormals[noKey] = totno
                                totno += 1
                                normal = 'vn %.6f %.6f %.6f\n' % noKey
                                normalList.append(normal)
                    else:
                        # Hard, 1 normal from the face.
                        noKey = veckey3d(f.normal)
                        if noKey not in globalNormals:
                            globalNormals[noKey] = totno
                            totno += 1
                            normal = 'vn %.6f %.6f %.6f\n' % noKey
                            normalList.append(normal)
                            
            if not faceuv:
                f_image = None

            # XXX
            vertGroupNames = ob.vertex_groups.keys()
            for vgroup in vertGroupNames:
                vertexGroupList.append(vgroup)
                
            if EXPORT_POLYGROUPS:
                # Retrieve the list of vertex groups
                vertGroupNames = ob.vertex_groups.keys()

                currentVGroup = ''
                # Create a dictionary keyed by face id and listing, for each vertex, the vertex groups it belongs to
                vgroupsMap = [[] for _i in range(len(me_verts))]
                for v_idx, v_ls in enumerate(vgroupsMap):
                    v_ls[:] = [(vertGroupNames[g.group], g.weight) for g in me_verts[v_idx].groups]

            for f, f_index in face_index_pairs:
                f_smooth = f.use_smooth
                f_mat = min(f.material_index, len(materials) - 1)

                if faceuv:
                    tface = uv_layer[f_index]
                    f_image = tface.image

                # MAKE KEY
                if faceuv and f_image:  # Object is always true.
                    key = material_names[f_mat], f_image.name
                else:
                    key = material_names[f_mat], None  # No image, use None instead.

                # Write the vertex group
                if EXPORT_POLYGROUPS:
                    if ob.vertex_groups:
                        # find what vertext group the face belongs to
                        vgroup_of_face = findVertexGroupName(f, vgroupsMap)
                        if vgroup_of_face != currentVGroup:
                            currentVGroup = vgroup_of_face

                # CHECK FOR CONTEXT SWITCH
                if key == contextMat:
                    pass  # Context already switched, dont do anything
                else:
                    mat_data = mtl_dict.get(key)
                    if not mat_data:
                        # First add to global dict so we can export to mtl
                        # Then write mtl

                        # Make a new names from the mat and image name,
                        # converting any spaces to underscores with name_compat.

                        # If none image dont bother adding it to the name
                        if key[1] is None:
                            mat_data = mtl_dict[key] = ("%s" % name_compat(key[0])), materials[f_mat], f_image
                        else:
                            mat_data = mtl_dict[key] = ("%s_%s" % (name_compat(key[0]), name_compat(key[1]))), materials[f_mat], f_image

                contextMat = key
                if f_smooth != contextSmooth:
                    if f_smooth:  # on now off
                        contextSmooth = f_smooth
                    else:  # was off now on
                        contextSmooth = f_smooth

                f_v_orig = [(vi, me_verts[v_idx]) for vi, v_idx in enumerate(f.vertices)]

                if not EXPORT_TRI or len(f_v_orig) == 3:
                    f_v_iter = (f_v_orig, )
                else:
                    f_v_iter = (f_v_orig[0], f_v_orig[1], f_v_orig[2]), (f_v_orig[0], f_v_orig[2], f_v_orig[3])

                # support for triangulation
                for f_v in f_v_iter:
                    polygon = []
                    for vi, v in f_v:
                        side = "%d/%d/%d " % (v.index + totverts,
                                              totuvco + uv_face_mapping[f_index][vi],
                                              globalNormals[veckey3d(v.normal)])
                        polygon.append(side)  # vert, uv, normal
                    face_vert_index += len(f_v)
                    polygonList.append(polygon)

            # Make the indices global rather then per mesh
            totverts += len(me_verts)
            if faceuv:
                totuvco += uv_unique_count

            # clean up
            bpy.data.meshes.remove(me)

        if ob_main.dupli_type != 'NONE':
            ob_main.dupli_list_clear()

    # output aggregates
    vgl = set(vertexGroupList)
    file.write('vgroupcount: %i\n' % len(vgl))
    for vgroup in vgl:
        file.write('vgroup: %s\n' % vgroup)

    file.write('vertexcount: %i\n' % len(vertexList))
    for vertex in vertexList:
        file.write(vertex['coordinate']);
        for group in vertex['groups']:
            file.write('groupname: %s\n' % vertexGroupList[group['name']])
            file.write('weight: %s\n' % group['weight'])
            
    file.write('normalcount: %i\n' % len(normalList))
    for normal in normalList:
        file.write(normal);

    file.write('uvcount: %i\n' % uv_unique_count)
    for uvinfo in uvList:
        file.write(uvinfo)

    file.write('facecount: %i\n' % len(polygonList))
    for polygon in polygonList:
        file.write('f %s %s %s\n' % (polygon[0],polygon[1],polygon[2]))


            
    file.close()

    # copy all collected files.
    bpy_extras.io_utils.path_reference_copy(copy_set)

    print("OBJ Export time: %.2f" % (time.time() - time1))


def _write(context, filepath,
              EXPORT_TRI,  # ok
              EXPORT_EDGES,
              EXPORT_NORMALS,  # not yet
              EXPORT_UV,  # ok
              EXPORT_APPLY_MODIFIERS,  # ok
              EXPORT_KEEP_VERT_ORDER,
              EXPORT_POLYGROUPS,
              EXPORT_SEL_ONLY,  # ok
              EXPORT_GLOBAL_MATRIX,
              EXPORT_PATH_MODE,
              EXPORT_ARMATURE
              ):  # Not used

    base_name, ext = os.path.splitext(filepath)
    context_name = [base_name, '', '', ext]  # Base name, scene name, frame number, extension
    armature_context = [base_name,'.eolarm'] # armatures get the extension .eolarm
    
    scene = context.scene

    # Exit edit mode before exporting, so current object states are exported properly.
    if bpy.ops.object.mode_set.poll():
        bpy.ops.object.mode_set(mode='OBJECT')

    orig_frame = scene.frame_current

    # Export an animation?
    scene_frames = [orig_frame]  # Dont export an animation.

    if EXPORT_SEL_ONLY:
        objects = context.selected_objects
    else:
        objects = scene.objects

    full_path = ''.join(context_name)

    # erm... bit of a problem here, this can overwrite files when exporting frames. not too bad.
    # EXPORT THE FILE.
    write_file(full_path, objects, scene,
                EXPORT_TRI,
                EXPORT_EDGES,
                EXPORT_NORMALS,
                EXPORT_UV,
                EXPORT_APPLY_MODIFIERS,
                EXPORT_KEEP_VERT_ORDER,
                EXPORT_POLYGROUPS,
                EXPORT_GLOBAL_MATRIX,
                EXPORT_PATH_MODE,
                )

    if (EXPORT_ARMATURE):
        armpath = ''.join(armature_context)
        write_animation(armpath,objects,scene)



def save(operator, context, filepath="",
         use_triangles=True,
         use_edges=True,
         use_normals=True,
         use_uvs=True,
         use_apply_modifiers=False,
         keep_vertex_order=True,
         use_vertex_groups=True,
         use_selection=True,
         global_matrix=None,
         path_mode='AUTO',
         export_armature=True
         ):

    _write(context, filepath,
           EXPORT_TRI=use_triangles,
           EXPORT_EDGES=use_edges,
           EXPORT_NORMALS=use_normals,
           EXPORT_UV=use_uvs,
           EXPORT_APPLY_MODIFIERS=use_apply_modifiers,
           EXPORT_KEEP_VERT_ORDER=keep_vertex_order,
           EXPORT_POLYGROUPS=use_vertex_groups,
           EXPORT_SEL_ONLY=use_selection,
           EXPORT_GLOBAL_MATRIX=global_matrix,
           EXPORT_PATH_MODE=path_mode,
           EXPORT_ARMATURE=export_armature
           )

    return {'FINISHED'}
