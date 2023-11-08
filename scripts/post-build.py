#!/usr/bin/python3

from argparse import ArgumentParser, Namespace
from common import LIB_DIR, BuildType, is_platform_windows, is_platform_linux
from pathlib import Path
from shutil import copyfile
import os

BIN_EXTENSION = ".exe" if is_platform_windows() else ""
if is_platform_windows():
    DYNLIB_EXTENSION = ".dll"
elif is_platform_linux():
    DYNLIB_EXTENSION = ".so"
else:
    DYNLIB_EXTENSION = ".dylib"


def create_dir_if_doesnt_exist(path: str) -> None:
    if not os.path.exists(path):
        os.makedirs(path)


def copy_clslang_bin(args: Namespace) -> None:
    create_dir_if_doesnt_exist(args.build_dir / "bin")

    copyfile(
        LIB_DIR / "glslang" / args.build_type.value / "bin" / f"glslang{BIN_EXTENSION}",
        args.build_dir / "bin" / f"glslang{BIN_EXTENSION}",
    )
    os.chmod(args.build_dir / "bin" / f"glslang{BIN_EXTENSION}", 0o755)


def copy_validation_layers(args: Namespace) -> None:
    if not os.path.exists(LIB_DIR / "vvl" / args.build_type.value):
        return

    create_dir_if_doesnt_exist(args.build_dir / "validation_layers" / "lib")
    create_dir_if_doesnt_exist(args.build_dir / "validation_layers" / "layers")

    copyfile(
        LIB_DIR
        / "vvl"
        / args.build_type.value
        / "lib"
        / f"libVkLayer_khronos_validation{DYNLIB_EXTENSION}",
        args.build_dir
        / "validation_layers"
        / "lib"
        / f"libVkLayer_khronos_validation{DYNLIB_EXTENSION}",
    )
    copyfile(
        LIB_DIR
        / "vvl"
        / args.build_type.value
        / "share"
        / "vulkan"
        / "explicit_layer.d"
        / "VkLayer_khronos_validation.json",
        args.build_dir
        / "validation_layers"
        / "layers"
        / "VkLayer_khronos_validation.json",
    )


def get_parsed_args() -> Namespace:
    parser = ArgumentParser(
        prog="post-build", description="Script to run after build is finished."
    )
    build_type_group = parser.add_mutually_exclusive_group()
    build_type_group.add_argument(
        "-d",
        "--debug",
        action="store_const",
        dest="build_type",
        const=BuildType.DEBUG,
        help="Use debug configuration. (default)",
    )
    build_type_group.add_argument(
        "-r",
        "--release",
        action="store_const",
        dest="build_type",
        const=BuildType.RELEASE,
        help="Use release configuration.",
    )
    parser.set_defaults(build_type=BuildType.DEBUG)

    parser.add_argument("build_dir", type=Path, help="Project build directory.")

    return parser.parse_args()


if __name__ == "__main__":
    args = get_parsed_args()

    copy_clslang_bin(args)

    if not is_platform_windows():
        copy_validation_layers(args)
