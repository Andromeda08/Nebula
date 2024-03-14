import io
import os
import glob
import shutil
import subprocess

shaders = []
shader_dirs = ["glsl", "hlsl", "hair", "hair_hlsl"]
bin_dir = "bin"
target_env = "vulkan1.3"
target_dir = "Nebula/nebula"


def collect_shaders():
    for shader_sources in shader_dirs:
        shaders_in_dir = glob.glob(f"{shader_sources}/*")
        for sh in shaders_in_dir:
            sh_file = sh.rsplit("\\")[-1]
            if '.hlsli' not in sh and 'inc.glsl' not in sh and 'common.glsl' not in sh:
                shaders.append(f"{shader_sources}/{sh_file}")


def compile_glsl(shader):
    command = f"glslangValidator -g -o {bin_dir}/{shader.rsplit('/')[-1]}.spv -V {shader} --target-env {target_env}"
    with subprocess.Popen(command, stdout=subprocess.PIPE, shell=True) as proc:
        for line in io.TextIOWrapper(proc.stdout, encoding="utf-8"):
            print(line.rstrip())


def get_hlsl_profile(shader):
    if ".vert" in shader:
        return 'vs_6_1'
    elif ".frag" in shader:
        return 'ps_6_4'
    elif ".comp" in shader:
        return 'cs_6_1'
    elif ".rgen" or ".rchit" or '.rmiss' in shader:
        return 'lib_6_3'
    elif ".mesh" in shader:
        return 'ms_6_6'
    elif ".task" in shader:
        return 'as_6_6'
    else:
        return ''


def compile_hlsl(shader):
    command = f"dxc -spirv -T {get_hlsl_profile(shader)} -E main -fspv-target-env=vulkan1.3 {shader} -Fo {bin_dir}/{shader.rsplit('/')[-1]}.spv"
    with subprocess.Popen(command, stdout=subprocess.PIPE, shell=True) as proc:
        for line in io.TextIOWrapper(proc.stdout, encoding="utf-8"):
            print(line.rstrip())


def compile_shaders():
    collect_shaders()
    if not os.path.exists(f"{bin_dir}"):
        os.makedirs(f"{bin_dir}")

    counter = 0
    print(f"Compiling {len(shaders)} shader(s), target environment: {target_env}")
    for sh in shaders:
        print(f"- {sh}:", end=" ")
        if "hlsl" in sh:
            compile_hlsl(sh)
            print("done")
        else:
            compile_glsl(sh)


def copy_shaders():
    compiled_shaders = glob.glob(f"bin/*")
    for shader in compiled_shaders:
        shutil.copy2(shader, f"../cmake-build-debug/{target_dir}")

    msvc_build_exists = os.path.exists(f"../out/build/x64-debug/{target_dir}")
    if msvc_build_exists:
        for shader in compiled_shaders:
            shutil.copy2(shader, f"../out/build/x64-debug/{target_dir}")
    print(f"Copied {len(compiled_shaders)} shader(s) to build directory")


compile_shaders()
copy_shaders()
