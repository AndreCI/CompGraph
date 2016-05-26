# the exercise name is nothing else than the directory
get_filename_component(EXERCISENAME ${CMAKE_CURRENT_LIST_DIR} NAME)
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

# these files are copied only once
file(GLOB TEXTURES
    cube/cloudtop_rt.tga
    cube/cloudtop_lf.tga
    cube/cloudtop_up.tga
    cube/cloudtop_dn.tga
    cube/cloudtop_bk.tga
    cube/cloudtop_ft.tga
    grid/grass.jpg
    grid/rock.jpg
    grid/sand.jpg
    grid/snow.jpg
    grid/water.jpg

    )
copy_files_once(${TEXTURES})

# list all the files you want to copy everytime
# you build (ie, you want the shaders there)
file(GLOB SHADERS
  cube/cube_vshader.glsl
  cube/cube_fshader.glsl
  grid/grid_vshader.glsl
  grid/grid_fshader.glsl
  screenquad/screenquad_vshader.glsl
  screenquad/screenquad_fshader.glsl)
deploy_shaders_to_build_dir(${SHADERS})

add_executable(${EXERCISENAME} ${SOURCES} ${HEADERS} ${SHADERS})
target_link_libraries(${EXERCISENAME} ${COMMON_LIBS})
