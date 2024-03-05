import io
import os
import glob
import shutil
import subprocess

shaders = []
bin_dir = "bin"
target_env = "vulkan1.3"
target_dir = "Nebula/samples"


def collect_shaders():
    for shader_sources in ["glsl", "hlsl", "hair"]:
        shaders_in_dir = glob.glob(f"{shader_sources}/*")
        for sh in shaders_in_dir:
            sh_file = sh.rsplit("\\")[-1]
            if ("common.glsl" not in sh) and (".h" not in sh) and ("." in sh):
                shaders.append(f"{shader_sources}/{sh_file}")


def compile_shaders():
    collect_shaders()
    if not os.path.exists(f"{bin_dir}"):
        os.makedirs(f"{bin_dir}")

    counter = 0
    print(f"Compiling {len(shaders)} shader(s), target environment: {target_env}")
    for sh in shaders:
        command = f"glslangValidator -g -o {bin_dir}/{sh.rsplit('/')[-1]}.spv -V {sh} --target-env {target_env}"
        if "hlsl" in sh:
            command += " -e main"

        with subprocess.Popen(command, stdout=subprocess.PIPE, shell=True) as proc:
            for line in io.TextIOWrapper(proc.stdout, encoding="utf-8"):
                print(line.rstrip())


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
